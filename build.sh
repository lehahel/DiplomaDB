#!/bin/bash
rm -rf build
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..
cd ..
rm .vscode/compile_commands.json
rm .vscode/compile_commands.json
cp -r build/compile_commands.json .vscode/
cd build
make
cd ..