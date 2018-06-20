include_directories("../include")
include_directories("../include/ql")
include_directories("../src")
include_directories("../build/generated")
include_directories("../contrib/protobuf/src")


# All the source files for the sc2 api
file(GLOB sources_qllib "qllib/*.cpp" "../include/ql/qllib/*.h")
file(GLOB sources_qlbot "qlbot/*.cpp" "../include/ql/qlbot/*.h")



add_library(qllib ${sources_qllib})
add_library(qlbot ${sources_qlbot})
set_target_properties(qllib PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(qlbot PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(qlbot qllib)
#set_target_properties(qllib PROPERTIES COMPILE_FLAGS "/W4 /WX")
#set_target_properties(qlbot PROPERTIES COMPILE_FLAGS "/W4 /WX") 
