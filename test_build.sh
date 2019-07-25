#!/bin/bash

DEPS_DIR="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps"

make OPENEXR_ROOT_DIR=/vfx ILMBASE_ROOT_DIR=/vfx PYTHON_VERSION="2.7" REDSDK_INCLUDE_DIR="${DEPS_DIR}/Include" REDSDK_LIBRARIES="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps/REDSDK/Lib" USE_REDSDK=1
