#include "exec.hpp"
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <filesystem>


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
    options.push_back("-DCMAKE_EXPORT_COMPILE_COMMANDS=1");
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
    return
        exec(cmake_command)
        && exec("cmake --build " + build_dir);
}

bool exec_install(const Config& config, const BuildArgs& args) {
    std::string build_dir =
        config.root_build_dir
        + "/" + args.target
        + "/" + args.type;
    std::string install_command = "sudo cmake --build " + build_dir + " --target install";
    return exec(install_command);
}
