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

add_executable(
  semantic_analyzer_test
  semantic_analyzer_test.cpp
  )

target_link_libraries(
  semantic_analyzer_test
  mycompiler_lib
  GTest::gtest_main
)

add_executable(
  codegen_test
  codegen_test.cpp
  )

target_link_libraries(
  codegen_test
  mycompiler_lib
  GTest::gtest_main
)

add_executable(
  e2e_test
  e2e_test.cpp
)
target_link_libraries(
  e2e_test
  mycompiler_lib
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lexer_test)
gtest_discover_tests(parser_test)
gtest_discover_tests(semantic_analyzer_test)
gtest_discover_tests(codegen_test)
gtest_discover_tests(e2e_test)
