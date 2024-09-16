#include <memory>
#include <filesystem>
#include <pwd.h>
#include "config.hpp"
#include "cli.hpp"
#include "exec.hpp"


std::string exec(const std::string& cmd, bool print=true) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        if (print) {
            std::cout << result;
        }
        result += buffer.data();
    }
    return result;
}

int main(int argc, const char** argv) {
    std::string config_home;
    if (const char* var = getenv("XDG_CONFIG_HOME")) {
        config_home = var;
    } else if (const char* var = getenv("HOME")) {
        config_home = var;
    } else {
        // https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
        config_home = getpwuid(getuid())->pw_dir;
    }
    const std::string config_dir = config_home + "/.cmake-manager";

    Config config;
    if (!load_config(config_dir + "/config.txt", config)) {
        return 1;
    }

    CliMain cli(&config);
    if (!argparse::parse(argc, argv, cli)) {
        return 1;
    }

    if (auto command = std::get_if<CliBuild>(&cli.command)) {
        if (!exec_build(config, command->build_args)) {
            return 1;
        }
    }
    else if (auto command = std::get_if<CliInstall>(&cli.command)) {
        if (!exec_install(config, command->build_args)) {
            return 1;
        }
    }
    else if (auto command = std::get_if<CliTest>(&cli.command)) {
        if (!exec_test(config, command->build_args)) {
            return 1;
        }
    }
    else {
        assert(false);
        std::cerr << "Unsupported command" << std::endl;
        return 1;
    }
    return 0;
}
