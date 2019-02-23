# xtl
set(XTL_VERSION 0.5.4)

find_package(xtl ${XTL_VERSION} QUIET)
if (xtl_FOUND)
  message(STATUS "Found xtl: ${xtl_INCLUDE_DIRS}/xtl")
  set(xtl_BUILTIN FALSE)
else()
  set(xtl_BUILTIN TRUE)
  message(STATUS "Building internal xtl version ${XTL_VERSION}")
  set(XTL_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")

  ExternalProject_Add(xtl
    URL "https://github.com/QuantStack/xtl/archive/${XTL_VERSION}.tar.gz"
    URL_HASH SHA256=35478bb08949d0c36d4cf24cabbaa8322c507f8247cc69e017bddb2e28ffaf15
    BUILD_IN_SOURCE 0
    INSTALL_DIR ${XTL_DESTDIR}
    LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    CMAKE_ARGS -G ${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    STEP_TARGETS install)
endif()

# xsimd
if (USE_AVX2)
  set(XSIMD_VERSION 7.1.3)
  find_package(xsimd ${XSIMD_VERSION} QUIET)

  if (xsimd_FOUND)
    message(STATUS "Found xsimd: ${xsimd_INCLUDE_DIRS}/xsimd")
    set(xsimd_BUILTIN FALSE)
  else()
    set(xsimd_BUILTIN TRUE)
    message(STATUS "Building internal xsimd version ${XSIMD_VERSION}")
    set(XSIMD_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")

    ExternalProject_Add(xsimd
      URL "https://github.com/QuantStack/xsimd/archive/${XSIMD_VERSION}.tar.gz"
      URL_HASH SHA256=faf98c4a29883a33ef844a2eac364762e35098aacc29bb9e255f799caf81a38e
      BUILD_IN_SOURCE 0
      INSTALL_DIR ${XSIMD_DESTDIR}
      LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
      CMAKE_ARGS -G ${CMAKE_GENERATOR}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)
    if (xtl_BUILTIN)
      ExternalProject_Add_StepDependencies(xsimd configure xtl)
    endif()
  endif()
endif()

# xtensor
set(XTENSOR_VERSION 0.19.4)
find_package(xtensor ${XTENSOR_VERSION} QUIET)

if (xtensor_FOUND)
  message(STATUS "Found xtensor: ${xtensor_INCLUDE_DIRS}/xtensor")
  set(xtensor_BUILTIN FALSE)
else()
  set(xtensor_BUILTIN TRUE)
  message(STATUS "Building internal xtensor version ${XTENSOR_VERSION}")
  set(XTENSOR_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")

  ExternalProject_Add(xtensor
    URL "https://github.com/QuantStack/xtensor/archive/${XTENSOR_VERSION}.tar.gz"
    URL_HASH SHA256=ea0ed42ac27888f4e4acaf99367fbef714373fa586f204e8bc22b8e5335ecf06
    BUILD_IN_SOURCE 0
    INSTALL_DIR ${XTENSOR_DESTDIR}
    LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    CMAKE_ARGS -G ${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DXTENSOR_ENABLE_XSIMD=$<BOOL:USE_AVX2>)
  if (xtl_BUILTIN)
    ExternalProject_Add_StepDependencies(xtensor configure xtl)
  endif()
  if (USE_AVX2 AND xsimd_BUILTIN)
    ExternalProject_Add_StepDependencies(xtensor configure xsimd)
  endif()
endif()

# xtensor-python
set(XTENSOR_PYTHON_VERSION 0.22.1)
find_package(xtensor-python ${XTENSOR_PYTHON_VERSION} QUIET)

if (xtensor-python_FOUND)
  message(STATUS "Found xtensor-python: ${xtensor-python_INCLUDE_DIRS}/xtensor-python")
  set(xtp_BUILTIN FALSE)
else()
  message(STATUS "Building internal xtensor-python version ${XTENSOR_PYTHON_VERSION}")
  set(xtp_BUILTIN TRUE)
  set(XTENSOR_PYTHON_DESTDIR "${CMAKE_BINARY_DIR}/${EXTERNAL_DIR}")

  ExternalProject_Add(xtp
    URL "https://github.com/QuantStack/xtensor-python/archive/${XTENSOR_PYTHON_VERSION}.tar.gz"
    URL_HASH SHA256=b92ce8979d54bd88e3613920208a13046f5944df6ada38711f4a89636c55304d
    BUILD_IN_SOURCE 0
    INSTALL_DIR ${XTENSOR_PYTHON_DESTDIR}
    LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    CMAKE_ARGS -G ${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/cmake
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    STEP_TARGETS install)
  if (xtensor_BUILTIN)
    ExternalProject_Add_StepDependencies(xtp configure xtensor)
  endif()
endif()
