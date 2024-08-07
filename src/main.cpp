#include "imgv.hpp"

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    argparse::ArgumentParser parser("IMGV", APP_VERSION);

    parser.add_description("Image viewer world never asked for");

    parser.add_argument("-i", "--input")
        .help("Open Image files(s)")
        .nargs(argparse::nargs_pattern::at_least_one)
        .append();

    parser.add_argument("-s", "--session")
        .help("Open session file with filename or id")
        .nargs(argparse::nargs_pattern::at_least_one)
        .append();

    parser.add_argument("-n", "--no-config")
        .help("Load without any configuration")
        .flag();

    parser.add_argument("-l", "--list-sessions")
        .help("List all the session files")
        .flag();

    parser.add_argument("files")
        .remaining();

    parser.add_epilog("For more information about the program usage please check the project page at https://github.com/dheerajshenoy/IMGV");


    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    
    IMGV *imgv = new IMGV(parser);
    return app.exec();
}
