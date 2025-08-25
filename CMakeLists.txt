cmake_minimum_required(VERSION 3.16)

## Default to release builds, must be set before project
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()


# Project Definition
set(VERIFYTAPN_NAME "verifytapn")
set(VERIFYTAPN_VERSION 1.3.2)
project(${VERIFYTAPN_NAME} VERSION ${VERIFYTAPN_VERSION} LANGUAGES CXX C)


# C++ Definition
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Default Values
## Build Flags
option(VERIFYTAPN_Static "Link libraries statically" ON)
option(VERIFYTAPN_GetDependencies "Fetch external dependencies from web." ON)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external CACHE PATH "Install location for external dependencies")
set(VERIFYTAPN_TARGETDIR "${CMAKE_BINARY_DIR}/${VERIFYTAPN_NAME}" CACHE PATH "Target directory for build files")
set(VERIFYTAPN_OSX_DEPLOYMENT_TARGET 10.8 CACHE STRING "Specify the minimum version of the target platform for MacOS on which the target binaries are to be deployed ")

# Configure Project

## Configure Static
if (VERIFYTAPN_Static)
    set(BUILD_SHARED_LIBS OFF)
    if (NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    else ()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif ()

## Configure Target Platform
if (UNIX AND NOT APPLE)
    set(ARCH_TYPE "linux64")
elseif (APPLE)
    set(ARCH_TYPE "osx64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET ${VERIFYTAPN_OSX_DEPLOYMENT_TARGET})
else ()
    set(ARCH_TYPE "win64")
endif ()

## Enabled INTERPROCEDURAL_OPTIMIZATION for non-macos
if (CMAKE_BUILD_TYPE MATCHES Release)
    if (NOT APPLE AND UNIX)
    	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif ()
endif ()

## Target File location
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VERIFYTAPN_TARGETDIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VERIFYTAPN_TARGETDIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VERIFYTAPN_TARGETDIR}/bin)

find_package(FLEX 2.6.0 REQUIRED)
find_package(BISON 3.5.0 REQUIRED)

