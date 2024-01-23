#define _POSIX_C_SOURCE 200112L
#define WLR_USE_UNSTABLE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/log.h>

/** Struct for holding the compositor's state */
struct agge_server {
    struct wl_display *wl_display;
    struct wl_event_loop *wl_event_loop;
    /** Server backend = wlroots - mice, keyboard, monitors, etc. */
    struct wlr_backend *backend;
    struct wlr_renderer *rendered;
    struct wlr_session **session;
};

int main(int argc, char **argv)
{
    // local instantiation of server
    struct agge_server server;

    server.wl_display = wl_display_create();
    assert(server.wl_display);
    server.wl_event_loop = wl_display_get_event_loop(server.wl_display);
    assert(server.wl_event_loop);

    /** wlroots provides a help fn for automatically choosing the most
     * appropriate backend based on the user's environment */
    server.backend = wlr_backend_autocreate(server.wl_display, server.session);
    assert(server.backend);

    /** Start the backend and enter the Wayland event loop. */
    /** If backend fails to start, print stderr, destroy Wayland display, and
     * return error. */
    if (!wlr_backend_start(server.backend)) {
        fprintf(stderr, "Failed to start backend\n");
        wl_display_destroy(server.wl_display);
        return 1;
        }

    /** Add a UNIX socket to the Wayland display. */
    const char *socket = wl_display_add_socket_auto(server.wl_display);
    if (!socket) {
        wlr_backend_destroy(server.backend);
        return 1;
    }

    /** Otherwise, run server. */
    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s",
            socket);
    wl_display_run(server.wl_display);
    wl_display_destroy(server.wl_display);

    return 0;
}
