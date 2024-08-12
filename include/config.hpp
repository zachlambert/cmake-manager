#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct BuildTarget {
    std::string toolchain;
};

struct Config {
    std::string root_build_dir;

    std::vector<std::string> build_types;
    std::unordered_map<std::string, std::string> cmake_build_types;
    std::string default_build_type;

    std::vector<std::string> build_targets;
    std::string host_build_target;
    std::unordered_map<std::string, BuildTarget> build_target_data;
    std::string host_install_prefix;
};

[[nodiscard]]
bool load_config(const std::string& config_dir, Config& config);
