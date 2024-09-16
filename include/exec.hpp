#pragma once

#include <string>
#include "config.hpp"


struct BuildArgs {
    std::string type;
    std::string target;
};

bool exec_build(const Config& config, const BuildArgs& args);
bool exec_install(const Config& config, const BuildArgs& args);
bool exec_test(const Config& config, const BuildArgs& args);
