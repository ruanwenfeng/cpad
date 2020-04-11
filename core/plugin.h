#pragma once

struct plugin_s;

typedef int InitPlugin(struct plugin_s* plguin);
typedef int StartPlugin();
typedef int StopPlugin();
typedef int ReleasePlugin();

typedef enum plugin_status_s {
    PLUGIN_UN_INIT    = 0,
    PLUGIN_INITIALIZE = 1,

    PLUGIN_STARTED  = 2,
    PLUGIN_STARTING = 3,

    PLUGIN_STOPED  = 4,
    PLUGIN_STOPING = 5,
} plugin_status_t;

typedef void* PluginAlloc(struct plugin_s* plugin, size_t size);

struct plugin_private_s;

typedef struct plugin_s {
    const char*     plugin_name;  // plugin name, for display
    const char*     plugin_id;
    const char*     plugin_version;
    const char**    depend_on;
    unsigned int    depend_size;
    const char*     description;
    const void*     parameter;
    plugin_status_t status;
    struct plugin_private_s* private_data;

    InitPlugin*    init_plugin;
    StartPlugin*   start_plugin;
    StopPlugin*    stop_plugin;
    ReleasePlugin* release_plugin;

    PluginAlloc* plugin_alloc;

} plugin_t;