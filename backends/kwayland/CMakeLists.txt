
set(wayland_SRCS
    waylandbackend.cpp
    waylandconfig.cpp
    waylandoutput.cpp
    waylandscreen.cpp
    ../utils.cpp
)

add_library(KSC_KWayland MODULE ${wayland_SRCS})

set_target_properties(KSC_KWayland PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/kf5/kscreen")
set_target_properties(KSC_KWayland PROPERTIES PREFIX "")
target_link_libraries(KSC_KWayland Qt5::Core
                                  Qt5::Gui
                                  KF5::Screen
                                  KF5::WaylandClient
)

install(TARGETS KSC_KWayland DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kscreen/)
