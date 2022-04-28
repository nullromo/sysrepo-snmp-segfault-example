cmake_minimum_required(VERSION 3.1)
project(example)

# use debug config by default
if(NOT "${CMAKE_BUILD_TYPE}")
    SET(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules)

find_package (snmp_pp)
if (NOT SNMP_PP_FOUND)
  message (FATAL_ERROR " Lib snmp++ NOT found, try to call cmake with '-D SNMP_PP_ROOT_DIR=/install/path')")
endif ()

find_package (agent_pp)
if (NOT AGENT_PP_FOUND)
  message (FATAL_ERROR " Lib agent++ NOT found, try to call cmake with '-D AGENT_PP_FOUND=/install/path')")
endif ()

find_package(Sysrepo)
find_package(LibYANG)

## SNMP
set(EXAMPLE_SNMP_SRC
    src/exampleSNMPAgent.cpp
)
set(EXAMPLE_SNMP_INC
    inc/
)
add_executable(example-snmp ${EXAMPLE_SNMP_SRC})
target_include_directories(example-snmp
    PUBLIC
    ${EXAMPLE_SNMP_INC}
    ${SNMP_PP_INCLUDE_DIR}
    ${AGENT_PP_INCLUDE_DIR}
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
)
target_link_libraries(
    example-snmp
    ${AGENT_PP_LIBRARIES}
    ${SNMP_PP_LIBRARIES}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
)

## Plugin
SET(PLUGIN_INCLUDE_DIRS
    inc/
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
)
SET(PLUGIN_LIBRARIES
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
)
set(EXAMPLE_PLUGIN_SRC
    src/examplePlugin.cpp
    src/pluginBase.cpp
)

# standalone executable plugin
add_executable(example-standalone-plugin ${EXAMPLE_PLUGIN_SRC})
target_compile_definitions(example-standalone-plugin PUBLIC -DSTANDALONE)
target_include_directories(example-standalone-plugin PRIVATE ${PLUGIN_INCLUDE_DIRS})
target_link_libraries(example-standalone-plugin ${PLUGIN_LIBRARIES})
