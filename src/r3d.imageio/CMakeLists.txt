# Require the RedSDK

if (NOT MSVC)
    set_source_files_properties (r3dinput.cpp
                             PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations" 
                                        COMPILE_DEFINITIONS "-DREDRUNTIME=${RED_RUNTIME_LIBRARIES}"
                             )
endif()

if (USE_REDSDK AND RED_FOUND)
    add_oiio_plugin (r3dinput.cpp
                     INCLUDE_DIRS ${RED_INCLUDE_DIR}
                     LINK_LIBRARIES ${RED_LIBRARIES}
                     DEFINITIONS "-DUSE_REDSDK=1")
    # target_compile_definitions(r3dinput PUBLIC "-DUSE_RED=1" "-DREDRUNTIME=\"${RED_RUNTIME_LIBRARIES}\"")
else()
    message (STATUS "RedSDK not found: r3d plugin will not be built")
endif()
