#!/bin/bash

# This script, which assumes it is runnign on a Mac OSX with Homebrew
# installed, does a "brew install" in all packages reasonably needed by
# OIIO.

if [[ `uname` != "Darwin" ]] ; then
    echo "Don't run this script unless you are on Mac OSX"
    exit 1
fi

if [[ `which brew` == "" ]] ; then
    echo "You need to install Homebrew before running this script."
    echo "See http://brew.sh"
    exit 1
fi


brew update >/dev/null
echo ""
echo "Before my brew installs:"
brew list --versions

if [[ "$BUILDTARGET" == "clang-format" ]] ; then
    # If we are running for the sake of clang-format only, just install the
    # bare minimum packages and return.
    brew install --display-times ilmbase openexr llvm clang-format libtiff libpng boost ninja giflib
else
    # All cases except for clang-format target, we need the dependencies.
    brew install --display-times gcc ccache cmake ninja boost
    brew link --overwrite gcc
    brew install --display-times libtiff ilmbase openexr opencolorio
    brew install --display-times libpng giflib webp jpeg-turbo openjpeg
    brew install --display-times freetype libraw dcmtk pybind11 numpy
    brew install --display-times field3d ffmpeg libheif openvdb tbb
    # brew install --display-times opencv qt
fi

if [[ "$LINKSTATIC" == "1" ]] ; then
    brew install --display-times little-cms2 tinyxml szip
    brew install --display-times homebrew/dupes/bzip2
    brew install --display-times yaml-cpp --with-static-lib
fi
if [[ "$CLANG_TIDY" != "" ]] ; then
    # If we are running for the sake of clang-tidy only, we will need
    # a modern clang version not just the xcode one.
    brew install --display-times llvm
fi

echo ""
echo "After brew installs:"
brew list --versions

# Set up paths. These will only affect the caller if this script is
# run with 'source' rather than in a separate shell.
export PATH=/usr/local/opt/qt5/bin:$PATH ;
export PATH=/usr/local/opt/python/libexec/bin:$PATH ;
export PYTHONPATH=/usr/local/lib/python${PYTHON_VERSION}/site-packages:$PYTHONPATH ;
export PATH=/usr/local/Cellar/llvm/8.0.0*/bin:$PATH ;
