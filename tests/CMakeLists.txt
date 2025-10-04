add_compile_definitions(
  TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

add_executable(
  lexer_test
  lexer_test.cpp
)
target_link_libraries(
  lexer_test
  mycompiler_lib
  GTest::gtest_main
)

add_executable(
  parser_test
  parser_test.cpp
)
target_link_libraries(
  parser_test
  mycompiler_lib
  GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(lexer_test)
gtest_discover_tests(parser_test)