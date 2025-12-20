#include "node.h"
#include "utils.h"
#include <cstdlib>
#include <filesystem>
#include <string>

std::string read_test_data(const std::string &filename) {
  std::string path = std::string(TEST_DATA_DIR) + "/" + filename;
  return read_file(path);
}

class ProgramBuilder {
public:
  ProgramBuilder() {
    global_scope = std::make_unique<ScopeNode>(
        std::vector<std::unique_ptr<StatementNode>>{});

    auto main_body = std::make_unique<ScopeNode>(
        std::vector<std::unique_ptr<StatementNode>>{});

    main_function = std::make_unique<FunctionDefinitionNode>(
        Datatype::INTEGER, std::make_unique<IdentifierNode>("huvud"),
        std::vector<std::unique_ptr<ParameterNode>>{}, std::move(main_body));

    std::string return_label = ".exit_huvud";
    auto scope_info = ScopeInfo(ScopeType::Function);
    scope_info.return_label = return_label;
    main_function->body->scope_annotation = scope_info;
  }

  void add_global_statement(std::unique_ptr<StatementNode> stmt) {
    global_scope->statements.push_back(std::move(stmt));
  }

  void add_to_main(std::unique_ptr<StatementNode> stmt) {
    auto main_body_ptr = static_cast<ScopeNode *>(main_function->body.get());
    main_body_ptr->statements.push_back(std::move(stmt));
  }

  void set_main_scope_stack_size(int stack_size) {
    main_function->body->scope_annotation->stack_size = stack_size;
  }

  std::unique_ptr<ScopeNode> build() {
    add_global_statement(std::move(main_function));
    return std::move(global_scope);
  }

  std::string get_main_return_label() const {
    if (!main_function->body->scope_annotation.has_value()) {
      throw std::runtime_error("Main function scope not annotated");
    }
    return main_function->body->scope_annotation->return_label;
  }

private:
  std::unique_ptr<ScopeNode> global_scope;
  std::unique_ptr<FunctionDefinitionNode> main_function;
};

int run_assembly(const std::string &asm_code) {
  auto temp_dir = std::filesystem::temp_directory_path();
  auto asm_file = temp_dir / "temp_assembly.s";
  auto obj_file = temp_dir / "temp_assembly.o";
  auto bin_file = temp_dir / "temp_binary";

  // Ensure temp files are removed on scope exit

  FileGuard guard({asm_file, obj_file, bin_file});

  write_file(asm_file, asm_code);

  // Assemble
  if (std::system(("as --64 -o " + obj_file.string() + " " + asm_file.string())
                      .c_str()) != 0)
    throw std::runtime_error("Assembly failed");

  // Link
  if (std::system(
          ("ld -o " + bin_file.string() + " " + obj_file.string()).c_str()) !=
      0)
    throw std::runtime_error("Linking failed");

  int status = std::system(bin_file.string().c_str());
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  } else {
    throw std::runtime_error("Process did not exit normally");
  }
}
