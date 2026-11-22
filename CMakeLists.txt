cmake_minimum_required(VERSION 3.16)

project(MVCPatternExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Sql)

qt_add_executable(MVCPatternExample WIN32
  main.cpp
  dbmanager.cpp
  dbmanager.h
  eventlogsmodel.cpp
  eventlogsmodel.h
  logger.cpp
  logger.h
  todolistmodel.cpp
  todolistmodel.h
  todonotesmodel.cpp
  todonotesmodel.h
  qml.qrc
)

target_link_libraries(MVCPatternExample PRIVATE Qt6::Quick Qt6::Sql)

# Uncomment once the build is clean to flag APIs deprecated before Qt 6.5.
#target_compile_definitions(MVCPatternExample PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0x060500)

# DBManager reads ./schema/databaseTemplate.sql relative to the working
# directory, which is the build-dir root when run from Qt Creator. Keep the
# exe there and copy the schema next to it.
set_target_properties(MVCPatternExample PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_command(TARGET MVCPatternExample POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/schema ${CMAKE_BINARY_DIR}/schema
)

include(GNUInstallDirs)
install(TARGETS MVCPatternExample
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY schema DESTINATION ${CMAKE_INSTALL_BINDIR})
