MESSAGE(STATUS "CMake game_server")


CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR} PRIVATE_SOURCES)

list(APPEND PRIVATE_SOURCES)
GroupSources(${CMAKE_CURRENT_SOURCE_DIR})


include_directories(${SolutionDir}/src/dependencies/g3dlib/include)
include_directories(${SolutionDir}/src/dependencies/recastnavigation/Detour/Include)
include_directories(${SolutionDir}/src/dependencies/recastnavigation/Recast/Include)
if (WIN32)
	include_directories(${SolutionDir}/src/dependencies/libgo)
else (UNIX)
	include_directories(${SolutionDir}/src/dependencies/libgo)
endif()

add_executable(game_server ${PRIVATE_SOURCES})


CollectIncludeDirectories(${CMAKE_CURRENT_SOURCE_DIR} PUBLIC_INCLUDES)

target_include_directories(game_server
  PUBLIC
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})


# “¿¿µœÓ
add_dependencies(game_server game_client_server db_redis cryptography data_agent ${COMM_DEP_LIB} g3dlib)

# ¡¥Ω”
target_link_libraries(game_server game_client_server db_redis cryptography data_agent ${COMM_LINK_LIB} g3dlib)	


set(EXECUTABLE_OUTPUT_PATH ${OutPutDir})
set_target_properties(game_server PROPERTIES FOLDER "server/game")