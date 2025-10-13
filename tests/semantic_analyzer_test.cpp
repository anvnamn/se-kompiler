#include "node.h"
#include "semantic_analyzer.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(SemanticAnalyzer, AnnotateGlobalVar) {
  const std::string expected_var_name = "this_var";
  constexpr Datatype expected_data_type = Datatype::INTEGER;

  auto var_decl = std::make_unique<VariableDeclarationNode>(
      expected_data_type, std::make_unique<IdentifierNode>(expected_var_name));

  auto var_decl_ptr = var_decl.get();

  std::vector<std::unique_ptr<StatementNode>> statements;
  statements.push_back(std::move(var_decl));

  Program program(std::move(statements));

  ASSERT_EQ(var_decl_ptr->variable->variable_annotation, nullptr);

  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program);

  ASSERT_NE(var_decl_ptr->variable->variable_annotation, nullptr);

  ASSERT_TRUE(var_decl_ptr->variable->variable_annotation->is_global);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->name,
            expected_var_name);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->stack_offset, 0);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->type,
            expected_data_type);
}

TEST(SemanticAnalyzer, DuplicateGlobalVar) {
  std::vector<std::unique_ptr<StatementNode>> statements;

  for (size_t i{0}; i < 2; i++) {
    const std::string expected_var_name = "this_var";
    constexpr Datatype expected_data_type = Datatype::INTEGER;

    auto var_decl = std::make_unique<VariableDeclarationNode>(
        expected_data_type,
        std::make_unique<IdentifierNode>(expected_var_name));

    statements.push_back(std::move(var_decl));
  }

  Program program(std::move(statements));

  auto analyzer = SemanticAnalyzer();
  ASSERT_ANY_THROW(analyzer.analyze_program(program));
}