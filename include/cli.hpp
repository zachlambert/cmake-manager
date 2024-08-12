#pragma once

#include <argparse.hpp>
#include "config.hpp"
#include "exec.hpp"


// TODO: Move to source
inline void parser_add(
    argparse::Parser& parser,
    const Config* config,
    BuildArgs& args)
{
    parser.add(args.type, "--type")
        .default_value(config->default_build_type)
        .choices(config->build_types)
        .help("Build type");
    parser.add(args.target, "--target")
        .default_value(config->host_build_target)
        .choices(config->build_targets)
        .help("Build target");
}

struct CliBuild: public argparse::Args {
    const Config* config;
    BuildArgs build_args;

    CliBuild(const Config* config):
        config(config)
    {}
private:
    void build(argparse::Parser& parser){
        parser_add(parser, config, build_args);
    }
};

struct CliInstall: public argparse::Args {
    const Config* config;
    BuildArgs build_args;

    CliInstall(const Config* config):
        config(config)
    {}
private:
    void build(argparse::Parser& parser){
        parser_add(parser, config, build_args);
    }
};

struct CliMain: public argparse::Args {
    const Config* config;
    std::variant<CliBuild, CliInstall> command;

    CliMain(const Config* config):
        config(config),
        command(CliBuild(config))
    {}
public:
    void build(argparse::Parser& parser) {
        parser.subcommand(command)
            .add<CliBuild>("build", "Build the project", config)
            .add<CliInstall>("install", "Install the project", config);
    };
};
