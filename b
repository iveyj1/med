#!/usr/bin/env bash
set -e
BUILD_FLAGS=""

echo "pwd: $(pwd)"

while [[ $# -gt 0 ]]; do 
    case $1 in
        -c | --clean)
           CLEAN=true
           echo
           echo "Cleaning build dirs"
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
        -f | --format)
            FORMAT_CODE=true
            shift
            ;;
        --strip)
            STRIP=true
            shift
            ;;
        *)
            echo "Invalid option $1"
            exit 1
            ;;
    esac
done

if [[ $FORMAT_CODE ]]; then 
    echo
    echo "Formatting all .c and .h files in $(pwd)"
    ./formatc --modify
fi

if [[ $CLEAN ]]; then
    LAST=`pwd` && cd build && rm -rf *; cd $LAST
    echo
    echo "Configuring with 'cmake -S . -B build $BUILD_FLAGS'"
    cmake -S . -B build $BUILD_FLAGS
fi

echo
echo "Building with 'cmake --build build'"
cmake --build build
if [[ $STRIP ]]; then
    echo
    echo "Stripping executable"
    cd build && strip med
fi

