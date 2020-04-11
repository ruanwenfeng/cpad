#pragma once

struct plugin_s;

typedef int InitPlugin(struct plugin_s* plguin);
typedef int StartPlugin();
typedef int StopPlugin();
typedef int ReleasePlugin();

typedef void* PluginAlloc(struct plugin_s* plugin, size_t size);

struct plugin_private_s;


typedef struct plugin_s {
    char        plugin_name[128];  // plugin name, for display
    char        plugin_id[32];
    char        plugin_version[32];
    char**      depend_on;
    char*       description;
    const void* parameter;
    struct plugin_private_s* private;

    InitPlugin*    init_plugin;
    StartPlugin*   start_plugin;
    StopPlugin*    stop_plugin;
    ReleasePlugin* release_plugin;

    PluginAlloc*   plugin_alloc;

} plugin_t;