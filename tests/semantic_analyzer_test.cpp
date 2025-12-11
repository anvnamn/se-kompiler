// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-goto,
// bugprone-unchecked-optional-access)

#include "node.h"
#include "semantic_analyzer.h"
#include "test_utils.h"
#include <fmt/format.h>
#include <gtest/gtest.h>

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

TEST(LocalVar, AnnotateLocalVar) {
  // Local variable names and expected stack offsets
  std::vector<std::pair<std::string, int>> local_vars = {
      {"local_var_one", 0},
      {"local_var_two", 4},
  };

  ProgramBuilder builder;
  for (const auto &[var_name, _] : local_vars) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(var_name));
    builder.add_to_main(std::move(var_decl));
  }
  builder.add_to_main(
      std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>(0)));
  auto program = builder.build();

  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  // Verify annotations
  auto main_func =
      dynamic_cast<FunctionDefinitionNode *>(program->statements[0].get());
  ASSERT_EQ(main_func->body->statements.size(),
            local_vars.size() + 1); // vars + return stmt
  const auto scope_annotation = main_func->body->scope_annotation;
  ASSERT_NE(scope_annotation, std::nullopt);
  ASSERT_EQ(scope_annotation->stack_size, 8); // 2 integers = 8 bytes
  for (const auto &[var_name, expected_offset] : local_vars) {
    ASSERT_TRUE(scope_annotation->variables.contains(var_name));
    const auto var_info = scope_annotation->variables.at(var_name).get();
    ASSERT_EQ(var_info->stack_offset, expected_offset);
    ASSERT_EQ(var_info->name, var_name);
    ASSERT_EQ(var_info->type, Datatype::INTEGER);
    ASSERT_FALSE(var_info->is_global);
  }
}

TEST(LocalVar, DuplicateLocalVar) {
  ProgramBuilder builder;

  const std::string duplicate_var_name = "duplicate_var";

  for (size_t i{0}; i < 2; i++) {
    auto var_decl = std::make_unique<VariableDeclarationNode>(
        Datatype::INTEGER,
        std::make_unique<IdentifierNode>(duplicate_var_name));
    builder.add_to_main(std::move(var_decl));
  }

  auto program = builder.build();

  auto analyzer = SemanticAnalyzer();
  ASSERT_ANY_THROW(analyzer.analyze_program(program.get()));
}

TEST(AnnotateIdentifier, ReturnStatement) {
  auto ident_node = std::make_unique<IdentifierNode>("global_var");

  auto var_info = std::make_shared<VariableInfo>();
  var_info->name = "global_var";
  var_info->type = Datatype::INTEGER;
  var_info->is_global = true;
  ident_node->variable_annotation = var_info;
  const auto int_lit_value = 55;
  auto int_lit_node = std::make_unique<IntegerLiteralNode>(int_lit_value);
  auto var_init = std::make_unique<VariableInitializationNode>(
      Datatype::INTEGER, std::move(ident_node), std::move(int_lit_node));

  ProgramBuilder builder;
  builder.add_global_statement(std::move(var_init));

  auto ident_node_ptr = new IdentifierNode("global_var");
  auto return_node = std::make_unique<ReturnNode>(
      std::unique_ptr<IdentifierNode>(ident_node_ptr));
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto analyzer = SemanticAnalyzer();
  analyzer.analyze_program(program.get());

  ASSERT_EQ(ident_node_ptr->variable_annotation->name, var_info->name);
  return;
  ASSERT_EQ(ident_node_ptr->variable_annotation->type, var_info->type);
  ASSERT_EQ(ident_node_ptr->variable_annotation->is_global,
            var_info->is_global);
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
  ASSERT_EQ(scope_annotation->stack_size, 8);
  for (int i{0}; i < var_names.size(); i++) {
    const auto var_name = var_names[i];
    const auto stack_offset =
        scope_annotation->variables.at(var_names[i]).get()->stack_offset;
    ASSERT_TRUE(scope_annotation->variables.contains(var_names[i]));
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
    ASSERT_EQ(ptr->scope_annotation->stack_size, 4);
  }
}

TEST(Function, DuplicateFunctionDeclaration) {
  ProgramBuilder builder;

  const std::string func_name = "duplicate_function";

  for (size_t i{0}; i < 2; i++) {
    auto func_decl = std::make_unique<FunctionDeclarationNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(func_name),
        std::vector<std::unique_ptr<ParameterNode>>{});
    builder.add_to_main(std::move(func_decl));
  }

  auto program = builder.build();
  auto analyzer = SemanticAnalyzer();
  try {
    analyzer.analyze_program(program.get());
    FAIL() << "Expected exception due to duplicate function declaration";
  } catch (const std::exception &e) {
    std::string msg = e.what();
    EXPECT_TRUE(msg.starts_with("Duplicate function declaration"));
  }
}

TEST(Function, DuplicateFunctionDefinition) {
  ProgramBuilder builder;

  const std::string func_name = "duplicate_function";

  for (size_t i{0}; i < 2; i++) {
    std::vector<std::unique_ptr<StatementNode>> body_statements;
    auto return_node =
        std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>(0));
    body_statements.push_back(std::move(return_node));
    auto func_def = std::make_unique<FunctionDefinitionNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>(func_name),
        std::vector<std::unique_ptr<ParameterNode>>{},
        std::make_unique<ScopeNode>(std::move(body_statements)));
    builder.add_to_main(std::move(func_def));
  }

  auto program = builder.build();
  auto analyzer = SemanticAnalyzer();
  try {
    analyzer.analyze_program(program.get());
    FAIL() << "Expected exception due to duplicate function definition";
  } catch (const std::exception &e) {
    std::string msg = e.what();
    EXPECT_TRUE(msg.starts_with("Duplicate function definition"));
  }
}

// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-goto,
// bugprone-unchecked-optional-access)
