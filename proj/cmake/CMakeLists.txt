cmake_minimum_required(VERSION 2.8...3.3)
project(myproject)

# Set version information in a config.h file
set(myproject_VERSION_MAJOR 1)
set(myproject_VERSION_MINOR 0)
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# Detect and add zlib
find_package(ZLIB REQUIRED)

# Detect and add boost
FIND_PACKAGE( Boost 1.70 COMPONENTS system filesystem REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Detect and add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

set(CBOE_LIBRARIES sfml-audio sfml-graphics sfml-network sfml-system Boost::filesystem Boost::system ZLIB::ZLIB ${OPENGL_LIBRARIES})
# create gitrev.hpp
set(CBOE_ROOT ${CMAKE_SOURCE_DIR}/../..)
set(CBOE_ROOT_SRC ${CBOE_ROOT}/src)
add_custom_command(
    OUTPUT ${CBOE_ROOT_SRC}/tools/gitrev.hpp
    COMMAND bash ${CBOE_ROOT}/pkg/gitrev.sh ${CBOE_ROOT_SRC}
)

add_custom_target(generate-gitrev DEPENDS ${CBOE_ROOT_SRC}/tools/gitrev.hpp)

# Computer dependent
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(EXTRA_LIBRARIES "-framework CoreFoundation -framework Cocoa  -framework OpenGL")
else()
#	find_package(TGUI 0.8 REQUIRED)
#	set(EXTRA_LIBRARIES ${TGUI_LIBRARY})
	set(EXTRA_LIBRARIES "-lX11 -ltgui") # changeme, we must find this automatically
endif()

# First compile common file
include_directories(${CBOE_ROOT_SRC}
		     ${CBOE_ROOT_SRC}/dialogxml ${CBOE_ROOT_SRC}/dialogxml/dialogs ${CBOE_ROOT_SRC}/dialogxml/widgets
		     ${CBOE_ROOT_SRC}/fileio ${CBOE_ROOT_SRC}/fileio/gzstream ${CBOE_ROOT_SRC}/fileio/resmgr ${CBOE_ROOT_SRC}/fileio/xml-parser
		     ${CBOE_ROOT_SRC}/gfx
		     ${CBOE_ROOT_SRC}/misc
		     ${CBOE_ROOT_SRC}/scenario
		     ${CBOE_ROOT_SRC}/tools
		     ${CBOE_ROOT_SRC}/universe )

file(GLOB commonSources
	  "${CBOE_ROOT_SRC}/*pp"
	  "${CBOE_ROOT_SRC}/dialogxml/*pp"  "${CBOE_ROOT_SRC}/dialogxml/dialogs/*pp" "${CBOE_ROOT_SRC}/dialogxml/widgets/*pp"
	  "${CBOE_ROOT_SRC}/fileio/*pp" "${CBOE_ROOT_SRC}/fileio/gzstream/gzstream.h" "${CBOE_ROOT_SRC}/fileio/gzstream/gzstream.cpp" "${CBOE_ROOT_SRC}/fileio/resmgr/*pp" "${CBOE_ROOT_SRC}/fileio/xml-parser/*pp"
	  "${CBOE_ROOT_SRC}/gfx/*pp"
	  "${CBOE_ROOT_SRC}/misc/*pp"
	  "${CBOE_ROOT_SRC}/scenario/*pp"
	  "${CBOE_ROOT_SRC}/tools/*pp"
	  "${CBOE_ROOT_SRC}/universe/*pp"
)
list(FILTER commonSources EXCLUDE REGEX ".*\\.(linux|win)\\.[hc]pp$")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   list(APPEND commonSources "${CBOE_ROOT_SRC}/tools/cursors.mac.mm" "${CBOE_ROOT_SRC}/tools/prefs.mac.mm" "${CBOE_ROOT_SRC}/tools/winutil.mac.mm")
else()
   list(APPEND commonSources "${CBOE_ROOT_SRC}/tools/cursors.linux.cpp" "${CBOE_ROOT_SRC}/tools/prefs.win.cpp" "${CBOE_ROOT_SRC}/tools/winutil.linux.cpp")
endif()
add_definitions("-DTIXML_USE_TICPP")
add_library(cboeCommon STATIC ${commonSources})
add_dependencies(cboeCommon generate-gitrev)

# Now compile the boe
file(GLOB boeSources
	  "${CBOE_ROOT_SRC}/game/*pp"
)
list(FILTER boeSources EXCLUDE REGEX ".*\\.(linux|win)\\.[hc]pp$")
list(APPEND boeSources "${CBOE_ROOT_SRC}/pcedit/pc.editors.cpp") # argh, this is not normal
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   list(REMOVE_ITEM boeSources "${CBOE_ROOT_SRC}/game/boe.menu.cpp")
   list(APPEND boeSources "${CBOE_ROOT_SRC}/game/boe.appleevents.mm" "${CBOE_ROOT_SRC}/game/boe.menus.mac.mm")
else()
   list(APPEND boeSources "${CBOE_ROOT_SRC}/game/boe.menus.linux.cpp")
endif()
add_executable(cboeGame ${boeSources})
set_target_properties(cboeGame PROPERTIES OUTPUT_NAME "Blades of Exile")
target_link_libraries(cboeGame cboeCommon "${CBOE_LIBRARIES}" "${EXTRA_LIBRARIES}")

# Now compile the character editor
file(GLOB charSources
	  "${CBOE_ROOT_SRC}/pcedit/*pp"
)
list(FILTER charSources EXCLUDE REGEX ".*\\.(linux|win)\\.[hc]pp$")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   list(REMOVE_ITEM charSources "${CBOE_ROOT_SRC}/pcedit/pc.menu.cpp")
   list(APPEND charSources "${CBOE_ROOT_SRC}/pcedit/pc.appleevents.mm" "${CBOE_ROOT_SRC}/pcedit/pc.menus.mac.mm")
else()
   list(APPEND charSources "${CBOE_ROOT_SRC}/pcedit/pc.menus.linux.cpp")
endif()
add_executable(cboeChar ${charSources})
set_target_properties(cboeChar PROPERTIES OUTPUT_NAME "Blades of Character Editor")
target_link_libraries(cboeChar cboeCommon "${CBOE_LIBRARIES}" "${EXTRA_LIBRARIES}")

# Now compile the scenario editor
file(GLOB scenSources
	  "${CBOE_ROOT_SRC}/scenedit/*pp"
)
list(FILTER scenSources EXCLUDE REGEX ".*\\.(linux|win)\\.[hc]pp$")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   list(REMOVE_ITEM scenSources "${CBOE_ROOT_SRC}/scenedit/scen.menu.cpp")
   list(APPEND scenSources "${CBOE_ROOT_SRC}/scenedit/scen.appleevents.mm" "${CBOE_ROOT_SRC}/scenedit/scen.menus.mac.mm")
else()
   list(APPEND scenSources "${CBOE_ROOT_SRC}/scenedit/scen.menus.linux.cpp")
endif()
add_executable(cboeScen ${scenSources})
set_target_properties(cboeScen PROPERTIES OUTPUT_NAME "BoE Scenario Editor")
target_link_libraries(cboeScen cboeCommon "${CBOE_LIBRARIES}" "${EXTRA_LIBRARIES}")





