find_package(Qt6 6.5 REQUIRED COMPONENTS Test Sql)

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# One executable per class; main.cpp, controller and QML are not under test.
# todolistmodel and todonotesmodel are not built yet: their tests still call
# addItemToList/removeItemFromList/addNoteToList/removeNoteFromList, which
# moved to Controller. Add them back to this list once the tests are updated.
foreach(name dbmanager logger eventlogsmodel)
  qt_add_executable(tst_${name}
    ${name}/tst_${name}.cpp
    testenv.h
    ${APP_DIR}/dbmanager.cpp
    ${APP_DIR}/logger.cpp
    ${APP_DIR}/eventlogsmodel.cpp
    ${APP_DIR}/todonotesmodel.cpp
    ${APP_DIR}/todolistmodel.cpp
  )
  # DBManager reads :/schema/databaseTemplate.sql from the resource system.
  qt_add_resources(tst_${name} schema_${name} PREFIX "/" BASE ${APP_DIR}
    FILES ${APP_DIR}/schema/databaseTemplate.sql)
  target_include_directories(tst_${name} PRIVATE ${APP_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(tst_${name} PRIVATE TEST_SCHEMA_DIR="${APP_DIR}/schema")
  target_link_libraries(tst_${name} PRIVATE Qt6::Test Qt6::Sql)
  add_test(NAME ${name} COMMAND tst_${name})
endforeach()
