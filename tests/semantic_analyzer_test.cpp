#include "node.h"
#include "semantic_analyzer.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(GlobalVar, AnnotateGlobalVar) {
  const std::string expected_var_name = "this_var";
  constexpr Datatype expected_data_type = Datatype::INTEGER;

  auto var_decl = std::make_unique<VariableDeclarationNode>(
      expected_data_type, std::make_unique<IdentifierNode>(expected_var_name));

  auto var_decl_ptr = var_decl.get();

  std::vector<std::unique_ptr<StatementNode>> statements;
  statements.push_back(std::move(var_decl));

  auto program = std::make_unique<ScopeNode>(std::move(statements));

  ASSERT_EQ(var_decl_ptr->variable->variable_annotation, nullptr);

  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  ASSERT_NE(var_decl_ptr->variable->variable_annotation, nullptr);

  ASSERT_TRUE(var_decl_ptr->variable->variable_annotation->is_global);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->name,
            expected_var_name);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->stack_offset, 0);
  ASSERT_EQ(var_decl_ptr->variable->variable_annotation->type,
            expected_data_type);
}

TEST(GlobalVar, DuplicateGlobalVar) {
  std::vector<std::unique_ptr<StatementNode>> statements;

  for (size_t i{0}; i < 2; i++) {
    const std::string expected_var_name = "this_var";
    constexpr Datatype expected_data_type = Datatype::INTEGER;

    auto var_decl = std::make_unique<VariableDeclarationNode>(
        expected_data_type,
        std::make_unique<IdentifierNode>(expected_var_name));

    statements.push_back(std::move(var_decl));
  }

  auto program = std::make_unique<ScopeNode>(std::move(statements));

  auto analyzer = SemanticAnalyzer();
  ASSERT_ANY_THROW(analyzer.analyze_program(program.get()));
}

TEST(BlockScope, StackVars) {
  std::vector<std::unique_ptr<StatementNode>> block_statements;
  const auto var_names = {"var_one", "var_two"};

  for (auto name : var_names) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(name));

    block_statements.push_back(std::move(var_decl));
  }

  auto scope_node = std::make_unique<ScopeNode>(std::move(block_statements));

  std::vector<std::unique_ptr<StatementNode>> program_statements;
  program_statements.push_back(std::move(scope_node));
  auto program = std::make_unique<ScopeNode>(std::move(program_statements));
  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  ASSERT_EQ(analyzer.scope_stack.size(), 2);
  ASSERT_EQ(analyzer.scope_stack.back().stack_size, 8);
}