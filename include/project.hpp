#pragma once

#include <string>
#include <optional>

struct Project {
    std::optional<std::string> previous_type;
    std::optional<std::string> previous_target;
};
