include(FindPackageHandleStandardArgs)

if(SYSREPO_LIBRARIES AND SYSREPO_INCLUDE_DIRS)
    # in cache already
    set(SYSREPO_FOUND TRUE)
else()
    find_path(SYSREPO_INCLUDE_DIR
        NAMES sysrepo.h
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include
        ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(SYSREPO_LIBRARY
        NAMES sysrepo libsysrepo libsysrepo.so.6.4.19
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib
        ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if(SYSREPO_INCLUDE_DIR)
        find_path(SR_VERSION_PATH "sysrepo/version.h" HINTS ${SYSREPO_INCLUDE_DIR})
        file(READ "${SR_VERSION_PATH}/sysrepo/version.h" SR_VERSION_FILE)
        string(REGEX MATCH "#define SR_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\"" SR_VERSION_MACRO "${SR_VERSION_FILE}")
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" SYSREPO_VERSION "${SR_VERSION_MACRO}")
    endif()

    set(SYSREPO_INCLUDE_DIRS ${SYSREPO_INCLUDE_DIR})
    set(SYSREPO_LIBRARIES ${SYSREPO_LIBRARY})
    mark_as_advanced(SYSREPO_INCLUDE_DIRS SYSREPO_LIBRARIES)

    # handle the QUIETLY and REQUIRED arguments and set SYSREPO_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(Sysrepo FOUND_VAR SYSREPO_FOUND
        REQUIRED_VARS SYSREPO_LIBRARY SYSREPO_INCLUDE_DIR
        VERSION_VAR SYSREPO_VERSION)
endif()
