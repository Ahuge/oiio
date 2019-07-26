# - Try to find RedSDK libraries
# Once done this will define
#
#  REDSDK_FOUND - system has RedSDK
#  REDSDK_INCLUDE_DIR - the RedSDK include directory
#  REDSDK_LIBRARIES - Link these to use RedSDK
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#  Modified for Hedgewars by Stepik777
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#

if (REDSDK_LIBRARIES AND REDSDK_INCLUDE_DIR)
  # in cache already
  set(REDSDK_FOUND TRUE)
else (REDSDK_LIBRARIES AND REDSDK_INCLUDE_DIR)
  if (REDSDK_ROOT)
    set(REDSDK_INCLUDE_DIR "${REDSDK_ROOT}/Include")
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
      set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/linux64/libR3DSDKPIC-cpp11.a" )
      # if( ${ARCHITECTURE} STREQUAL "64Bit" )
      #     set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/linux64/libR3DSDKPIC-cpp11.a" )
      # else()
      #     set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/linux32/libR3DSDKPIC-cpp11.a" )
      # endif()
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/win64/libR3DSDK.lib" )
      # if( ${ARCHITECTURE} STREQUAL "64Bit" )
      #     set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/win64/libR3DSDK.lib" )
      # else()
      #     set( REDSDK_LIBRARIES "${REDSDK_ROOT}/Lib/win32/libR3DSDK.lib" )
      # endif()
    endif()
    set(REDSDK_FOUND TRUE)
  endif (REDSDK_ROOT)

  if (REDSDK_FOUND)
    if (NOT RedSDK_FIND_QUIETLY)
      message(STATUS "Found REDSDK: ${REDSDK_LIBRARIES}, ${REDSDK_INCLUDE_DIR}")
    endif (NOT RedSDK_FIND_QUIETLY)
  else (REDSDK_FOUND)
    if (REDSDK_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find RedSDK. Set REDSDK_INCLUDE_DIR and REDSDK_LIBRARIES")
    endif (REDSDK_FIND_REQUIRED)
  endif (REDSDK_FOUND)

endif (REDSDK_LIBRARIES AND REDSDK_INCLUDE_DIR)
