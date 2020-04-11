#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/plugin.h"

#define PLUGIN_NAME "demo plugin"

#define PLUGIN_ID "com.plugin.demo"

#define PLUGIN_VERSION "1.0.1"

#define PLUGIN_DEPEND "com.plugin.aaaa@1.x", "com.plugin.bbbb@2.2.x"

#define PLUGIN_DESCRIPTION "a simple plugin demo"

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

    plugin->plugin_name  = PLUGIN_NAME;
    plugin->plugin_id    = PLUGIN_ID;
    plugin->plugin_id    = PLUGIN_VERSION;
    plugin->description  = PLUGIN_DESCRIPTION;
    const char* depend[] = { PLUGIN_DEPEND };

    plugin->depend_on   = plugin->plugin_alloc(plugin, _countof(depend) * sizeof(char*));
    plugin->depend_size = _countof(depend);
    for (size_t i = 0; i < _countof(depend); i++) {
        plugin->depend_on[i] = depend[i];
    }

    signal_plugin = plugin;
    return 0;
}
