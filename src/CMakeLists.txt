find_package(lua REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
   
include_directories(${LUA_INCLUDE_DIR})
aux_source_directory(. DIR_EA_ENGINE_SRCS)
add_executable(eaEngine ${DIR_EA_ENGINE_SRCS})

target_link_libraries(eaEngine PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_ttf)
target_link_libraries(eaEngine PRIVATE ${LUA_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../bin)
set_target_properties(eaEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

add_custom_command(TARGET eaEngine POST_BUILD
	COMMAND echo Copy lua file from ${PROJECT_SOURCE_DIR}/lua to ${EXECUTABLE_OUTPUT_PATH}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/lua ${EXECUTABLE_OUTPUT_PATH}
)

add_custom_command(TARGET eaEngine POST_BUILD
	COMMAND echo Copy scene file from ${PROJECT_SOURCE_DIR}/scene to ${EXECUTABLE_OUTPUT_PATH}/scene
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/scene ${EXECUTABLE_OUTPUT_PATH}/scene
)

add_custom_command(TARGET eaEngine POST_BUILD
	COMMAND echo Copy image file from ${PROJECT_SOURCE_DIR}/image to ${EXECUTABLE_OUTPUT_PATH}/image
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/image ${EXECUTABLE_OUTPUT_PATH}/image
)

add_custom_command(TARGET eaEngine POST_BUILD
	COMMAND echo Copy font file from ${PROJECT_SOURCE_DIR}/font to ${EXECUTABLE_OUTPUT_PATH}/font
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/font ${EXECUTABLE_OUTPUT_PATH}/font
)