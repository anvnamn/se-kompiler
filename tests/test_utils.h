#include <fstream>

std::string read_file(const std::string &filename) {
  std::string path = std::string(TEST_DATA_DIR) + "/" + filename;
  std::ifstream ifs(path);
  if (!ifs)
    throw std::runtime_error("Failed to open file: " + path);

  return std::string(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());
}