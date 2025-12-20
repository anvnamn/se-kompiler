#include "codegen.h"
#include "semantic_analyzer.h"
#include "test_utils.h"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>
#include <regex>

TEST(Codegen, ReturnIntegerLiteral) {
  ProgramBuilder builder;

  constexpr int return_value = 123;
  auto return_node = std::make_unique<ReturnNode>(
      std::make_unique<IntegerLiteralNode>(return_value));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));
  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  auto const expected_as = read_test_data("return123.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, return_value);
}

TEST(Codegen, ReturnGlobalVariable) {
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

  auto ident_node_return = std::make_unique<IdentifierNode>("global_var");
  ident_node_return->variable_annotation = var_info;
  auto return_node = std::make_unique<ReturnNode>(std::move(ident_node_return));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  const auto expected_as = read_test_data("return_global_var.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, int_lit_value);
}

TEST(Codegen, ReturnLocalVariable) {
  auto ident_node = std::make_unique<IdentifierNode>("local_var");

  auto var_info = std::make_shared<VariableInfo>();
  var_info->name = "local_var";
  var_info->type = Datatype::INTEGER;
  var_info->is_global = false;
  var_info->stack_offset = -4; // Top of stack
  ident_node->variable_annotation = var_info;
  const auto int_lit_value = 77;
  auto int_lit_node = std::make_unique<IntegerLiteralNode>(int_lit_value);
  auto var_init = std::make_unique<VariableInitializationNode>(
      Datatype::INTEGER, std::move(ident_node), std::move(int_lit_node));

  ProgramBuilder builder;
  builder.add_to_main(std::move(var_init));

  auto ident_node_return = std::make_unique<IdentifierNode>("local_var");
  ident_node_return->variable_annotation = var_info;
  auto return_node = std::make_unique<ReturnNode>(std::move(ident_node_return));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto generated_assembly = codegen.generate_assembly(*program);

  const auto return_status = run_assembly(generated_assembly);
  ASSERT_EQ(return_status, int_lit_value);
}

TEST(Codegen, GlobalVariableDeclaration) {
  auto ident_node = std::make_unique<IdentifierNode>("global_var");
  auto var_info = std::make_shared<VariableInfo>();
  var_info->name = "global_var";
  var_info->type = Datatype::INTEGER;
  var_info->is_global = true;
  ident_node->variable_annotation = var_info;
  auto var_decl = std::make_unique<VariableDeclarationNode>(
      Datatype::INTEGER, std::move(ident_node));

  ProgramBuilder builder;
  builder.add_global_statement(std::move(var_decl));

  auto return_node =
      std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>(0));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  fmt::print("Generated assembly:\n{}", actual_as);

  const auto expected_as = read_test_data("global_var_decl.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, 0);
}

TEST(Codegen, GlobalVariableInitialization) {
  auto ident_node = std::make_unique<IdentifierNode>("global_var");
  auto var_info = std::make_shared<VariableInfo>();
  var_info->name = "global_var";
  var_info->type = Datatype::INTEGER;
  var_info->is_global = true;
  ident_node->variable_annotation = var_info;
  const auto int_lit_value = 1621;
  auto int_lit_node = std::make_unique<IntegerLiteralNode>(int_lit_value);
  auto var_decl = std::make_unique<VariableInitializationNode>(
      Datatype::INTEGER, std::move(ident_node), std::move(int_lit_node));

  ProgramBuilder builder;
  builder.add_global_statement(std::move(var_decl));

  auto return_node =
      std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>(0));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  const auto expected_as = read_test_data("global_var_def.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, 0);
}

TEST(Codegen, AssignIntegerVar) {
  const std::string var_name = "assigned_var";
  constexpr int init_value = 33;
  constexpr int assigned_value = 44;
  auto ident_node = std::make_unique<IdentifierNode>(var_name);

  auto var_info = std::make_shared<VariableInfo>();
  var_info->name = var_name;
  var_info->type = Datatype::INTEGER;
  var_info->stack_offset = -4; // Integer at top of stack
  ident_node->variable_annotation = var_info;
  const auto int_lit_value = init_value;
  auto int_lit_node = std::make_unique<IntegerLiteralNode>(int_lit_value);
  auto var_init = std::make_unique<VariableInitializationNode>(
      Datatype::INTEGER, std::move(ident_node), std::move(int_lit_node));

  ProgramBuilder builder;
  builder.add_to_main(std::move(var_init));

  auto ident_node_ptr = new IdentifierNode(var_name);
  ident_node_ptr->variable_annotation = var_info;
  auto assignment_node = std::make_unique<AssignmentNode>(
      std::unique_ptr<IdentifierNode>(ident_node_ptr),
      std::make_unique<IntegerLiteralNode>(assigned_value));
  builder.add_to_main(std::move(assignment_node));

  auto return_ident_node_ptr = new IdentifierNode(var_name);
  return_ident_node_ptr->variable_annotation = var_info;
  auto return_node = std::make_unique<ReturnNode>(
      std::unique_ptr<IdentifierNode>(return_ident_node_ptr));
  return_node->return_label = builder.get_main_return_label();
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();
  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);
  fmt::print("Generated assembly:\n{}", actual_as);
  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, assigned_value);
}

class StackSizeParams : public ::testing::TestWithParam<std::tuple<int, int>> {
};

TEST_P(StackSizeParams, StackSize) {

  int size_annotation = std::get<0>(GetParam());
  int expected_code_size = std::get<1>(GetParam());

  fmt::print("Testing stack size annotation: {}, expecting codegen size: {}\n",
             size_annotation, expected_code_size);

  ProgramBuilder builder;
  builder.set_main_scope_stack_size(size_annotation);
  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);
  fmt::print("Generated assembly:\n{}", actual_as);
  // use regex to find stack size allocation
  std::regex stack_size_regex(R"(\s*sub\s+\$(\d+),\s+\%rsp)");
  std::smatch match;
  if (std::regex_search(actual_as, match, stack_size_regex)) {
    const auto actual_code_size = std::stoi(match[1].str());
    ASSERT_EQ(actual_code_size, expected_code_size);
  } else {
    if (expected_code_size == 0) {
      // No stack allocation expected, test passes
      return;
    }
    FAIL() << "No stack size allocation found in assembly";
  }
}

INSTANTIATE_TEST_SUITE_P(
    Codegen, StackSizeParams,
    ::testing::Values(std::make_tuple(0, 0), std::make_tuple(4, 16),
                      std::make_tuple(8, 16), std::make_tuple(13, 16),
                      std::make_tuple(16, 16), std::make_tuple(24, 32)));
