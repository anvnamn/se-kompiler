#include "codegen.h"
#include "test_utils.h"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>

TEST(CodegenTest, PlainReturn) {
  constexpr int return_value = 123;
  auto return_node = std::make_unique<ReturnNode>(
      std::make_unique<IntegerLiteralNode>(return_value));

  ProgramBuilder builder;
  builder.add_to_main(std::move(return_node));
  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  auto const expected_as = read_test_data("return123.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, return_value);
}

TEST(CodegenTest, GlobalVariableDeclaration) {
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
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  const auto expected_as = read_test_data("global_var_decl.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, 0);
}

TEST(CodegenTest, GlobalVariableInitialization) {
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
  builder.add_to_main(std::move(return_node));

  auto program = builder.build();

  auto codegen = Codegen();
  const auto actual_as = codegen.generate_assembly(*program);

  const auto expected_as = read_test_data("global_var_def.as");

  ASSERT_EQ(actual_as, expected_as);

  const auto return_status = run_assembly(actual_as);
  ASSERT_EQ(return_status, 0);
}
