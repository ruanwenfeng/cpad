#pragma once
#include <assert.h>
#include <stdio.h>

#include <apr_pools.h>
#include <apr_tables.h>
#include <apr_errno.h>

#include <plugin.h>

#define FAIL_CHILD_RETRUN(rv, pool) \
    assert(pool != NULL);           \
    if (rv != APR_SUCCESS) {        \
        apr_pool_destroy(pool);     \
        return rv;                  \
    }
#define FAIL_CHILD_CONTINUE(rv, pool) \
    assert(pool != NULL);             \
    if (rv != APR_SUCCESS) {          \
        apr_pool_destroy(pool);       \
        continue;                     \
    }

#define PONITER_CHILD_CONTINUE(pointer, pool) \
    assert(pool != NULL);                     \
    assert(pointer != NULL);                  \
    if (pointer == NULL) {                    \
        apr_pool_destroy(pool);               \
        continue;                             \
    }

#define SUCCESS_CHILD_RETRUN(rv, pool) \
    apr_pool_destroy(pool);            \
    return rv;
#define FAIL_RETRUN(rv)        \
    assert(rv == APR_SUCCESS); \
    if (rv != APR_SUCCESS) {   \
        return rv;             \
    }
#define POINTER_RETRUN(poniter, rv) \
    assert(poniter != NULL);        \
    if (poniter == NULL) {          \
        return rv;                  \
    }
#define PONITER_CONTINUE(poniter) \
    assert(poniter != NULL);      \
    if (poniter == NULL) {        \
        continue;                 \
    }

#define FAIL_CONTINUE(rv)    \
    if (rv != APR_SUCCESS) { \
        continue;            \
    }
#define CREATE_CHILD_POOL(child, rv, pool)              \
    apr_pool_t*  child = NULL;                          \
    apr_status_t rv    = apr_pool_create(&child, pool); \
    FAIL_RETRUN(rv);

typedef struct plugin_manager_s {
    plugin_t** plugins;
} plugin_manager_t;

#ifdef __cplusplus
extern "C" {
#endif

apr_status_t init_plugin_system();
apr_status_t start_plugin_system();
apr_status_t shutdown_plugin_system();
apr_status_t release_plugin_system();

apr_status_t load_all_plugin(const char* plugin_folder, apr_pool_t* pool);

void* plugin_alloc(plugin_t* plugin, size_t size);

void print_plugin_info();
void print_plugin_active_info();

apr_pool_t*         get_root_pool();
apr_array_header_t* get_plugin_list();
apr_array_header_t* get_plugin_active_list();
void                set_root_pool(apr_pool_t*);

#ifdef __cplusplus
}
#endif