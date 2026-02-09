#!/usr/bin/env bash
set -e
BUILD_FLAGS=""
while [[ $# -gt 0 ]]; do 
    case $1 in
        -c | --clean)
           CLEAN=true
           echo "cleaning"
           shift
           ;;
        -d | --debug)
            BUILD_FLAGS+=" -DCMAKE_BUILD_TYPE=Debug"
            shift
            ;;
        -s | --static)
            BUILD_FLAGS+=" -DCMAKE_EXE_LINKER_FLAGS=\"-static\""
            shift
            ;;
        --strip)
            STRIP=true
            shift
            ;;
        *)
            echo "invalid option $1"
            exit 1
            ;;
    esac
done
if [[ $CLEAN ]]; then
    LAST=`pwd` && cd build && rm -rf *; cd $LAST
    echo "pwd: $(pwd)"
    echo "configuring with 'cmake -S . -B build $BUILD_FLAGS'"
    cmake -S . -B build $BUILD_FLAGS
fi
echo
echo "building with 'cmake --build build'"
cmake --build build
if [[ $STRIP ]]; then
    echo "stripping executable"
    cd build && strip med
fi

