include_directories("../include")
include_directories("common")

file(GLOB common_sources "common/*.h" "common/*.cc")

include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/contrib/SDL-mirror/include")

#bot_simple
add_executable(bot_simple "bot_simple.cc")
set_target_properties(bot_simple PROPERTIES FOLDER examples)
target_link_libraries(bot_simple sc2api sc2lib sc2utils)

#echo_actions
add_executable(echo_actions "echo_actions.cc")
set_target_properties(echo_actions PROPERTIES FOLDER examples)
target_link_libraries(echo_actions sc2api sc2utils)

#bot_micro
add_executable(bot_micro "bot_micro.cc" ${common_sources})
set_target_properties(bot_micro PROPERTIES FOLDER examples)
target_link_libraries(bot_micro sc2api sc2lib sc2utils)

#replay
add_executable(replay "replay.cc")
set_target_properties(replay PROPERTIES FOLDER examples)
target_link_libraries(replay sc2api sc2utils)

#Tutorial
add_executable(tutorial "tutorial.cc")
set_target_properties(tutorial PROPERTIES FOLDER examples)
target_link_libraries(tutorial sc2api sc2utils)

#bot_mp
add_executable(bot_mp "bot_mp.cc" ${common_sources})
set_target_properties(bot_mp PROPERTIES FOLDER examples)
target_link_libraries(bot_mp sc2api sc2lib sc2utils)


# Exclude SDL and related projects to work around linker issue.
if (NOT APPLE)
	#feature layers
	add_executable(feature_layers "feature_layers.cc")
	set_target_properties(feature_layers PROPERTIES FOLDER examples)
	target_link_libraries(feature_layers sc2api sc2utils sc2renderer)
endif ()

#Intermediate bot
add_executable(intermediate_bot "intermediate_bot.cc" ${common_sources})
set_target_properties(intermediate_bot PROPERTIES FOLDER examples)
target_link_libraries(intermediate_bot sc2api sc2lib sc2utils)

#annoying_helper
add_executable(annoying_helper "annoying_helper.cc")
set_target_properties(annoying_helper PROPERTIES FOLDER examples)
target_link_libraries(annoying_helper sc2api sc2lib sc2utils)
