mkdir build_vs2019
pushd build_vs2019
cmake --debug-trycompile -G "Visual Studio 16 2019" -A x64 ..
popd
