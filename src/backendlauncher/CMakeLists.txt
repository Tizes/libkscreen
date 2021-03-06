include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set(backendlauncher_SRCS
    main.cpp
    backendloader.cpp
    backenddbuswrapper.cpp
)

ecm_qt_declare_logging_category(backendlauncher_SRCS
                                HEADER debug_p.h
                                IDENTIFIER KSCREEN_BACKEND_LAUNCHER
                                CATEGORY_NAME kscreen.backendLauncher
)

qt5_add_dbus_adaptor(backendlauncher_SRCS ${CMAKE_SOURCE_DIR}/interfaces/org.kde.KScreen.Backend.xml
                     backenddbuswrapper.h BackendDBusWrapper backendadaptor BackendAdaptor)
qt5_add_dbus_adaptor(backendlauncher_SRCS ${CMAKE_SOURCE_DIR}/interfaces/org.kde.KScreen.xml
                     backendloader.h BackendLoader backendloaderadaptor BackendLoaderAdaptor)

add_executable(kscreen_backend_launcher ${backendlauncher_SRCS})

target_link_libraries(kscreen_backend_launcher
    KF5Screen
    Qt5::Core
    Qt5::Gui
    Qt5::X11Extras
    Qt5::DBus
)

install(TARGETS kscreen_backend_launcher
        DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR_KF5}
)

configure_file(org.kde.kscreen.service.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kscreen.service @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kscreen.service
        DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
)
