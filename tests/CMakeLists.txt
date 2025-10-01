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

include(GoogleTest)
gtest_discover_tests(lexer_test)