#!/bin/bash

DEPS_DIR="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps"

make OPENEXR_ROOT_DIR=/vfx ILMBASE_ROOT_DIR=/vfx PYTHON_VERSION="2.7" REDSDK_INCLUDE_DIR="${DEPS_DIR}/Include" REDSDK_LIBRARIES="${DEPS_DIR}/Lib" REDSDK_ROOT="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps"
