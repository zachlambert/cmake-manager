#include "config.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>


std::vector<std::string> split_line(const std::string& line) {
    std::string word;
    std::vector<std::string> words;
    std::size_t begin = 0;
    for (std::size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ') {
            if (i != begin) {
                words.push_back(line.substr(begin, i-begin));
            }
            begin = i+1;
        }
    }
    if (begin != words.size()) {
        words.push_back(line.substr(begin, words.size()-begin));
    }
    return words;
}

bool load_config(const std::string& config_dir, Config& config) {
    // Fixed/default values

    config.root_build_dir = "build";

    config.cmake_build_types = {
        {"debug", "Debug"},
        {"release", "Release"},
        {"rel_with_deb_info", "RelWithDebInfo"},
        {"min_size_rel", "MinSizeRel"},
    };
    for (const auto& [build_type, _]: config.cmake_build_types) {
        config.build_types.push_back(build_type);
    }
    config.default_build_type = "debug";

    config.build_targets.push_back("host");
    config.host_build_target = "host";
    config.host_install_prefix = "/usr";

    config.generate_compile_commands = false;
    config.copy_compile_commands = false;

    // Read values from config

    if (!std::filesystem::exists(config_dir)) {
        // Sensible defaults should be available without a config file
        return true;
    }

    std::ifstream file(config_dir);
    std::string line;
    for (std::size_t i = 0; std::getline(file, line); i++) {
        if (line.empty()) {
            continue;
        }
        auto words = split_line(line);

        auto check_word_count = [&](const std::string& key, std::size_t required) {
            if (words.size() == required + 1) {
                return true;
            }
            std::cerr
                << "Invalid config file, line " << i << ", "
                << "expected " << required << " "
                << (required == 1 ? "word" : "words") << " "
                << "following key '" << key << "'";
            return false;
        };

        if (words[0] == "root_build_dir") {
            if (!check_word_count("root_build_dir", 1)) return false;
            config.root_build_dir = words[1];
        }
        else if (words[0] == "target") {
            if (!check_word_count("target", 2)) return false;
            std::string target_name = words[1];
            BuildTarget target;
            target.toolchain = words[2];
            if (!std::filesystem::exists(target.toolchain)) {
                std::cerr
                    << "Invalid config file, line " << i << ", "
                    << "toolchain file '" << target.toolchain << "'"
                    << "for target '" << target_name << "'"
                    << "doesn't exist"
                    << std::endl;
                return false;
            }
            config.build_targets.push_back(target_name);
            config.build_target_data.emplace(target_name, target);
        }
        else if (words[0] == "default_build_type") {
            if (!check_word_count("default_build_type", 1)) return false;
            config.default_build_type = words[1];

            auto iter = std::find(config.build_types.begin(), config.build_types.end(), config.default_build_type);
            if (iter == config.build_types.end()){
                std::cerr
                    << "Invalid config file, line " << i << ", "
                    << "value '" << config.default_build_type << "'"
                    << "for 'default_build_type' is not valid, valid choices: ";
                for (std::size_t i = 0; i < config.build_types.size(); i++){
                    std::cerr << config.build_types[i];
                    if (i+1 != config.build_types.size()) {
                        std::cerr << ", ";
                    }
                }
                std::cerr << std::endl;
                return false;
            }
        }
        else if (words[0] == "host_install_prefix") {
            if (!check_word_count("host_install_prefix", 1)) return false;
            config.host_install_prefix = words[1];
            if (!std::filesystem::exists(config.host_install_prefix)) {
                std::cerr
                    << "Invalid config file, line " << i << ", "
                    << "install prefix '" << config.host_install_prefix << "'"
                    << "doesn't exist"
                    << std::endl;
                return false;
            }
        }
        else if (words[0] == "generate_compile_commands") {
            if (!check_word_count("generate_compile_commands", 1)) return false;
            if (words[1] == "true") {
                config.generate_compile_commands = true;
            } else if (words[1] == "false"){
                config.generate_compile_commands = false;
            } else {
                std::cerr
                    << "Invalid config file, line " << i << ", "
                    << "value for 'generate_compile_commands' must be "
                    << "'true' or 'false'";
            }
        }
        else if (words[0] == "copy_compile_commands") {
            if (!check_word_count("copy_compile_commands", 1)) return false;
            if (words[1] == "true") {
                config.copy_compile_commands = true;
            } else if (words[1] == "false"){
                config.copy_compile_commands = false;
            } else {
                std::cerr
                    << "Invalid config file, line " << i << ", "
                    << "value for 'copy_compile_commands' must be "
                    << "'true' or 'false'";
            }
        }
        else {
            std::cerr
                << "Invalid config file, line " << i << ", "
                << "unexpected value '" << words[0] << "'"
                << std::endl;
            return false;
        }
    }

    return true;
}
