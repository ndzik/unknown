#!/bin/bash

function is_installed() {
    if ! command -v $1 &> /dev/null
    then
        echo "$1 could not be found"
        exit
    fi
}

is_installed bear

# Generate compile_commands.json
make clean
bear -- make build
