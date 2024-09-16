#include "cli.hpp"

void parser_add(
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

void CliBuild::build(argparse::Parser& parser){
    parser_add(parser, config, build_args);
}

void CliInstall::build(argparse::Parser& parser){
    parser_add(parser, config, build_args);
}

void CliTest::build(argparse::Parser& parser){
    parser_add(parser, config, build_args);
}

void CliMain::build(argparse::Parser& parser) {
    parser.subcommand(command)
        .add<CliBuild>("build", "Build the project", config)
        .add<CliInstall>("install", "Install the project", config)
        .add<CliTest>("test", "Run tests", config);
}