if (VERIFYTAPN_GetDependencies)
    include(ExternalProject)
    # Install location should be ${EXTERNAL_INSTALL_LOCATION}

    if (UNIX AND NOT APPLE)
        set(BOOST_CONFIG_JAM "using gcc : : ${CMAKE_CXX_COMPILER}")
        set(BOOST_B2_OPTS "")
    elseif (APPLE)
        set(BOOST_CONFIG_JAM "using gcc : : ${CMAKE_CXX_COMPILER}")
        set(BOOST_B2_OPTS "")
    else ()
        set(BOOST_CONFIG_JAM "using gcc : : ${CMAKE_CXX_COMPILER}")
        set(BOOST_B2_OPTS target-os=windows binary-format=pe abi=ms threading=multi threadapi=win32)
    endif ()

    if (ARCH_TYPE MATCHES "win64")
        set(BOOST_CONFIG_JAM "using gcc : : ${CMAKE_CXX_COMPILER}")
        set(SPARSEHASH_HOST --host x86_64-w64-mingw32 )
    endif ()
    ExternalProject_Add(
        boost-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://archives.boost.io/release/1.78.0/source/boost_1_78_0.zip
        URL_HASH SHA256=f22143b5528e081123c3c5ed437e92f648fe69748e95fa6e2bd41484e2986cc3
        PATCH_COMMAND echo "${BOOST_CONFIG_JAM}" $<SEMICOLON>  > config.jam
        CONFIGURE_COMMAND ./bootstrap.sh --prefix=${CMAKE_BINARY_DIR}/external cxxflags="-arch x86_64" --without-icu
        BUILD_COMMAND ./b2 --user-config=config.jam --prefix=${CMAKE_BINARY_DIR}/external --with-program_options cxxstd=17 cflags="-fPIC" cxxflags="-fPIC" address-model=64 ${BOOST_B2_OPTS} variant=release link=static runtime-link=static install
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
    )

    ExternalProject_add(
        rapidxml-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://downloads.sourceforge.net/project/rapidxml/rapidxml/rapidxml%201.13/rapidxml-1.13.zip
        URL_HASH SHA512=6c10583e6631ccdb0217d0a5381172cb4c1046226de6ef1acf398d85e81d145228e14c3016aefcd7b70a1db8631505b048d8b4f5d4b0dbf1811d2482eefdd265
        BUILD_COMMAND ""
        CONFIGURE_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/external/include
        INSTALL_COMMAND cd ../rapidxml-ext && ${CMAKE_COMMAND} -E copy rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp ${EXTERNAL_INSTALL_LOCATION}/include
    )

    ExternalProject_add(
        unfoldtacpn-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        GIT_REPOSITORY https://github.com/TAPAAL/unfoldTACPN
        GIT_TAG 5948ffff5a084eccef7c2758b8df371b170fa68d
        CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}  -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DFLEX_EXECUTABLE=${FLEX_EXECUTABLE} -DBISON_EXECUTABLE=${BISON_EXECUTABLE}
    )

    ExternalProject_Add(
        xxhash-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/Cyan4973/xxHash/archive/refs/tags/v0.8.0.zip
        URL_HASH SHA256=064333c754f166837bbefefa497642a60b3f8035e54bae52eb304d3cb3ceb655
        SOURCE_SUBDIR cmake_unofficial
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
    )

    ExternalProject_Add(
        uutils-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/UPPAALModelChecker/UUtils/archive/refs/tags/v1.1.1.zip
        URL_HASH SHA256=5e0ccc6996f2eed47dc3ec7fb71cb6b79e0526db9b30622b552ba35a11f96424
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
    )

    #set(cflag-dbm "-DDISABLE_ASSERTX -DENABLE_64BIT -DCMAKE_TOOLCHAIN_FILE")
    ExternalProject_add(
        udbm-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/UPPAALModelChecker/UDBM/archive/refs/tags/v2.0.12.zip
        URL_HASH SHA512=a1217448fdbf0a358d2956b16b308de5baffff3bc2d988589758e52dc3bda81bd4eb50988a9ed19f1010be95b1ba2c6cfeccb535b7f7ed8aae022bfbeee868be
        CMAKE_ARGS(
            -DCMAKE_PREFIX_PATH=${EXTERNAL_INSTALL_LOCATION}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        )
        DEPENDS xxhash-ext uutils-ext
        INSTALL_COMMAND mkdir -p ${EXTERNAL_INSTALL_LOCATION}/include/dbm &&
        ${CMAKE_COMMAND} -E copy <BINARY_DIR>/include/dbm/config.h ${EXTERNAL_INSTALL_LOCATION}/include/dbm/config.h &&
        ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include ${EXTERNAL_INSTALL_LOCATION}/include &&
        mkdir -p ${EXTERNAL_INSTALL_LOCATION}/lib &&
        ${CMAKE_COMMAND} -E copy <BINARY_DIR>/src/libUDBM.a ${EXTERNAL_INSTALL_LOCATION}/lib/libUDBM.a
    )

    ExternalProject_Add(
        sparsehash-ext
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/sparsehash/sparsehash/archive/sparsehash-2.0.4.zip
        CONFIGURE_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ./configure --prefix=${CMAKE_BINARY_DIR}/external CXXFLAGS=-std=c++11 ${SPARSEHASH_HOST}
        BUILD_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} make
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 1
    )

    set(CMAKE_PREFIX_PATH ${EXTERNAL_INSTALL_LOCATION};${CMAKE_PREFIX_PATH})
    link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
    include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
endif (VERIFYTAPN_GetDependencies)

if (CMAKE_BUILD_TYPE MATCHES Prebuild)
    # dummy for pre-build
    add_custom_target( prebuild )
    add_dependencies( prebuild boost-ext rapidxml-ext unfoldtacpn-ext sparsehash-ext)
else ( )

    set(Boost_USE_RELEASE_LIBS ON)
    set(Boost_USE_MULTITHREADED OFF)
    if (NOT BUILD_TESTING)
        set(Boost_USE_STATIC_LIBS ON)
    else ()
        set(Boost_USE_STATIC_LIBS OFF)
    endif()

    if(NOT BUILD_TESTING)
        set(Boost_USE_STATIC_RUNTIME ON)
        set(Boost_USE_STATIC_LIBS ON)
        set(Boost_USE_RELEASE_LIBS ON)
        set(Boost_USE_MULTITHREADED OFF)
    else()
        set(Boost_USE_STATIC_LIBS OFF)
        set(Boost_USE_RELEASE_LIBS ON)
        set(Boost_USE_MULTITHREADED OFF)
    endif()

    find_package(Boost 1.66 REQUIRED program_options)
    include_directories(${Boost_INCLUDE_DIRS})

    # Set Macros
    add_compile_definitions(VERIFYTAPN_VERSION=\"${VERIFYTAPN_VERSION}\")
    add_compile_definitions(VERIFYTAPN_NAME=\"${VERIFYTAPN_NAME}-${ARCH_TYPE}\")

    # Source
    #include_directories(include)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/)
endif()

