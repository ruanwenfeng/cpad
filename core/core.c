#include <stdlib.h>
#include <crtdbg.h>

#include <apr_dso.h>
#include <apr_errno.h>
#include <apr_file_info.h>
#include <apr_fnmatch.h>
#include <apr_strings.h>

#include "core.h"
#include "plugin.h"

typedef struct plugin_private_s {
    apr_pool_t*       pool;
    apr_dso_handle_t* dso_h;
    int*              denped_check;
} plugin_private_t;

static apr_status_t dso_cleanup(apr_dso_handle_t* data) {
    fprintf(stdout, "unload load: 0X%p\n", data);

    return apr_dso_unload(data);
}

static apr_pool_t*         root_pool          = NULL;
static apr_array_header_t* plugin_list        = NULL;
static apr_array_header_t* plugin_active_list = NULL;

apr_status_t load_plugin_dso(const char* file_name, plugin_t** plugin, apr_pool_t* pool) {
    apr_dso_handle_t*    dso_h       = NULL;
    apr_dso_handle_sym_t init_plugin = NULL;

    apr_status_t rv = apr_dso_load(&dso_h, file_name, pool);
    fprintf(stdout, "load: %s 0X%p\n", file_name, dso_h);

    FAIL_RETRUN(rv);

    apr_pool_cleanup_register(pool, dso_h, dso_cleanup, apr_pool_cleanup_null);

    rv = apr_dso_sym(&init_plugin, dso_h, "init_plugin");
    FAIL_RETRUN(rv);

    *plugin = apr_pcalloc(pool, sizeof(plugin_t));
    POINTER_RETRUN(*plugin, rv);

    plugin_private_t* plugin_private = apr_pcalloc(pool, sizeof(plugin_private_t));
    POINTER_RETRUN(plugin_private, rv);

    plugin_private->pool  = pool;
    plugin_private->dso_h = dso_h;
    (*plugin)->private_data       = plugin_private;
    (*plugin)->plugin_alloc  = plugin_alloc;

    rv = ((InitPlugin*)init_plugin)(*plugin);
    FAIL_RETRUN(rv);
    (*plugin)->status = PLUGIN_INITIALIZE;
    return rv;
}

apr_status_t load_all_plugin(const char* plugin_folder, apr_pool_t* pool) {

    CREATE_CHILD_POOL(tmp, rv, pool);

    char* search_folder = NULL;
    rv                  = apr_filepath_merge(&search_folder, plugin_folder, "*.dll", APR_FILEPATH_NOTABOVEROOT, pool);
    FAIL_CHILD_RETRUN(rv, tmp);

    apr_array_header_t* result = NULL;
    rv                         = apr_match_glob(search_folder, &result, tmp);
    FAIL_CHILD_RETRUN(rv, tmp);

    char* default_path = NULL;
    rv                 = apr_filepath_get(&default_path, APR_FILEPATH_NATIVE, tmp);
    FAIL_CHILD_RETRUN(rv, tmp);

    rv = apr_filepath_set(plugin_folder, tmp);
    FAIL_CHILD_RETRUN(rv, tmp);

    char** list = (char**)result->elts;
    for (int i = 0; i < result->nelts; i++) {
        CREATE_CHILD_POOL(plugin_pool, rv, pool);
        plugin_t*            plugin      = NULL;
        rv = load_plugin_dso(list[i], &plugin, plugin_pool);
        FAIL_CHILD_CONTINUE(rv, plugin_pool);
        PONITER_CHILD_CONTINUE(plugin, plugin_pool);

        APR_ARRAY_PUSH(get_plugin_list(), plugin_t*) = plugin;

        fprintf(stdout, "load complete: %s\n", list[i]);
    }
    rv = apr_filepath_set(default_path, tmp);
    FAIL_CHILD_RETRUN(rv, tmp);

    SUCCESS_CHILD_RETRUN(rv, tmp);
}

void* plugin_alloc(struct plugin_s* plugin, size_t size) {
    return apr_palloc(plugin->private_data->pool, size);
}

void print_plugin_info() {
    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;
    fprintf(stdout, "plugin size: %zu\n", sizeof(plugin_t));
    fprintf(stdout, "plugin count: %d\n", get_plugin_list()->nelts);
    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        fprintf(stdout, "plugin name: %s\n", plugins[i]->plugin_name);
        fprintf(stdout, "plugin description: %s\n", plugins[i]->description);
        for (size_t j = 0; j < plugins[i]->depend_size; j++) {
            fprintf(stdout, "plugin depend on: %s\n", plugins[i]->depend_on[j]);
        }
    }
}
void print_plugin_active_info() {

    fprintf(stdout, "active plugin info ------------------\n");
    fprintf(stdout, "plugin size: %zu\n", sizeof(plugin_t));
    apr_array_header_t** plugin_lists = (apr_array_header_t**)get_plugin_active_list()->elts;
    for (int i = 0; i < get_plugin_active_list()->nelts; i++) {
        plugin_t** plugins = (plugin_t**)plugin_lists[i]->elts;
        fprintf(stdout, "plugin count: %d\n", plugin_lists[i]->nelts);
        for (int j = 0; j < plugin_lists[i]->nelts; j++) {
            fprintf(stdout, "plugin name: %s\n", plugins[j]->plugin_name);
            fprintf(stdout, "plugin description: %s\n", plugins[j]->description);
            fprintf(stdout, "plugin plugin id: %s\n", plugins[j]->plugin_id);
            for (size_t k = 0; k < plugins[j]->depend_size; k++) {
                fprintf(stdout, "plugin depend on: %s\n", plugins[j]->depend_on[k]);
            }
        }
    }
    fprintf(stdout, "active plugin info ------------------\n");
}
apr_array_header_t* get_plugin_list() {
    return plugin_list;
}

