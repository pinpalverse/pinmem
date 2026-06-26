cmake_minimum_required(VERSION 4.3)
project(pinmem VERSION 1.0 DESCRIPTION "The memory analysis library for pinpal" LANGUAGES C)
include(GNUInstallDirs)

# ---------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------
find_package(pinlog REQUIRED)


set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


set(CMAKE_BUILD_RPATH 
    "${CMAKE_CURRENT_BINARY_DIR}:$<TARGET_FILE_DIR:pinlog::pinlog>"
)

# Install-time: look in the standard library install directory
set(CMAKE_INSTALL_RPATH 
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)

# ---------------------------------------------------------------------
# Libraries
# ---------------------------------------------------------------------
add_library(pinmem SHARED src/pinmem.c)
add_library(pinmem_lite SHARED src/pinmem_lite.c)

add_library(pinmem_static STATIC src/pinmem.c)
add_library(pinmem_lite_static STATIC src/pinmem_lite.c)

set_target_properties(pinmem PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pinmem/pinmem.h)

set_target_properties(pinmem_lite PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pinmem/pinmem_lite.h)

set_target_properties(pinmem_static PROPERTIES
    OUTPUT_NAME pinmem
    CLEAN_DIRECT_OUTPUT 1)
set_target_properties(pinmem_lite_static PROPERTIES
    OUTPUT_NAME pinmem_lite
    CLEAN_DIRECT_OUTPUT 1)

target_link_libraries(pinmem PRIVATE pinlog)
target_link_libraries(pinmem_lite PRIVATE pinlog)

target_include_directories(pinmem PRIVATE include/)
target_include_directories(pinmem_lite PRIVATE include/)
target_include_directories(pinmem_static PRIVATE include/)
target_include_directories(pinmem_lite_static PRIVATE include/)

# ---------------------------------------------------------------------
# Examples (build only, not installed)
# ---------------------------------------------------------------------
add_executable(example example.c)
add_executable(example_lite example_lite.c)
target_link_libraries(example pinmem)
target_link_libraries(example_lite pinmem_lite)
target_include_directories(example PRIVATE include)
target_include_directories(example_lite PRIVATE include)

# ---------------------------------------------------------------------
# pkg-config files (optional, for legacy use)
# ---------------------------------------------------------------------
configure_file(pinmem.pc.in pinmem.pc @ONLY)
configure_file(pinmem_lite.pc.in pinmem_lite.pc @ONLY)

# ---------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------
install(TARGETS pinmem pinmem_lite pinmem_static pinmem_lite_static
    EXPORT pinmemTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # for static libraries
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pinmem/
)

install(FILES ${CMAKE_BINARY_DIR}/pinmem.pc
              ${CMAKE_BINARY_DIR}/pinmem_lite.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# ---------------------------------------------------------------------
# CMake package configuration (for find_package)
# ---------------------------------------------------------------------
include(CMakePackageConfigHelpers)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/pinmemConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Create a config file template
set(CONFIG_TEMPLATE ${CMAKE_BINARY_DIR}/pinmemConfig.cmake.in)
file(WRITE ${CONFIG_TEMPLATE}
"@PACKAGE_INIT@

# Find our dependency
find_package(pinlog REQUIRED)

# Import targets
include(\${CMAKE_CURRENT_LIST_DIR}/pinmemTargets.cmake)

# Provide convenience aliases (optional)
if(NOT TARGET pinmem::pinmem)
    add_library(pinmem::pinmem ALIAS pinmem)
endif()
if(NOT TARGET pinmem::pinmem_lite)
    add_library(pinmem::pinmem_lite ALIAS pinmem_lite)
endif()
")

# Configure the file (this substitutes @PACKAGE_INIT@ and paths)
configure_package_config_file(
    ${CONFIG_TEMPLATE}
    ${CMAKE_BINARY_DIR}/pinmemConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pinmem
    PATH_VARS CMAKE_INSTALL_PREFIX
)

# Install config and version files
install(FILES
    ${CMAKE_BINARY_DIR}/pinmemConfig.cmake
    ${CMAKE_BINARY_DIR}/pinmemConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pinmem
)

# Export the targets (so they become imported targets for consumers)
install(EXPORT pinmemTargets
    FILE pinmemTargets.cmake
    NAMESPACE pinmem::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pinmem
)

# (Optional) Export from build tree for testing
export(EXPORT pinmemTargets
    FILE ${CMAKE_BINARY_DIR}/pinmemTargets.cmake
    NAMESPACE pinmem::
)
