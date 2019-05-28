MESSAGE(STATUS "CMake login_server")


CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR} PRIVATE_SOURCES)

list(APPEND PRIVATE_SOURCES)
GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(login_server ${PRIVATE_SOURCES})


CollectIncludeDirectories(${CMAKE_CURRENT_SOURCE_DIR} PUBLIC_INCLUDES)

target_include_directories(login_server
  PUBLIC
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})


# “¿¿µœÓ
add_dependencies(login_server cryptography data_agent db_redis ${COMM_DEP_LIB})

# ¡¥Ω”
target_link_libraries(login_server login_client_server cryptography data_agent db_redis ${COMM_LINK_LIB})	

set(EXECUTABLE_OUTPUT_PATH ${OutPutDir})
set(EXECUTABLE_OUTPUT_PATH ${OutPutDir})
set_target_properties(login_server PROPERTIES FOLDER "server/login")