cmake_minimum_required (VERSION 2.8)
project (shine)

# Build mode
set(CMAKE_CXX_FLAGS_DEVELOPER "-O0 -Wall -Wextra -pedantic -g -Wno-unused-variable -Wno-unused-parameter" CACHE STRING
    "Flags used by the C++ compiler during maintainer builds."
    FORCE)

mark_as_advanced(CMAKE_CXX_FLAGS_DEVELOPER)

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Developer."
    FORCE)

option(BUILD_TESTING "Set to true to build the tests"
         "true")
############################################################################################
# Search for LLVM
############################################################################################

set(LLVM_ROOT "/usr/local" CACHE PATH "Root of LLVM install.")

if(NOT EXISTS ${LLVM_ROOT}/include/llvm)
    message(FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake")
include(LLVM)

include_directories(${LLVM_ROOT}/include)
include_directories(include)

link_directories(${LLVM_ROOT}/lib)

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES bitreader linker
                                                    instrumentation ipo
                                                    core jit native)

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
add_definitions(-DG_LOG_DOMAIN="Shine")

set(EXTRA_LIBRARIES pthread dl)

add_subdirectory(src)
add_subdirectory(include)


############################################################################################
# TESTING
############################################################################################
IF(BUILD_TESTING)
    message(STATUS "You have selected the building of the tests.")

    ############################################################################################
    # Search for Glib2
    ############################################################################################
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)

    include_directories(${GLIB2_INCLUDE_DIRS})
    link_directories(${GLIB2_LIBRARY_DIRS})
    add_definitions(${GLIB2_CFLAGS_OTHER})

    include(CTest)
    enable_testing()
    add_subdirectory(test)
ENDIF(BUILD_TESTING)


############################################################################################
# Search Doxygen
############################################################################################

find_package(Doxygen)

if(DOXYGEN_FOUND STREQUAL "NO")
    message(WARNING "Doxygen not found. Please get a copy http://www.doxygen.org")
endif(DOXYGEN_FOUND STREQUAL "NO")

if(DOXYGEN_FOUND STREQUAL "YES")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in
                           ${CMAKE_BINARY_DIR}/doxyfile)

    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxyfile
        COMMENT "Running Doxygen... (documentation generator)")
endif(DOXYGEN_FOUND STREQUAL "YES")

############################################################################################
# CPack
############################################################################################
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shine Library")
set(CPACK_PACKAGE_VENDOR "Christian S. Perone")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
include(CPack)



