mkdir build_vs2015
pushd build_vs2015
cmake --debug-trycompile -G "Visual Studio 14 2015 Win64" ..
popd
