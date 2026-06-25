cmake_minimum_required(VERSION 4.3)
project(pinmem VERSION 1.0 DESCRIPTION "The memory analysis library for pinpal" LANGUAGES C)
include(GNUInstallDirs)

find_package(pinlog REQUIRED)

add_library(pinmem SHARED src/pinmem.c)
add_library(pinmem_lite SHARED src/pinmem_lite.c)
set_target_properties(pinmem PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pinmem.h)

set_target_properties(pinmem_lite PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pinmem_lite.h)

configure_file(pinmem.pc.in pinmem.pc @ONLY)
configure_file(pinmem_lite.pc.in pinmem_lite.pc @ONLY)

target_include_directories(pinmem PRIVATE include/)
target_include_directories(pinmem_lite PRIVATE include/)

add_executable(example example.c)
add_executable(example_lite example_lite.c)

target_link_libraries(pinmem PRIVATE pinlog)
target_link_libraries(pinmem_lite PRIVATE pinlog)
target_link_libraries(example pinmem)
target_link_libraries(example_lite pinmem_lite)

target_include_directories(example PRIVATE include)
target_include_directories(example_lite PRIVATE include)

install(TARGETS pinmem pinmem_lite
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/pinmem.pc
              ${CMAKE_BINARY_DIR}/pinmem_lite.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
