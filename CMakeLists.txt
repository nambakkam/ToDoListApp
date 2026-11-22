cmake_minimum_required(VERSION 3.16)

project(MVCPatternExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Sql)

qt_add_executable(MVCPatternExample WIN32
  main.cpp
  controller.cpp
  controller.h
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

# schema/databaseTemplate.sql is compiled into the executable through qml.qrc
# (read as :/schema/databaseTemplate.sql), so no post-build copy is needed.
# database.db is created in the working directory.

include(GNUInstallDirs)
install(TARGETS MVCPatternExample
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt Test suite: one executable per class, because DBManager is a singleton
# that cannot be reset within a process.
option(BUILD_TESTING "Build the Qt Test suite" ON)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
