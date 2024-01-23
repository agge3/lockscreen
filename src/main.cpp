#include "type-name.h"

#include <elogind/sd-login.h>
// instead of systemd...
//#include <systemd/sd-login.h>
#include <dbus/dbus.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

// c libs
#include <cstdlib>

// linux libs
#include <unistd.h>
#include <sys/types.h>

// cpp libs
#include <iostream>

void get_login_info()
{
    // get login and uid, and print
    char *user = getlogin();
    uid_t uid = getuid();
    std::cout << "user: " << user << "\n" << "uid: " << uid << "\n";

    // c string for session
    char *session;
    // arr of c strings for sessions
    char **sessions;

    // pass arr of c strings by address = char***
    int amt = sd_uid_get_sessions(uid, false, &sessions);
    std::cout << amt << " sessions\n";
    for (int i = 0; i < amt; ++i)
        std::cout << "session " << (i + 1) << ": " << sessions[i] << "\n";
    free(sessions); // sessions from sd_uid_get_sessions needs to be free()
}

/// Client bus name: "user.agge.lockscreen".
const char* CLIENT_BUS_NAME = "user.agge.lockscreen";
const char* SERVER_INTERFACE_NAME = "org.freedesktop.login1.Session";
const char* METHOD_NAME = "Lock";

/**
 * D-Bus listener to listen for "Lock" signal.
 * @return TRUE if signal recieved, FALSE if not.
 */
bool is_locked()
{
    // setup needed var
    DBusError err;
    DBusConnection* conn;
    DBusMessage* msg;
    // print type of msg - want to use type_name<decltype(msg)>() for testing
    //std::cout << "decltype(msg): " << type_name<decltype(msg)>() << "\n";
    DBusMessageIter args;
    char* sigvalue;
    int ret;
    /// init dbus errors
    dbus_error_init(&err);

    /// Connect to the DBus message bus.
    // connect to DBUS_BUS_SYSTEM, NOT DBUS_BUS_SESSION for system bus
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        std::cerr << "Connection Error " << err.message << "\n";
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        // if there's no connection (NULL), exit(true)
        exit(1);
    }

    /// Request a name on the DBus message bus.
    bool flag = false;
    while (true && !flag) {
        ret = dbus_bus_request_name(conn, CLIENT_BUS_NAME,
            DBUS_NAME_FLAG_REPLACE_EXISTING, &err);

        // if name request success, exit loop
        if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
            flag = true;
        }

        // if name request in queue, print err and wait, reiter loop
        if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
            std::cerr << "Waiting for the bus...'\n'";
            sleep(1);
            continue;
        }

        // if dbus err, print name err and exit dbus
        if (dbus_error_is_set(&err)) {
            std::cerr << "Name error: " << err.message << "\n";
            dbus_error_free(&err);
            exit(true);
        }

        if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
            // if the request name is not correct, exit(true)
            exit(1);
        }
    }
    // print success
    std::cout << "Name request success: " << CLIENT_BUS_NAME << '\n';

    /// Recieve 'Lock' method from interface='org.freedesktop.login1.Session'.
    /// Add a rule to see correct messages.
    /// See signals from interface='org.freedesktop.login1.Session'.
    dbus_bus_add_match(conn,
            "type='signal',interface='org.freedesktop.login1.Session'", &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        // if err recieved, print stderr and exit(true)
        std::cerr << "Match Error " << err.message;
        exit(1);
    }

    /// while (true) loop listening for signals and messages being emmitted.
    while (true) {
        // non-blocking read of the next available message
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        // loop again if message hasn't been read
        if (NULL == msg) {
            /// Use of sleep() to simulate others things blocking on.
            sleep(1);
            continue;
        }

        /// Check if message is a signal from the correct interface and with
        /// the correct name: interface='org.freedesktop.login1.Session',
        /// name='Lock'.
       if (dbus_message_is_signal(msg, SERVER_INTERFACE_NAME, METHOD_NAME)) {
            // print success for debug
            std::cout << "'Lock' signal recieved!" << std::endl;

            // read the parameters
            if (!dbus_message_iter_init(msg, &args)) {
                std::cerr << "Message has no arguments!\n";
            } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
                std::cerr << "Argument is not a string!\n";
            } else {
                dbus_message_iter_get_basic(&args, &sigvalue);
                std::cerr << "Got Signal with value " << sigvalue << "\n";
            }
            // free the message
            dbus_message_unref(msg);
            /// Return 1 for true.
            return true;
        }
        /// Check if method call is from the correct interface and is the
        /// correct method: interface='org.freedesktop.login1.Session',
        /// method='Lock'.
        else if (dbus_message_is_method_call(
                    msg, SERVER_INTERFACE_NAME, METHOD_NAME)) {
            std::cout << "'Lock' method recieved!" << std::endl;
            // free the message
            dbus_message_unref(msg);
        }
    }

    /// Before application terminates, close DBus connection.
    dbus_connection_close(conn);
    return false;
}

int main()
{
    if (is_locked()) {
        sf::RenderWindow window0(sf::VideoMode(3840, 2160), "lockscreen0");
        window0.setFramerateLimit(1);
        sf::RenderWindow window1(sf::VideoMode(3840, 2160), "lockscreen1");
        window1.setFramerateLimit(1);
        while (true) {
            window0.clear();
            window1.clear();
            window0.display();
            sleep(2);
            window1.display();
        }
    }

    return 0;
}
