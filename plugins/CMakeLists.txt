set(PLUGINS serial)

file(GLOB plugins_SOURCES
  "*.h" "*.cpp")
list(APPEND arduino_SOURCES ${plugins_SOURCES})
set(arduino_SOURCES ${arduino_SOURCES} PARENT_SCOPE)

foreach(PLUGIN ${PLUGINS})
  file(GLOB plugin_SOURCES
    "${PLUGIN}/*.h"
    "${PLUGIN}/*.cpp")

  file(GLOB plugin_UIS
    "${PLUGIN}/*.ui")

  file(GLOB plugin_MOCS
    "${PLUGIN}/*.h")

  file(GLOB plugin_QRCS
    "${PLUGIN}/*.qrc")

  qt4_wrap_ui(plugin_UI_SOURCES ${plugin_UIS})
  qt4_wrap_cpp(plugin_MOC_SOURCES ${plugin_MOCS})
  qt4_add_resources(plugin_QRC_SOURCES ${plugin_QRCS})

  add_library(${PLUGIN} MODULE
    ${plugins_SOURCES}
    ${plugin_SOURCES}
    ${plugin_UI_SOURCES}
    ${plugin_MOC_SOURCES}
    ${plugin_QRC_SOURCES})
  set_target_properties(${PLUGIN} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
endforeach()

# Install
if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
  install(TARGETS ${PLUGINS}
    LIBRARY DESTINATION "${arduino_PLUGIN_PATH}")
endif()