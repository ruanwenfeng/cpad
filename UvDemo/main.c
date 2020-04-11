
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <stdio.h>
#include <crtdbg.h>


#include <apr_pools.h>

#include <core.h>

int main(int argc, char* argv[]) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

   apr_status_t rv  = init_plugin_system();
   FAIL_RETRUN(rv);

   rv              = load_all_plugin("C:/Users/ruanw/source/repos/d12/build/demoplugin", get_root_pool());
   
   char error[256] = { 0 };
   if (rv != APR_SUCCESS) {
       fprintf(stderr, "load plugin failed: %s", apr_strerror(rv, error, 256));
       return rv;
   }

    // LoadLibrary(L"advapi32.dll");

    

    // apr_pool_t* pmain  = NULL;
    // rv                 = apr_pool_create(&pmain, NULL);


    // char* p = calloc(99, 100);

    // uv_loop_t *loop = apr_palloc(pmain,sizeof(uv_loop_t));
    // uv_loop_init(loop);

    // printf("Now quitting.\n");

    // apr_pool_cleanup_register(pmain, loop, uv_loop_close, apr_pool_cleanup_null);

    // uv_run(loop, UV_RUN_DEFAULT);

    print_plugin_info();

    start_plugin_system();



    shutdown_plugin_system();

    release_plugin_system();
    fprintf(stderr, "app exit");

    return 0;
}
// LoadLibrary(L"advapi32.dll");
//#pragma warning(disable:4244 4267)
