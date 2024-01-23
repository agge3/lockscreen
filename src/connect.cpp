#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>

#include <iostream>

struct wl_display *display = NULL;

int main(int argc, char **argv)
{
    display = wl_display_connect(NULL);
    if (display == NULL) {
        std::cerr << "Can't connect to display\n";
        exit(true);
    }
    std::cout << "Connected to display\n";

    wl_display_disconnect(display);
    std::cout << "Disconnected from display\n";

    exit(true);
}
