#---------------------------------------------------
# OpenMM Metal Platform
#
# Creates OpenMMMetal library.
#
# Windows:
#   OpenMMMetal.dll
#   OpenMMMetal.lib
#   OpenMMMetal_static.lib
# Unix:
#   libOpenMMMetal.so
#   libOpenMMMetal_static.a
#----------------------------------------------------

SET(OPENMM_BUILD_METAL_TESTS TRUE CACHE BOOL "Whether to build Metal platform test cases")
MARK_AS_ADVANCED(OPENMM_BUILD_METAL_TESTS)
IF(BUILD_TESTING AND OPENMM_BUILD_METAL_TESTS)
    SUBDIRS(tests)
ENDIF(BUILD_TESTING AND OPENMM_BUILD_METAL_TESTS)

# The source is organized into subdirectories, but we handle them all from
# this CMakeLists file rather than letting CMake visit them as SUBDIRS.
SET(OPENMM_SOURCE_SUBDIRS .)

# These are all the places to search for header files which are
# to be part of the API.
SET(API_INCLUDE_DIRS) # start empty
FOREACH(subdir ${OPENMM_SOURCE_SUBDIRS})
    # append
    SET(API_INCLUDE_DIRS ${API_INCLUDE_DIRS}
                         ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/include
                         ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/include/internal)
ENDFOREACH(subdir)

# Install headers

FILE(GLOB CORE_HEADERS include/*.h)
INSTALL_FILES(/include/openmm/metal FILES ${CORE_HEADERS})

IF(OPENMM_BUILD_SHARED_LIB)
    SUBDIRS (sharedTarget)
ENDIF(OPENMM_BUILD_SHARED_LIB)
IF(OPENMM_BUILD_STATIC_LIB)
    SUBDIRS (staticTarget)
ENDIF(OPENMM_BUILD_STATIC_LIB)
