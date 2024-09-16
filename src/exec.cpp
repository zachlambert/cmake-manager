#include "exec.hpp"
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>


bool exec(const std::string& cmd) {
    std::array<char, 128> buffer;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (!feof(pipe)) {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            std::cout << buffer.data();
        }
    }
    int rc = pclose(pipe);
    return rc == 0;
}

bool exec_build(const Config& config, const BuildArgs& args) {
    std::string build_dir =
        config.root_build_dir
        + "/" + args.target
        + "/" + args.type;

    std::string cmake_build_type = config.cmake_build_types.at(args.type);
    std::filesystem::create_directories(build_dir);

    std::vector<std::string> options;
    options.push_back("-DCMAKE_BUILD_TYPE=" + cmake_build_type);
    if (config.generate_compile_commands) {
        options.push_back("-DCMAKE_EXPORT_COMPILE_COMMANDS=1");
    }
    if (args.target == config.host_build_target) {
        options.push_back("-DCMAKE_INSTALL_PREFIX=" + config.host_install_prefix);
    } else {
        const auto& data = config.build_target_data.at(args.target);
        options.push_back("-DCMAKE_TOOLCHAIN_FILE=" + data.toolchain);
    }

    std::string cmake_command = "cmake -E chdir " + build_dir + " cmake";
    for (const auto& option: options) {
        cmake_command += " " + option;
    }
    cmake_command += " ../../..";
    std::cout << cmake_command << std::endl;

    bool success = exec(cmake_command)
        && exec("cmake --build " + build_dir);

    if (!success) {
        return false;
    }
    if (config.generate_compile_commands && config.copy_compile_commands) {
        std::ifstream in(build_dir + "/compile_commands.json");
        std::ofstream out("compile_commands.json");
        out << in.rdbuf();
    }

    return true;
}

bool exec_install(const Config& config, const BuildArgs& args) {
    std::string build_dir =
        config.root_build_dir
        + "/" + args.target
        + "/" + args.type;
    std::string install_command = "sudo cmake --build " + build_dir + " --target install";
    return exec(install_command);
}

bool exec_test(const Config& config, const BuildArgs& args) {
    std::string build_dir =
        config.root_build_dir
        + "/" + args.target
        + "/" + args.type;
    std::string install_command = "cd " + build_dir + " && ctest --output-on-failure";
    return exec(install_command);
}
