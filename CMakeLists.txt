# cmake tested versions
cmake_minimum_required(VERSION 3.22...3.27)

# project name, description, and lang
project(lock-screen
    DESCRIPTION "A simple lock screen"
    LANGUAGES CXX)

project(dbus-example
    DESCRIPTION "Example provided by www.matthew.ath.cx/misc/dbus"
    LANGUAGES C)

project(registry
    DESCRIPTION "Followed example from
    jan.newmarch.name/Wayland/ProgrammingClient/"
    LANGUAGES CXX)

project(compositor
    DESCRIPTION "Followed example from
    drewdevault.com/2018/02/17/Writing-a-Wayland-compositor-1.html"
    LANGUAGES C)

# set version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# declare exe
add_executable(lock-screen)
add_executable(dbus-example)
add_executable(registry)
add_executable(compositor)

# add src
target_sources(lock-screen PRIVATE src/main.cpp)
target_sources(dbus-example PRIVATE src/dbus-example.c)
target_sources(registry PRIVATE src/registry.cpp
    src/session-lock.c)
target_sources(compositor PRIVATE src/compositor.c)

# find necc pkgs
find_package(SFML COMPONENTS graphics window system REQUIRED)

# include dir
target_include_directories(lock-screen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    /usr/include/dbus-1.0
    /usr/lib64/dbus-1.0/include
    )
target_include_directories(dbus-example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    /usr/include/dbus-1.0
    /usr/lib64/dbus-1.0/include
    )
target_include_directories(registry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    /usr/include/
    )
target_include_directories(compositor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    /usr/include/
    /usr/include/wlr/
    /usr/include/pixman-1/
    )

# link lib
target_link_libraries(lock-screen PRIVATE
    elogind
    dbus-1
    sfml-graphics
    sfml-window
    sfml-system
    )
target_link_libraries(dbus-example PRIVATE
    elogind
    dbus-1
    )
target_link_libraries(registry PRIVATE
    wayland-client
    )
target_link_libraries(compositor PRIVATE
    wayland-client
    wayland-server
    wlroots
    )
