#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "core/plugin.h"

#define PLUGIN_DECLARE(type) __declspec(dllexport) type __stdcall

PLUGIN_DECLARE(int) start_plugin() {
    fprintf(stdout, "start plugin demoplugin\n");
    return 1;
}

PLUGIN_DECLARE(int) stop_plugin() {
    fprintf(stdout, "stop plugin demoplugin\n");
    return 2;
}

PLUGIN_DECLARE(int) release_plugin() {
    fprintf(stdout, "release plugin demoplugin\n");
    return 3;
}

static plugin_t* signal_plugin = NULL;

PLUGIN_DECLARE(int) init_plugin(plugin_t* plugin) {
    if (signal_plugin != NULL) {
        fprintf(stdout, "already init\n");
        return -1;
    }

    fprintf(stdout, "init plugin plugindemo\n");
    plugin->init_plugin    = init_plugin;
    plugin->start_plugin   = start_plugin;
    plugin->stop_plugin    = stop_plugin;
    plugin->release_plugin = release_plugin;
    strcpy_s(plugin->plugin_name, 127, "demoplugin");
    signal_plugin          = plugin;
    return 0;
}
