#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"

#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <string>

int main(int argc, char **argv) {
  try {
    cxxopts::Options options("sek", "SE kompiler");

    options.add_options()("i,input", "Input source file",
                          cxxopts::value<std::string>())(
        "o,output", "Output file",
        cxxopts::value<std::string>()->default_value("out"))("h,help",
                                                             "Print usage");
    cxxopts::ParseResult result;

    try {
      options.parse_positional({"input"});
      result = options.parse(argc, argv);

    } catch (const std::exception &e) {
      fmt::print(stderr, "Error parsing options: {}\n", e.what());
      return 1;
    }

    if (result.count("help")) {
      fmt::print("{}\n", options.help());
      return 0;
    }

    if (!result.count("input")) {
      fmt::print(stderr, "Please provide an input file.\n");
      return 1;
    }

    std::filesystem::path input_file = result["input"].as<std::string>();
    std::filesystem::path output_file = result["output"].as<std::string>();

    std::string content;
    {
      std::ifstream ifs(input_file);
      if (!ifs) {
        fmt::print(stderr, "Failed to open {}.\n", input_file.string());
        return 1;
      }
      content.assign((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());
    }

    std::string asm_code;

    try {
      TokenStream ts = tokenize(content);

      auto ast = parse_tokens(ts);

      SemanticAnalyzer semantic_analyzer;

      semantic_analyzer.analyze_program(&ast);

      Codegen codegen;
      asm_code = codegen.generate_assembly(ast);
    } catch (const std::exception &e) {
      fmt::print(stderr, "Compilation error: {}\n", e.what());
      return 1;
    }

    fmt::print("{}\n", asm_code);
    std::filesystem::path temp_dir;
    try {
      temp_dir = std::filesystem::temp_directory_path();
    } catch (const std::exception &e) {
      fmt::print(stderr, "Failed to get temp directory: {}\n", e.what());
      return 1;
    }
    auto temp_asm = temp_dir / "temp_assembly.s";
    auto temp_obj = temp_dir / "temp_assembly.o";
    auto temp_bin = temp_dir / "temp_binary";

    {
      std::ofstream asm_osm(temp_asm);

      if (asm_osm.is_open()) {
        asm_osm << asm_code;
      } else {
        fmt::print(stderr, "Failed to write assembly to temp file.\n");
        return 1;
      }
    }

    std::string asm_cmd =
        fmt::format("as -o {} {}", temp_obj.string(), temp_asm.string());
    if (std::system(asm_cmd.c_str()) != 0) {
      fmt::print(stderr, "Assembly failed.\n");
      return 1;
    }

    std::string link_cmd =
        fmt::format("ld -o {} {}", temp_bin.string(), temp_obj.string());
    if (std::system(link_cmd.c_str()) != 0) {
      fmt::print(stderr, "Linking failed.\n");
      return 1;
    }

    std::filesystem::rename(temp_bin, output_file);

    std::filesystem::remove(temp_asm);
    std::filesystem::remove(temp_obj);
    std::filesystem::remove(temp_bin);

    return EXIT_SUCCESS;

  } catch (const std::exception &e) {
    fmt::print(stderr, "Unexpected error: {}\n", e.what());
    return 1;
  }
}