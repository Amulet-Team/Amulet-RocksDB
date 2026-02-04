cmake_minimum_required(VERSION 3.13)

project(amulet_rocksdb LANGUAGES CXX)

set(amulet_rocksdb_DIR ${CMAKE_CURRENT_LIST_DIR}/src/amulet/rocksdb CACHE PATH "")

# Set C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set platform variables
if (WIN32)
	# set windows 7 as the minimum version
	add_definitions(-D_WIN32_WINNT=0x0602)
elseif(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
elseif(UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
    message( FATAL_ERROR "Unsupported platform. Please submit a pull request to support this platform." )
endif()

if (MSVC)
    add_definitions("/MP")
endif()

# Find libraries
if (NOT TARGET pybind11::module)
    find_package(pybind11 CONFIG REQUIRED)
endif()
if (NOT TARGET amulet_pybind11_extensions)
    find_package(amulet_pybind11_extensions CONFIG REQUIRED)
endif()

# Add implementation
include(FetchContent)
set(WITH_CORE_TOOLS OFF CACHE BOOL "" FORCE)
set(WITH_TOOLS OFF CACHE BOOL "" FORCE)
set(WITH_TRACE_TOOLS OFF CACHE BOOL "" FORCE)
set(WITH_TESTS OFF CACHE BOOL "" FORCE)
set(WITH_ALL_TESTS OFF CACHE BOOL "" FORCE)
set(WITH_BENCHMARK_TOOLS OFF CACHE BOOL "" FORCE)
set(USE_RTTI OFF CACHE BOOL "" FORCE)
set(WITH_RUNTIME_DEBUG OFF CACHE BOOL "" FORCE)
set(WITH_WINDOWS_UTF8_FILENAMES OFF CACHE BOOL "" FORCE)
set(ROCKSDB_BUILD_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  rocksdb
  GIT_REPOSITORY https://github.com/facebook/rocksdb.git
  GIT_TAG        5fbc1cd5bcf63782675168b98e114151490de6d9
)
FetchContent_MakeAvailable(rocksdb)
#set_target_properties(libzstd_static PROPERTIES FOLDER "ThirdParty")
#set_target_properties(zlibstatic PROPERTIES FOLDER "ThirdParty")
set_target_properties(rocksdb PROPERTIES FOLDER "ThirdParty")

if(UNIX)
    FetchContent_Declare(
      gflags
      GIT_REPOSITORY https://github.com/gflags/gflags.git
      GIT_TAG        33bb3461e10864b5406353d3a1fb7ff34c2314b8
    )
    FetchContent_MakeAvailable(gflags)
    target_link_libraries(rocksdb PRIVATE gflags)
endif()

set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_PROGRAMS  OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://github.com/facebook/zstd
    GIT_TAG f8745da6ff1ad1e7bab384bd1f9d742439278e99
)
FetchContent_MakeAvailable(zstd)
target_link_libraries(rocksdb PRIVATE zstd)

# Find C++ files
file(REAL_PATH src SOURCE_PATH)
file(GLOB_RECURSE EXTENSION_SOURCES LIST_DIRECTORIES false ${SOURCE_PATH}/amulet/*.py.cpp)
file(GLOB_RECURSE EXTENSION_HEADERS LIST_DIRECTORIES false ${SOURCE_PATH}/amulet/*.py.hpp)
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ${SOURCE_PATH}/amulet/*.cpp)
file(GLOB_RECURSE HEADERS LIST_DIRECTORIES false ${SOURCE_PATH}/amulet/*.hpp)
list(REMOVE_ITEM SOURCES ${EXTENSION_SOURCES})
list(REMOVE_ITEM HEADERS ${EXTENSION_HEADERS})

# Add implementation
add_library(amulet_rocksdb STATIC)
set_target_properties(amulet_rocksdb PROPERTIES FOLDER "CPP")
target_compile_definitions(amulet_rocksdb PRIVATE AMULET_ROCKSDB_EXPORT= AMULET_ROCKSDB_EXPORT_EXCEPTION=)
target_link_libraries(amulet_rocksdb PRIVATE rocksdb)
target_include_directories(amulet_rocksdb PUBLIC ${SOURCE_PATH})
target_sources(amulet_rocksdb PRIVATE ${SOURCES} ${HEADERS})
foreach(FILE ${SOURCES} ${HEADERS})
    file(RELATIVE_PATH REL_PATH ${SOURCE_PATH} ${FILE})
    get_filename_component(GROUP ${REL_PATH} DIRECTORY)
    string(REPLACE "/" "\\" GROUP ${GROUP})
    source_group(${GROUP} FILES ${FILE})
endforeach()

# Add python extension
pybind11_add_module(_rocksdb)
set_target_properties(_rocksdb PROPERTIES FOLDER "Python")
target_link_libraries(_rocksdb PRIVATE amulet_pybind11_extensions)
target_link_libraries(_rocksdb PRIVATE amulet_rocksdb)
target_compile_definitions(_rocksdb PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
target_compile_definitions(_rocksdb PRIVATE PYBIND11_VERSION="${pybind11_VERSION}")
target_compile_definitions(_rocksdb PRIVATE COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")
target_compile_definitions(_rocksdb PRIVATE COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
target_include_directories(_rocksdb PRIVATE ${SOURCE_PATH})
target_sources(_rocksdb PRIVATE ${EXTENSION_SOURCES} ${EXTENSION_HEADERS})
foreach(FILE ${EXTENSION_SOURCES} ${EXTENSION_HEADERS})
    file(RELATIVE_PATH REL_PATH ${SOURCE_PATH} ${FILE})
    get_filename_component(GROUP ${REL_PATH} DIRECTORY)
    string(REPLACE "/" "\\" GROUP ${GROUP})
    source_group(${GROUP} FILES ${FILE})
endforeach()

if(NOT DEFINED AMULET_ROCKSDB_EXT_DIR)
    set(AMULET_ROCKSDB_EXT_DIR ${amulet_rocksdb_DIR})
endif()

# Install
install(TARGETS _rocksdb DESTINATION ${AMULET_ROCKSDB_EXT_DIR})
