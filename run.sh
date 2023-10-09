#!/bin/bash

set -ue

cmake -B build
cmake --build build

time (./build/inOneWeekend.exe > img.ppm)
