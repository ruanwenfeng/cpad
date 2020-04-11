#include <stdio.h>
#include <stdlib.h>

#include <apr_dso.h>
#include <apr_errno.h>
#include <apr_file_info.h>
#include <apr_fnmatch.h>

#include "core.h"
#include "plugin.h"

typedef struct plugin_private_s {
    apr_pool_t*       pool;
    apr_dso_handle_t* dso_h;
} plugin_private_t;
static apr_status_t dso_cleanup(apr_dso_handle_t* data) {
    fprintf(stdout, "unload load: 0X%p\n", data);

    return apr_dso_unload(data);
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

        apr_dso_handle_t*    dso_h       = NULL;
        apr_dso_handle_sym_t init_plugin = NULL;

        rv = apr_dso_load(&dso_h, list[i], plugin_pool);
        fprintf(stdout, "load: %s 0X%p\n", list[i], dso_h);

        FAIL_CONTINUE(rv);
        
        apr_pool_cleanup_register(plugin_pool, dso_h, dso_cleanup, apr_pool_cleanup_null);

        rv = apr_dso_sym(&init_plugin, dso_h, "init_plugin");
        FAIL_CONTINUE(rv);

        plugin_t* plugin = apr_palloc(plugin_pool, sizeof(plugin_t));
        PONITER_CHILD_CONTINUE(plugin, plugin_pool);

        plugin_private_t* plugin_private = apr_palloc(plugin_pool, sizeof(plugin_private_t));
        PONITER_CHILD_CONTINUE(plugin_private, plugin_pool);
        
        plugin_private->pool             = plugin_pool;
        plugin_private->dso_h            = dso_h;
        plugin->private                  = plugin_private;
        plugin->plugin_alloc             = plugin_alloc;

        rv = ((InitPlugin*)init_plugin)(plugin);
        FAIL_CHILD_CONTINUE(rv, plugin_pool);

        APR_ARRAY_PUSH(get_plugin_list(), plugin_t*) = plugin;

        fprintf(stdout, "load complete: %s\n", list[i]);
    }
    rv = apr_filepath_set(default_path, tmp);
    FAIL_CHILD_RETRUN(rv, tmp);

    SUCCESS_CHILD_RETRUN(rv, tmp);
}

void*        plugin_alloc(struct plugin_s* plugin, size_t size){
    return apr_palloc(plugin->private->pool, size);
}

void print_plugin_info() {
    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;
    fprintf(stdout, "plugin size: %d\n", get_plugin_list()->nelts);
    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        fprintf(stdout, "plugin name: %s\n", plugins[i]->plugin_name);
    }
}

apr_array_header_t* get_plugin_list() {
    return plugin_list;
}

apr_pool_t* get_root_pool() {
    return root_pool;
}

void set_root_pool(apr_pool_t* pool) {
    root_pool = pool;
}

apr_status_t init_plugin_system() {
    apr_status_t rv = apr_initialize();
    FAIL_RETRUN(rv);
    rv = apr_pool_create(&root_pool, NULL);
    FAIL_RETRUN(rv);
    POINTER_RETRUN(get_root_pool(), rv);

    plugin_list = apr_array_make(get_root_pool(), 8, sizeof(plugin_t*));
    assert(get_plugin_list() != NULL);

    return rv;
}

apr_status_t start_plugin_system() {
    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;
    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        plugins[i]->start_plugin();
    }
    return APR_SUCCESS;
}

apr_status_t shutdown_plugin_system() {
    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;

    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        plugins[i]->stop_plugin();
    }
    return APR_SUCCESS;
}

apr_status_t release_plugin_system() {

    plugin_t** plugins = (plugin_t**)get_plugin_list()->elts;

    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        plugins[i]->release_plugin();
    }
    for (int i = 0; i < get_plugin_list()->nelts; i++) {
        apr_pool_t* pool = plugins[i]->private->pool;
        apr_pool_clear(pool);
        apr_pool_destroy(pool);
    }
    apr_pool_clear(get_root_pool());
    apr_pool_destroy(get_root_pool());
    apr_terminate();
    return APR_SUCCESS;
}

