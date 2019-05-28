cmake_minimum_required(VERSION 3.1)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(wow)

message(STATUS "project source dir = ${PROJECT_SOURCE_DIR}")
message(STATUS "project bin dir = ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

#工程路径
set(SolutionDir ${PROJECT_SOURCE_DIR})

# vs的工程筛选，可以将多个工程筛选到一个属性内
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# 一些设置
# set(CMAKE_SKIP_BUILD_RPATH 0)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

#debug模式
option(DEBUG "ON for Debug or OFF for release" ON)
set(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
message(STATUS "debug or release:" ${CMAKE_BUILD_TYPE})


# 额外的cmake规则
list(APPEND CMAKE_MODULE_PATH 
	"${CMAKE_SOURCE_DIR}/cmake"
	"${CMAKE_SOURCE_DIR}/cmake/macros")
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(ConfigureScripts)
include(cmake/options.cmake) # showoptions.cmake影响vs的筛选器
include(ConfigureBaseTargets)
include(CheckPlatform)
include(GroupSources)
include(AutoCollect)
include(cmake/showoptions.cmake) 


# 生成静态库还是动态库
set(LIB_TYPE STATIC)
set(USE_STATIC 1)
if (USE_STATIC)
	message(STATUS "build static lib")
	set(LIB_TYPE STATIC) 
else()
	message(STATUS "build dynamic lib")
	set(LIB_TYPE SHARED) 
endif()
	
	
find_package(MySQL)
find_package(OpenSSL REQUIRED)

if (UNIX)
find_package (libmongoc-static-1.0 1.7 REQUIRED)
endif()

message("-- mongoc found version \"${MONGOC_STATIC_VERSION}\"")
message("-- mongoc include path \"${MONGOC_STATIC_INCLUDE_DIRS}\"")
message("-- mongoc libraries \"${MONGOC_STATIC_LIBRARIES}\"")
message("-- MySQL Include Dir \"${MYSQL_INCLUDE_DIR}\"")
message("-- OpenSSL Include Dir \"${OPENSSL_INCLUDE_DIR}\"")
message("-- OpenSSL Lib Dir \"${OPENSSL_LIBRARIES}\"")
# 设置输出路径
if (WIN32)
	
	include_directories(${SolutionDir}/src/dep/zq_engine/zq_engine/dependencies/Python-3.7.1/PC)
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	
		# lib输出目录
		set(LibOutPutDir ${SolutionDir}/_libs)		
		# bin输出目录
		set(BinOutPutDir ${SolutionDir}/bin)
		
	else()
	
		# lib输出目录
		set(LibOutPutDir ${SolutionDir}/_libs)	
		# bin输出目录
		set(BinOutPutDir ${SolutionDir}/bin)
	
	endif()
else(UNIX)
	
	include_directories(${SolutionDir}/src/dep/zq_engine/zq_engine/dependencies/Python-3.7.1)	
	
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	
		# lib输出目录
		set(LibOutPutDir ${SolutionDir}/_libs/Debug)		
		# bin输出目录
		set(BinOutPutDir ${SolutionDir}/bin/Debug)
		
	else()
	
		# lib输出目录
		set(LibOutPutDir ${SolutionDir}/_libs/Release)	
		# bin输出目录
		set(BinOutPutDir ${SolutionDir}/bin/Release)
	
	endif()
endif()


# 头文件包含路径
include_directories(
	${SolutionDir}/src
	${SolutionDir}/src/dep/zq_engine
	${SolutionDir}/src/dep/zq_engine/zq_engine/include
	${SolutionDir}/src/dep/zq_engine/zq_engine/dependencies/protobuf-3.6.1/src
	${SolutionDir}/src/dep/zq_engine/zq_engine/dependencies/Python-3.7.1/Include
	${SolutionDir}/src/dep/zq_engine/zq_engine/dependencies/lua-5.3.5/src
	
	# 第三方库
	${OPENSSL_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
	${MONGOC_STATIC_INCLUDE_DIRS}
	)


# 需要链接的库的路径
link_directories(
	${SolutionDir}/src/dep/zq_engine/_libs/${CMAKE_BUILD_TYPE}
	${LibOutPutDir}
	${OPENSSL_LIBRARIES}
	${MYSQL_LIBRARY}
	${MONGOC_STATIC_LIBRARIES}
	)

	
	
# 需要连接的库的名字
list(APPEND COMM_LINK_LIB
	message
	zq_engine
	${OPENSSL_LIBRARIES}
	${MYSQL_LIBRARY}
	${MONGOC_STATIC_LIBRARIES}
)
if (WIN32)
	list(APPEND COMM_LINK_LIB hiredis.lib protobuf.lib python37_d lua-5.3.5.lib bson-1.0.lib mongoc-1.0.lib dbghelp libgo.lib)
else (UNIX)
	# python err undefined reference to `openpty' -lutil 
	list(APPEND COMM_LINK_LIB hiredis.a protobuf.a python3.7.a lua.a dl m pthread stdc++fs util liblibgo.a)
endif()

# 一些公共的依赖库	
set(COMM_DEP_LIB kernel network base_code message http_lib libloader)

add_subdirectory(src)


set(CMAKE_INSTALL_PREFIX ${SolutionDir}/install)
INSTALL(DIRECTORY bin/
	DESTINATION bin/)




