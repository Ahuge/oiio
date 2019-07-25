# Require the RedSDK

if (NOT MSVC)
    set_source_files_properties (r3dinput.cpp
                             PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
endif()

if (USE_REDSDK AND REDSDK_FOUND)
    add_oiio_plugin (r3dinput.cpp
                     INCLUDE_DIRS ${REDSDK_INCLUDE_DIR}
                     LINK_LIBRARIES ${REDSDK_LIBRARIES}
                     DEFINITIONS "-DUSE_REDSDK")
else()
    message (STATUS "RedSDK not found: r3d plugin will not be built")
endif()
