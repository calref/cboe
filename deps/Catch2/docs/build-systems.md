# Integration with build systems

Build Systems may refer to low-level tools, like CMake, or larger systems that run on servers, like Jenkins or TeamCity. This page will talk about both.

# Continuous Integration systems

Probably the most important aspect to using Catch with a build server is the use of different reporters. Catch comes bundled with three reporters that should cover the majority of build servers out there - although adding more for better integration with some is always a possibility (as has been done with TeamCity).

Two of these reporters are built in (XML and JUnit) and the third (TeamCity) is included as a separate header. It's possible that the other two may be split out in the future too - as that would make the core of Catch smaller for those that don't need them.

## XML Reporter
```-r xml``` 

The XML Reporter writes in an XML format that is specific to Catch. 

The advantage of this format is that it corresponds well to the way Catch works (especially the more unusual features, such as nested sections) and is a fully streaming format - that is it writes output as it goes, without having to store up all its results before it can start writing.

The disadvantage is that, being specific to Catch, no existing build servers understand the format natively. It can be used as input to an XSLT transformation that could covert it to, say, HTML - although this loses the streaming advantage, of course.

## JUnit Reporter
```-r junit```

The JUnit Reporter writes in an XML format that mimics the JUnit ANT schema.

The advantage of this format is that the JUnit Ant schema is widely understood by most build servers and so can usually be consumed with no additional work.

The disadvantage is that this schema was designed to correspond to how JUnit works - and there is a significant mismatch with how Catch works. Additionally the format is not streamable (because opening elements hold counts of failed and passing tests as attributes) - so the whole test run must complete before it can be written.

## TeamCity Reporter
```-r teamcity```

The TeamCity Reporter writes TeamCity service messages to stdout. In order to be able to use this reporter an additional header must also be included.

```catch_reporter_teamcity.hpp``` can be found in the ```include\reporters``` directory. It should be included in the same file that ```#define```s ```CATCH_CONFIG_MAIN``` or ```CATCH_CONFIG_RUNNER```. The ```#include``` should be placed after ```#include```ing Catch itself.

e.g.:

```
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "catch_reporter_teamcity.hpp"
```

Being specific to TeamCity this is the best reporter to use with it - but it is completely unsuitable for any other purpose. It is a streaming format (it writes as it goes) - although test results don't appear in the TeamCity interface until the completion of a suite (usually the whole test run).

# Low-level tools

## CMake

You can use the following CMake script to automatically fetch Catch from github and configure it as an external project:

```CMake
cmake_minimum_required(VERSION 2.8.8)
project(catch_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    catch
    PREFIX ${CMAKE_BINARY_DIR}/catch
    GIT_REPOSITORY https://github.com/philsquared/Catch.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
   )

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(catch source_dir)
set(CATCH_INCLUDE_DIR ${source_dir}/include CACHE INTERNAL "Path to include folder for Catch")
```

If you put it in, e.g., `${PROJECT_SRC_DIR}/${EXT_PROJECTS_DIR}/catch/`, you can use it in your project by adding the following to your root CMake file:

```CMake
# Includes Catch in the project:
add_subdirectory(${EXT_PROJECTS_DIR}/catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.
```

---

[Home](Readme.md)