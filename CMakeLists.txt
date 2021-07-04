option(CCAP_NET_CLONE_JCLIB_FROM_GITHUB "Enable this to auto-clone jclib from github" OFF)

cmake_minimum_required(VERSION 3.8)

project(CCapNet)

include("CMakeUtils.cmake")


set(lib_dir "${CMAKE_CURRENT_SOURCE_DIR}/local")
set(jclib_url "https://github.com/JonathanCline/JCLib")
set(jclib_branch "main")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ".")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)



if(CCAP_NET_CLONE_JCLIB_FROM_GITHUB)
	ADD_GIT_DEPENDENCY(${lib_dir} JCLib JCLib ${jclib_url} ${jclib_branch})
endif()

if(NOT TARGET JCLib)
	message(FATAL_ERROR "JCLib is not defined, enable CCAP_NET_CLONE_JCLIB_FROM_GITHUB to automatically clone it from github")
endif()
target_link_libraries(${PROJECT_NAME} INTERFACE JCLib)
