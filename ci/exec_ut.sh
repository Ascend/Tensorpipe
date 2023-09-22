#!/bin/bash

set -e

CUR_DIR=$(dirname $(readlink -f $0))

function main()
{
    # exec ut
    ./build/tensorpipe/test/tensorpipe_test

    exit 0
}

mian "$@"
