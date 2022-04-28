# Find agent++ library
#
# Optional: AGENT_PP_ROOT_DIR - where agent++ library is installed
#
# The following variables are set:
#  AGENT_PP_FOUND
#  AGENT_PP_INCLUDE_DIR
#  AGENT_PP_LIBRARIES

find_path(AGENT_PP_INCLUDE_DIR_TMP NAMES agent_pp/agent++.h PATHS ${AGENT_PP_ROOT_DIR} ../agent++ ../../agent++ PATH_SUFFIXES include)

if(AGENT_PP_INCLUDE_DIR_TMP)
  set(AGENT_PP_INCLUDE_DIR "${AGENT_PP_INCLUDE_DIR_TMP}" "${AGENT_PP_INCLUDE_DIR_TMP}/agent_pp")
endif(AGENT_PP_INCLUDE_DIR_TMP)

UNSET(AGENT_PP_INCLUDE_DIR_TMP)

find_library(AGENT_PP_LIBRARIES NAMES agent++ libagent++ PATHS ${AGENT_PP_ROOT_DIR} ../agent++ ../../agent++ PATH_SUFFIXES lib build)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(agent_pp DEFAULT_MSG AGENT_PP_INCLUDE_DIR AGENT_PP_LIBRARIES)

if(AGENT_PP_FOUND)
  message(STATUS "Found agent++ (include: ${AGENT_PP_INCLUDE_DIR}, library: ${AGENT_PP_LIBRARIES})")
  mark_as_advanced(AGENT_PP_INCLUDE_DIR AGENT_PP_LIBRARIES)
else(AGENT_PP_FOUND)
  message(STATUS "agent++ NOT found (include: ${AGENT_PP_INCLUDE_DIR}, library: ${AGENT_PP_LIBRARIES})")
endif(AGENT_PP_FOUND)


