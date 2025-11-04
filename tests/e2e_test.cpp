#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include "test_utils.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>

TEST(E2E, Return123) {
  std::filesystem::path compiler =
      std::filesystem::path(BUILD_DIR) / "src" / "sek";
  fmt::print("Using compiler at: {}\n", compiler.string());
  ASSERT_TRUE(std::filesystem::exists(compiler))
      << "Compiler not found at " << compiler;

  auto src = std::filesystem::path(TEST_DATA_DIR) / "return123.se";
  ASSERT_TRUE(std::filesystem::exists(src));

  auto temp_dir = std::filesystem::temp_directory_path();
  std::filesystem::path out = temp_dir / "out.tmp";

  FileGuard guard({out});

  if (std::filesystem::exists(out)) {
    std::error_code ec;
    std::filesystem::remove(out, ec);
  }

  std::string cmd =
      compiler.string() + " -o " + out.string() + " " + src.string();
  ASSERT_EQ(std::system(cmd.c_str()), EXIT_SUCCESS)
      << "Compiler failed to compile source";

  int status = std::system(out.string().c_str());
  if (WIFEXITED(status)) {
    ASSERT_EQ(WEXITSTATUS(status), 123);
  } else {
    FAIL() << "Compiled binary did not exit normally";
  }
}
