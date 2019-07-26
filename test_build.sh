#!/bin/bash

DEPS_DIR="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps"

# BUILDSTATIC=1 
# LINKSTATIC=1
# debug LINK_STATIC=1 
#  BOOST_HOME=/tmp/boost_1_53_0/install
make OPENEXR_ROOT_DIR=/vfx ILMBASE_ROOT_DIR=/vfx PYTHON_VERSION="2.7" RED_ROOT="/mnt/Profiles/ahughes@netflix.local/_dev/oiio/deps/REDSDK"
