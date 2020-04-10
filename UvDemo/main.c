
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 


#include <assert.h>
#include <stdio.h>

#include <apr_pools.h>
#include <uv.h>

apr_status_t checker_cleanup(void* data) {
    printf("checker_cleanup: %s\n", (char*)data);
    return TRUE ? APR_SUCCESS : APR_EGENERAL;
}
apr_status_t success_cleanup(void* data) {
    printf("success_cleanup: %s\n", (char*)data);
    return APR_SUCCESS;
}

int main(int argc, char* argv[]) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    
    LoadLibrary(L"advapi32.dll");
    apr_status_t rv = apr_initialize();

    assert(rv == APR_SUCCESS);

    apr_pool_t* pmain  = NULL;
    rv                 = apr_pool_create(&pmain, NULL);
    assert(rv == APR_SUCCESS);
    assert(pmain);

    char* p = calloc(99, 100);

    uv_loop_t *loop = apr_palloc(pmain,sizeof(uv_loop_t));
    uv_loop_init(loop);

    printf("Now quitting.\n");

    apr_pool_cleanup_register(pmain, loop, uv_loop_close, apr_pool_cleanup_null);

    uv_run(loop, UV_RUN_DEFAULT);




    apr_pool_clear(pmain);
    apr_pool_destroy(pmain);


    apr_terminate();
    
    return 0;
}
// LoadLibrary(L"advapi32.dll");
//#pragma warning(disable:4244 4267)
