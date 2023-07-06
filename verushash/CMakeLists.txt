cmake_minimum_required(VERSION 3.10)
project(verushash)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") # -Wall
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(verushash STATIC
        bcd.c
        blake.c
        blake256.c
        bmw.c
        aes_helper.c
        cryptonight.c
        cubehash.c
        echo.c
        fugue.c
        gost.c
        groestl.c
        hamsi_helper.c
        hamsi.c
        haraka.c
        haraka_portable.c
        hsr.c
        jh.c
        Keccak-compact64.c
        keccak.c
        KeccakDuplex.c
        KeccakHash.c
        KeccakSponge.c
        lbry.c
        luffa.c
        lyra2.c
        lyra2rev2.c
        lyra2Z.c
        maxcoin.c
        md_helper.c
        phi.c
        ripemd.c
        sha2.c
        sha2big.c
        shavite.c
        simd.c
        skein_block.c
        skein.c
        skein2.c
        skunk.c
        sm3.c
        sponge.c
        tribus.c
        verus_clhash_portable.cpp
        )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64")

    # optimizations
    add_definitions(-O2)


# MACOS
if(APPLE)
   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compat)
endif(APPLE)

#include "lbry.h"
#include "lyra2rev2.h"
#include "lyra2Z.h"
#include "phi.h"
#include "cryptonight.h"
#include "maxcoin.h"
#include "tribus.h"
#include "sph_sha2.h"
#include "skein.h"
#include "KeccakHash.h"
#include "haraka_portable.h"

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/lbry.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/lyra2rev2.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/lyra2Z.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/phi.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/cryptonight.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/maxcoin.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/tribus.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/sph_sha2.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/skein.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/KeccakHash.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/haraka.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/haraka_portable.c PROPERTIES COMPILE_FLAGS "-m64 -mpclmul -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -maes -g -fomit-frame-pointer")


# Common
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# BOOST
#find_package(Threads REQUIRED COMPONENTS)
# compile boost statically
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#set(BUILD_SHARED_LIBRARIES OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
find_package(Boost REQUIRED COMPONENTS system)

if (Boost_FOUND)
    # From the official documentation:
    # Add include directories to the build. [...] If the SYSTEM option is given,
    # the compiler will be told the directories are meant as system include
    # directories on some platforms (signalling this setting might achieve effects
    # such as the compiler skipping warnings [...])."
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})

    # From the official documentation:
    # "Specify directories in which the linker will look for libraries. [...] Note
    # that this command is rarely necessary. Library locations returned by
    # find_package() and find_library() are absolute paths. Pass these absolute
    # library file paths directly to the target_link_libraries() command. CMake
    # will ensure the linker finds them."
    link_directories (${Boost_LIBRARY_DIRS})
else()
    message("Boost_FOUND NOT FOUND")
endif ()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../)
find_package(PkgConfig REQUIRED)


set(LIBS ${LIBS} ${Boost_LIBRARIES})

message("-- CXXFLAGS: ${CMAKE_CXX_FLAGS}")
message("-- LIBS: ${LIBS}")

target_link_libraries (verushash ${LIBS})