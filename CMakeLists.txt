cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(k40gen CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake) # for find_package

option(ENABLE_PYTHON "Enable python bindings" TRUE)
option(ENABLE_VC "Enable usage of VC library" TRUE)
option(ENABLE_TESTS "Enable tests" TRUE)

include(ExternalProject)
include(FindPackageHandleStandardArgs)

if (ENABLE_VC)
find_package(Vc)

if (NOT Vc_FOUND OR "${Vc_VERSION}" VERSION_LESS "1.3.2")

message(STATUS "Using builtin Vc")

set(EXTERNAL_DIR "external")
set(Vc_VERSION "1.4.1")
set(Vc_PROJECT "Vc-${Vc_VERSION}")
set(Vc_SRC_URI "https://github.com/VcDevel/Vc/archive/${Vc_VERSION}.tar.gz")
set(Vc_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")
set(Vc_LIBNAME "${CMAKE_STATIC_LIBRARY_PREFIX}Vc${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(Vc_LIBRARY "${Vc_DESTDIR}/lib/${Vc_LIBNAME}")


ExternalProject_Add(VC
  URL ${Vc_SRC_URI}
  URL_HASH SHA256=7e8b57ed5ff9eb0835636203898c21302733973ff8eaede5134dd7cb87f915f6
  INSTALL_DIR ${Vc_DESTDIR}
  BUILD_IN_SOURCE 0
  BUILD_BYPRODUCTS ${Vc_LIBRARY}
  LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
  CMAKE_ARGS -G ${CMAKE_GENERATOR}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
             -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
             -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

set(VC_TARGET Vc)
set(Vc_LIBRARIES Vc)
set(Vc_INCLUDE_DIR "${Vc_DESTDIR}/include")
set(Vc_CMAKE_MODULES_DIR "${Vc_DESTDIR}/lib/cmake/Vc")

# add_library(VcExt STATIC IMPORTED)
# set_property(TARGET VcExt PROPERTY IMPORTED_LOCATION ${Vc_LIBRARY})
# add_dependencies(VcExt VC)

# add_library(Vc INTERFACE)
# target_include_directories(Vc SYSTEM BEFORE INTERFACE $<BUILD_INTERFACE:${Vc_INCLUDE_DIR}>)
# target_link_libraries(Vc INTERFACE VcExt)

find_package_handle_standard_args(Vc
  FOUND_VAR Vc_FOUND
  REQUIRED_VARS Vc_INCLUDE_DIR Vc_LIBRARIES Vc_CMAKE_MODULES_DIR
  VERSION_VAR Vc_VERSION)

endif()
else()
  set(USE_AVX2 FALSE)
endif()

# vectorclass
set(vectorclass_VERSION "1.30")
set(vectorclass_SRC_URI "https://www.agner.org/optimize/vectorclass.zip")
set(vectorclass_DESTDIR "${CMAKE_BINARY_DIR}/include")
set(vectorclass_ROOTDIR "${vectorclass_DESTDIR}/vectorclass")

ExternalProject_Add(vectorclass
  URL ${vectorclass_SRC_URI}
  URL_HASH SHA256=f9cb70a3e865dd019b58f449d11f90147ce8ba5f2c60410389ec0ead92944b97
  SOURCE_DIR vectorclass
  INSTALL_DIR ${vectorclass_ROOTDIR}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND unzip -d <SOURCE_DIR> -o <SOURCE_DIR>/special.zip
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR> <INSTALL_DIR>
  STEP_TARGETS install
)

# FIXME: This is a workaround to let ROOT find the headers at runtime if
# they are in the build directory. This is necessary until we decide how to
# treat externals with headers used by ROOT
if(NOT EXISTS ${CMAKE_BINARY_DIR}/include/Vc)
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/include)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include)
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${Vc_INCLUDE_DIR}/Vc ${CMAKE_BINARY_DIR}/include/Vc)
endif()
# end of workaround

# install(DIRECTORY ${Vc_DESTDIR}/ DESTINATION ".")

if(ENABLE_PYTHON)
# pybind11
find_package(pybind11)

if (NOT pybind11_FOUND OR pybind11_VERSION VERSION_LESS "2.1.0")
message(STATUS "Using builtin pybind11")
set(BUILTIN_PYBIND TRUE)

set(PYBIND11_VERSION "2.2.4")
set(PYBIND11_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")

ExternalProject_Add(pybind11
  URL "https://github.com/pybind/pybind11/archive/v${PYBIND11_VERSION}.tar.gz"
  URL_HASH SHA256=b69e83658513215b8d1443544d0549b7d231b9f201f6fc787a2b2218b408181e
  BUILD_IN_SOURCE 0
  INSTALL_DIR ${PYBIND11_DESTDIR}
  LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
  CMAKE_ARGS -G ${CMAKE_GENERATOR}
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
             -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
             -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
             -DPYBIND11_TEST=OFF
  STEP_TARGETS install
)

set(PYBIND11_TARGET pybind11)
set(PYBIND11_LIBRARIES pybind11)
set(PYBIND11_INCLUDE_DIR "${PYBIND11_DESTDIR}/include")
set(PYBIND11_CMAKE_MODULES_DIR "${PYBIND11_DESTDIR}/lib/cmake/pybind11")

find_package_handle_standard_args(pybind11
  FOUND_VAR PYBIND11_FOUND
  REQUIRED_VARS PYBIND11_INCLUDE_DIR PYBIND11_CMAKE_MODULES_DIR
  VERSION_VAR PYBIND11_VERSION)

# install(DIRECTORY ${PYBIND11_DESTDIR}/ DESTINATION ".")
else()
  set(BUILTIN_PYBIND FALSE)
endif()
else()
  set(pybind11_FOUND FALSE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
if (ENABLE_VC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_ARCHITECTURE_FLAGS}")
endif()
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")

SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${INCLUDES} -W -O3 -DNDEBUG")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} " )

include(VcMacros)
vc_set_preferred_compiler_flags()

if (USE_AVX2)
  include_directories(${Vc_INCLUDE_DIR})
  add_definitions("-DUSE_AVX2")
  string(REPLACE ";" " " VC_FLAGS "${Vc_COMPILE_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VC_FLAGS} -mavx2 -mfma -mbmi2")
endif()

set(generate_SOURCES
  src/generate/generate.cpp
  src/generate/storage.cpp
  src/generate/generate_scalar.cpp)

if (USE_AVX2)
  set(generate_SOURCES ${generate_SOURCES}
      src/generate/generate_avx2.cpp
      src/generate/vectorclass_ranvec1.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/generate)

add_library(generate SHARED ${generate_SOURCES})

if (USE_AVX2)
target_link_libraries(generate PUBLIC ${Vc_LIBRARIES})
target_include_directories(generate
    PUBLIC ${CMAKE_BINARY_DIR}/include/vectorclass)
add_dependencies(generate vectorclass-install)
target_compile_definitions(generate PUBLIC "-DVc_IMPL=AVX2")
endif()

if(pybind11_FOUND)
  include(pybind11Tools)
  pybind11_add_module(k40gen "src/k40gen/bindings.cpp")
  target_include_directories(k40gen PRIVATE ${PYTHON_SITE_PACKAGES}/numpy/core/include)
  target_link_libraries(k40gen PUBLIC generate)
  if(BUILTIN_PYBIND)
	add_dependencies(k40gen pybind11)
  endif()
endif()

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
