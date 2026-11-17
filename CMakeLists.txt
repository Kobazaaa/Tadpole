cmake_minimum_required(VERSION 3.27)
project(Tadpole LANGUAGES CXX)

option(TADPOLE_WARNINGS_AS_ERRORS "Treat warnings as errors in Tadpole" ON)

# Dependencies
if(NOT TARGET kobengine::kobengine)
	add_subdirectory(extern/kobengine)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchAndIncludeLibraries.cmake")

# Exe
file(GLOB_RECURSE TADPOLE_SOURCES CONFIGURE_DEPENDS	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${TADPOLE_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${PROJECT_NAME} PRIVATE
	kobengine::kobengine
)

if(MSVC)
	set(TADPOLE_WARNINGS /W4)
	if(TADPOLE_WARNINGS_AS_ERRORS)
		list(APPEND TADPOLE_WARNINGS /WX)
	endif()
else()
	set(TADPOLE_WARNINGS -Wall -Wextra -Wpedantic)
	if(TADPOLE_WARNINGS_AS_ERRORS)
		list(APPEND TADPOLE_WARNINGS -Werror)
	endif()
endif()
target_compile_options(${PROJECT_NAME} PRIVATE ${TADPOLE_WARNINGS})