# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_safe_landing_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED safe_landing_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(safe_landing_FOUND FALSE)
  elseif(NOT safe_landing_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(safe_landing_FOUND FALSE)
  endif()
  return()
endif()
set(_safe_landing_CONFIG_INCLUDED TRUE)

# output package information
if(NOT safe_landing_FIND_QUIETLY)
  message(STATUS "Found safe_landing: 0.0.0 (${safe_landing_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'safe_landing' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${safe_landing_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(safe_landing_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${safe_landing_DIR}/${_extra}")
endforeach()
