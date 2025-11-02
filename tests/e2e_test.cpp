#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include "test_utils.h"
#include <gtest/gtest.h>

TEST(E2E, Return123) {
  auto const source_file = read_test_data("return123.se");

  TokenStream ts = tokenize(source_file);

  auto ast = parse_tokens(ts);

  SemanticAnalyzer semantic_analyzer;

  try {
    semantic_analyzer.analyze_program(&ast);
  } catch (const std::exception &e) {
    FAIL() << "Semantic analysis failed: " << e.what();
  }

  Codegen codegen;
  const auto asm_code = codegen.generate_assembly(ast);

  const auto exit_status = run_assembly(asm_code);
  ASSERT_EQ(exit_status, 123);
}
