#include "node.h"
#include "semantic_analyzer.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

class ProgramBuilder {
public:
  ProgramBuilder() {
    global_scope = std::make_unique<ScopeNode>(
        std::vector<std::unique_ptr<StatementNode>>{});

    auto main_body = std::make_unique<ScopeNode>(
        std::vector<std::unique_ptr<StatementNode>>{});

    main_function = std::make_unique<FunctionDefinitionNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>("main"),
        std::vector<std::unique_ptr<ParameterNode>>{}, std::move(main_body));
  }

  void add_global_statement(std::unique_ptr<StatementNode> stmt) {
    global_scope->statements.push_back(std::move(stmt));
  }

  void add_to_main(std::unique_ptr<StatementNode> stmt) {
    auto main_body_ptr = static_cast<ScopeNode *>(main_function->body.get());
    main_body_ptr->statements.push_back(std::move(stmt));
  }

  std::unique_ptr<ScopeNode> build() {
    add_global_statement(std::move(main_function));
    return std::move(global_scope);
  }

private:
  std::unique_ptr<ScopeNode> global_scope;
  std::unique_ptr<FunctionDefinitionNode> main_function;
};

TEST(GlobalVar, AnnotateGlobalVar) {
  const std::string expected_var_name = "global_var";
  constexpr Datatype expected_data_type = Datatype::INTEGER;

  auto var_decl = std::make_unique<VariableDeclarationNode>(
      expected_data_type, std::make_unique<IdentifierNode>(expected_var_name));

  auto var_decl_ptr = var_decl.get();

  ProgramBuilder builder;
  builder.add_global_statement(std::move(var_decl));

  auto program = builder.build();

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

TEST(BlockScope, DeclareIntegers) {
  std::vector<std::unique_ptr<StatementNode>> block_statements;
  const std::vector<std::string> var_names = {"var_one", "var_two"};
  const std::vector<int> expected_stack_offsets = {0, 4};

  for (auto name : var_names) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(name));

    block_statements.push_back(std::move(var_decl));
  }

  auto scope_node = std::make_unique<ScopeNode>(std::move(block_statements));
  auto scope_node_ptr = scope_node.get();

  ProgramBuilder builder;
  builder.add_to_main(std::move(scope_node));
  auto program = builder.build();
  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  // Check annotation
  auto main_func =
      dynamic_cast<FunctionDefinitionNode *>(program->statements[0].get());
  ASSERT_EQ(main_func->body->statements.size(), 1);
  const auto scope_annotation = scope_node_ptr->scope_annotation;
  ASSERT_EQ(scope_annotation.stack_size, 8);
  for (int i{0}; i < var_names.size(); i++) {
    const auto var_name = var_names[i];
    const auto stack_offset =
        scope_annotation.variables.at(var_names[i]).get()->stack_offset;
    ASSERT_TRUE(scope_annotation.variables.contains(var_names[i]));
    ASSERT_EQ(stack_offset, expected_stack_offsets[i]);
  }
}

TEST(BlockScope, DuplicateDeclaration) {
  std::vector<std::unique_ptr<StatementNode>> block_statements;
  const auto var_names = {"duplicate_var", "duplicate_var"};

  for (auto name : var_names) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(name));

    block_statements.push_back(std::move(var_decl));
  }

  auto scope_node = std::make_unique<ScopeNode>(std::move(block_statements));

  ProgramBuilder builder;
  builder.add_to_main(std::move(scope_node));
  auto program = builder.build();
  auto analyzer = SemanticAnalyzer();

  try {
    analyzer.analyze_program(program.get());
    FAIL() << "Expected exception due to duplicate variable declaration";
  } catch (const std::runtime_error &e) {
    std::string msg = e.what();
    EXPECT_TRUE(msg.starts_with("Duplicate variable declaration"));
  } catch (...) {
    FAIL() << "Expected std::runtime error, but caught a different exception "
              "type";
  }
}

TEST(BlockScope, NestedScopes) {
  const auto var_name = "nested_var";

  std::unique_ptr<ScopeNode> scope = nullptr;
  std::vector<ScopeNode *> scope_raw_ptrs;

  for (int i{0}; i < 4; i++) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(var_name));

    std::vector<std::unique_ptr<StatementNode>> block_statements;
    block_statements.push_back(std::move(var_decl));

    if (!scope) {
      scope = std::make_unique<ScopeNode>(std::move(block_statements));
    } else {
      block_statements.push_back(std::move(scope));
      scope = std::make_unique<ScopeNode>(std::move(block_statements));
    }
    scope_raw_ptrs.push_back(scope.get());
  }

  ProgramBuilder builder;

  builder.add_to_main(std::move(scope));
  auto program = builder.build();

  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  for (auto ptr : scope_raw_ptrs) {
    ASSERT_EQ(ptr->scope_annotation.stack_size, 4);
  }
}
