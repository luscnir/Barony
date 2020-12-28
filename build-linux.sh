#!/bin/sh

steamworksDir="$(readlink -f "$STEAMWORKSROOT")"

rm -rf build/
mkdir -p build/
cd build/


STEAMWORKS_ENABLED=1 STEAMWORKSROOT="$steamworksDir" cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DFMOD_ENABLED=ON \
  ..

make
