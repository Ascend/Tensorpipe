#!/bin/bash

set -e

CUR_DIR=$(dirname $(readlink -f $0))

function main()
{
    cd ${CUR_DIR}/..

    if [ -d "build" ]; then
        rm -r ./build
    fi
    mkdir ./build

    bash ./third_party/acl/libs/build_stub.sh

    cd ./build
    cmake ../ -DTP_ENABLE_SHM=ON -DTP_ENABLE_CMA=ON -DTP_ENABLE_IBV=OFF -DTP_TEST_NPU=OFF
    make

    exit 0
}

main "$@"
