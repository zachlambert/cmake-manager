#pragma once

#include <argparse.hpp>
#include "config.hpp"
#include "exec.hpp"


void parser_add(
    argparse::Parser& parser,
    const Config* config,
    BuildArgs& args);

struct CliBuild: public argparse::Args {
    const Config* config;
    BuildArgs build_args;

    CliBuild(const Config* config):
        config(config)
    {}
private:
    void build(argparse::Parser& parser);
};

struct CliInstall: public argparse::Args {
    const Config* config;
    BuildArgs build_args;

    CliInstall(const Config* config):
        config(config)
    {}
private:
    void build(argparse::Parser& parser);
};

struct CliTest: public argparse::Args {
    const Config* config;
    BuildArgs build_args;

    CliTest(const Config* config):
        config(config)
    {}
private:
    void build(argparse::Parser& parser);
};

struct CliMain: public argparse::Args {
    const Config* config;
    std::variant<CliBuild, CliInstall, CliTest> command;

    CliMain(const Config* config):
        config(config),
        command(CliBuild(config))
    {}
public:
    void build(argparse::Parser& parser);
};
