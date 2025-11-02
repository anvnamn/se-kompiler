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
