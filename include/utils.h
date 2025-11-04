#pragma once
#include <chrono>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <random>
#include <stdexcept>
#include <system_error>
#include <vector>

struct FileGuard {

  std::vector<std::filesystem::path> paths;

  FileGuard(std::vector<std::filesystem::path> paths) : paths(paths) {}

  ~FileGuard() {
    std::error_code ec;
    for (auto path : paths) {
      std::filesystem::remove(path, ec);
    }
  }

  // Delete copy and move operations
  FileGuard(const FileGuard &) = delete;
  FileGuard &operator=(const FileGuard &) = delete;
  FileGuard(FileGuard &&) = delete;
  FileGuard &operator=(FileGuard &&) = delete;
};

inline std::string read_file(const std::filesystem::path &path) {
  std::ifstream ifs(path);
  if (!ifs)
    throw std::runtime_error(
        fmt::format("Failed to open file: {}", path.string()));

  return std::string(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());
}

inline void write_file(const std::filesystem::path &path,
                       const std::string &data) {
  if (path.has_parent_path() && !std::filesystem::exists(path.parent_path())) {
    throw std::runtime_error(
        fmt::format("Can't write to file, parent directory does not exist: {}",
                    path.parent_path().string()));
  }

  std::filesystem::path tmp_dir = std::filesystem::temp_directory_path();
  auto tmp_path = tmp_dir / "temp_write_file";

  // Ensure the temp file is deleted on scope exit
  FileGuard guard({tmp_path});

  {
    std::ofstream ofs(tmp_path);
    if (!ofs) {
      throw std::runtime_error(fmt::format(
          "Failed to open temporary file for writing: {}", tmp_path.string()));
    }
    ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    if (!ofs) {
      throw std::runtime_error(fmt::format(
          "Failed while writing to temporary file: {}", tmp_path.string()));
    }
  }

  // Rename into place
  std::error_code ec;
  std::filesystem::rename(tmp_path, path, ec);
  if (ec) {
    throw std::runtime_error(fmt::format(
        "Failed to rename temporary file into place: {}", ec.message()));
  }
}