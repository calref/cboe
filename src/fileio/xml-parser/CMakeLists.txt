cmake_minimum_required(VERSION 3.21)

project(ticpp VERSION 2.5.3 LANGUAGES CXX)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(PROJECT_IS_TOP_LEVEL)
  set(buildTypes "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  set(defaultBuildType "Debug")
  if(NOT isMultiConfig)
    if(NOT CMAKE_BUILD_TYPE)
      message(STATUS "Setting build type to '${defaultBuildType}' as none was specified.")
      set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
      set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${buildTypes}")
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST buildTypes)
      message(FATAL_ERROR "Unknown build type: '${CMAKE_BUILD_TYPE}'")
    endif()
  endif()
  unset(defaultBuildType)
  unset(buildTypes)

  # TODO: This is not the ideal solution to apply these warning flags.
  #       Toolchain files are not really intended for this purpose.
  #       Presets would be a viable solution, but currently, at least on VSCode, this
  #       has the side effect that pretty much everything from generator to build type
  #       to binary directory needs to be specified. The usual control ability of especially
  #       the build type gets lost in preset mode.
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(APPEND CMAKE_CXX_FLAGS " /W4")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # This assumes the default Clang frontend and not the MSVC compatible one
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
  else()
    # This assumes GCC
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
  endif()
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
elseif(CMAKE_CXX_STANDARD LESS 17 OR CMAKE_CXX_STANDARD GREATER_EQUAL 98)
  message(FATAL_ERROR "The CMAKE_CXX_STANDARD value needs to be at least 17, current value: ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(MSVC)
  # Disable warnings about standard conformant code that is not conform to Microsoft standards
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
  # Without BOM the UTF-8 encoding doesn't get detected so enforce it
  add_compile_options(/source-charset:utf-8)
endif()

add_library(ticpp_ticpp)
add_library(ticpp::ticpp ALIAS ticpp_ticpp)
set_target_properties(ticpp_ticpp PROPERTIES
  OUTPUT_NAME ticpp
)

target_sources(ticpp_ticpp
  PRIVATE
    ticpp.cpp
    ticpp.h
    ticppapi.h
    ticpprc.h
    tinystr.cpp
    tinystr.h
    tinyxml.cpp
    tinyxml.h
    tinyxmlparser.cpp
    tinyxmlerror.cpp
)

target_compile_definitions(ticpp_ticpp
  PUBLIC
    TIXML_USE_TICPP
    $<$<BOOL:${BUILD_SHARED_LIBS}>:BUILD_TICPP_DLL>
)
target_include_directories(ticpp_ticpp
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

if(PROJECT_IS_TOP_LEVEL)
  set(excludeFromAllTag "")
else()
  set(excludeFromAllTag EXCLUDE_FROM_ALL)
endif()
install(TARGETS ticpp_ticpp
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ${excludeFromAllTag}
)
unset(excludeFromAllTag)
