include(FindPackageHandleStandardArgs)

if(LIBYANG_LIBRARIES AND LIBYANG_INCLUDE_DIRS)
    # in cache already
    set(LIBYANG_FOUND TRUE)
else()
    find_path(LIBYANG_INCLUDE_DIR
        NAMES
        libyang/libyang.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_INCLUDE_PATH}
        ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(LIBYANG_LIBRARY
        NAMES
        yang
        libyang
        libyang.so.2.13.7
        PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /opt/local/lib
        /sw/lib
        ${CMAKE_LIBRARY_PATH}
        ${CMAKE_INSTALL_PREFIX}/lib
    )

    if(LIBYANG_INCLUDE_DIR)
        find_path(LY_VERSION_PATH "libyang/version.h" HINTS ${LIBYANG_INCLUDE_DIR})
        if(LY_VERSION_PATH)
            file(READ "${LY_VERSION_PATH}/libyang/version.h" LY_VERSION_FILE)
        else()
            find_path(LY_HEADER_PATH "libyang/libyang.h" HINTS ${LIBYANG_INCLUDE_DIR})
            file(READ "${LY_HEADER_PATH}/libyang/libyang.h" LY_VERSION_FILE)
        endif()
        string(REGEX MATCH "#define LY_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\"" LY_VERSION_MACRO "${LY_VERSION_FILE}")
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" LIBYANG_VERSION "${LY_VERSION_MACRO}")
    endif()

    set(LIBYANG_INCLUDE_DIRS ${LIBYANG_INCLUDE_DIR})
    set(LIBYANG_LIBRARIES ${LIBYANG_LIBRARY})
    mark_as_advanced(LIBYANG_INCLUDE_DIRS LIBYANG_LIBRARIES)

    # handle the QUIETLY and REQUIRED arguments and set LIBYANG_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(LibYANG FOUND_VAR LIBYANG_FOUND
        REQUIRED_VARS LIBYANG_LIBRARY LIBYANG_INCLUDE_DIR
        VERSION_VAR LIBYANG_VERSION)
endif()
