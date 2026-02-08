#!/usr/bin/env bash
set -e
while [[ $# -gt 0 ]]; do 
    case $1 in
        -c | --clean)
           CLEAN=true
           echo "cleaning"
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
    echo
    echo "configuring with 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug'"
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug 
fi

echo
echo "building with 'cmake --build build'"
cmake --build build
