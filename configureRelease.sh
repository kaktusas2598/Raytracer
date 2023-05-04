#!/bin/bash
rm -rf out/*
cmake -S . -B out/ -G"Unix Makefiles" -D CMAKE_BUILD_TYPE=Release
