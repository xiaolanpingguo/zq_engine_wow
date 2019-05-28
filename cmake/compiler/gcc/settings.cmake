# Set build-directive (used in core to tell which buildtype we used)
target_compile_definitions(trinity-compile-option-interface
  INTERFACE
    -D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

set(GCC_EXPECTED_VERSION 7.2.0)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_EXPECTED_VERSION)
  message(FATAL_ERROR "GCC: requires version ${GCC_EXPECTED_VERSION} to build but found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O0 -m64 -Wall -Wno-reorder -ldl -fPIC -std=c++17")
set(CMAKE_C_FLAGS " -ggdb3 -O0 -m64 -Wall -ldl -fPIC")
set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")

add_definitions(-D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")	