apr_array_header_t* get_plugin_active_list() {
    return plugin_active_list;
}

apr_pool_t* get_root_pool() {
    return root_pool;
}

void set_root_pool(apr_pool_t* pool) {
    root_pool = pool;
}

apr_status_t init_plugin_system() {
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif
    apr_status_t rv = apr_initialize();
    FAIL_RETRUN(rv);
    rv = apr_pool_create(&root_pool, NULL);
    FAIL_RETRUN(rv);
    POINTER_RETRUN(get_root_pool(), rv);

    plugin_list        = apr_array_make(get_root_pool(), 8, sizeof(plugin_t*));
    plugin_active_list = apr_array_make(get_root_pool(), 4, sizeof(apr_array_header_t*));

    assert(get_plugin_list() != NULL);

    return rv;
}

BOOL check_plugin_version(const char* plugin_dep, apr_array_header_t* active_plugin) {
    if (active_plugin == NULL) return FALSE;
    // apr_array_header_t** plugin_lists = (apr_array_header_t**)active_plugin->elts;
    // for (int i = 0; i < get_plugin_active_list()->nelts; i++) {
    plugin_t** plugins = (plugin_t**)active_plugin->elts;
    for (int j = 0; j < active_plugin->nelts; j++) {
        size_t id_length      = strlen(plugins[j]->plugin_id);
        size_t version_length = strlen(plugins[j]->plugin_version);
        size_t dep_length     = strlen(plugin_dep);
        int    id_queal       = strncmp(plugins[j]->plugin_id, plugin_dep, min(version_length, dep_length));
        if (id_queal != 0) continue;
        return TRUE;
    }
    // }
    return FALSE;
}

apr_status_t start_plugin_system() {
    plugin_t**          plugins         = (plugin_t**)get_plugin_list()->elts;
    apr_array_header_t* pre_start_level = NULL;
    int                 start_nums      = 0;
    apr_status_t        rv              = 0;
    do {
        start_nums                              = 0;
        apr_array_header_t* current_start_level = apr_array_make(get_root_pool(), 4, sizeof(apr_array_header_t*));
        PONITER_CONTINUE(current_start_level);

        for (int i = 0; i < get_plugin_list()->nelts; i++) {
            if (plugins[i]->status == PLUGIN_STARTED) continue;  // skip started plugin
            if (plugins[i]->private_data->denped_check == NULL) plugins[i]->private_data->denped_check = apr_pcalloc(plugins[i]->private_data->pool, plugins[i]->depend_size * sizeof(int));
            unsigned int check = 0;
            for (size_t j = 0; j < plugins[i]->depend_size && pre_start_level != NULL; j++) {
                if (!plugins[i]->private_data->denped_check[j]) plugins[i]->private_data->denped_check[j] = check_plugin_version(plugins[i]->depend_on[j], pre_start_level);
                if (plugins[i]->private_data->denped_check[j]) ++check;
            }
            if (check < plugins[i]->depend_size) continue;

            plugins[i]->status = PLUGIN_STARTING;
            rv                 = plugins[i]->start_plugin();
            FAIL_CONTINUE(rv);
            plugins[i]->status = PLUGIN_STARTED;

            ++start_nums;
            APR_ARRAY_PUSH(current_start_level, plugin_t*) = plugins[i];
            fprintf(stdout, "start plugin: %s\n", plugins[i]->plugin_id);
        }
        pre_start_level                                               = current_start_level;
        APR_ARRAY_PUSH(get_plugin_active_list(), apr_array_header_t*) = current_start_level;

    } while (start_nums);

    return APR_SUCCESS;
}

apr_status_t shutdown_plugin_system() {

    apr_array_header_t** plugin_lists = (apr_array_header_t**)get_plugin_active_list()->elts;
    for (int i = get_plugin_active_list()->nelts - 1; i >= 0; i--) {
        plugin_t** plugins = (plugin_t**)plugin_lists[i]->elts;
        for (int j = plugin_lists[i]->nelts - 1; j >= 0; j--) {
            plugins[j]->stop_plugin();
        }
    }

    return APR_SUCCESS;
}

apr_status_t release_plugin_system() {

    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;

    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        plugins[i]->release_plugin();
    }
    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        apr_pool_t* pool = plugins[i]->private_data->pool;
        apr_pool_clear(pool);
        apr_pool_destroy(pool);
    }
    apr_pool_clear(get_root_pool());
    apr_pool_destroy(get_root_pool());
    apr_terminate();
    return APR_SUCCESS;
}
