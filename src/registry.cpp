#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include "session-lock.h"

/** If Wayland display is named NULL, will default to WAYLAND_DISPLAY
 * environment variable.
 * Wayland client gets started by obtaining a reference to the wl_display.
 * This establishes a connection to the Wayland server. */
struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
/** The registry enumerates the globals available on the server.
 * The registry emits an event every time the server adds or removes a global. */
struct wl_registry *registry = NULL;
/** @note wl_interface is used to access Wayland protocols. */
struct wl_interface *lock_manager = NULL;

/** Fn to add to the Wayland server global registry.
 * @param uint32_t name is more like an ID and identifies this resource.
 * @param const char *interface tells what API the resource implements
 * (distinguishes things like wl_output from a wl_seat).
 */
void global_add(void *data, struct wl_registry *registry, uint32_t name,
        const char *interface, uint32_t version)
{
    // TODO
}

/** Fn to remove from the Wayland server global registry. */
void global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    // TODO
}

/** Need two listeners (proxy objects), add proxy objects and remove proxy objects. */
/** Need to listen to events by providing an implementation of a
 * wl_registry_listener. */
struct wl_registry_listener registry_listener = {
    .global = global_add,
    .global_remove = global_remove
};

static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    /** wl_registry object has string name=interface (which registry object it is),
     * id. */
    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = static_cast<wl_compositor*>(
                wl_registry_bind(registry, id, &wl_compositor_interface, 1));
    }
    if (strcmp(interface, "ext_session_lock_manager_v1") == 0) {
        lock_manager = static_cast<wl_interface*>(
                wl_registry_bind(registry, id,
                    &ext_session_lock_manager_v1_interface, 1));
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry,
        uint32_t id)
{
    printf("Got a registry losing event for %d\n", id);
}

int main(int argc, char **argv)
{
    /** Attach a listener to the registry. */
    void *data = NULL; // arbitrary data
                       //
    /** Get registry of display, type=wl_registry. */
    registry = wl_display_get_registry(display);
    /** Add listener to registry, type=&wl_registry_listener. */
    wl_registry_add_listener(registry, &registry_listener, NULL);

    display = wl_display_connect(NULL);

    if (display == NULL) {
        fprintf(stderr, "Can't connect to display\n");
        exit(1);
    }
    printf("Connected to display\n");

    /** Dispatches the default queue. */
    // during wl_display_dispatch, global_add fn is called for each global on
    // the server. subsequent calls may call global_remove when the server
    // destroys globals.
    wl_display_dispatch(display);

    /** Roundtrip will block until the server responds - fn won't return until
     * server has processed all currently issued requests.
     * @note Can slow down the system, or deadlock. */
    wl_display_roundtrip(display);

    if (compositor == NULL) {
        fprintf(stderr, "Can't find compositor\n");
        exit(1);
    } else {
        fprintf(stderr, "Found compositor\n");
    }

    wl_display_disconnect(display);
    printf("Disconnected from display\n");

    exit(0);
}
