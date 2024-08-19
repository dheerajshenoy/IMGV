#include "imgv.hpp"
#include <string>

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    argparse::ArgumentParser parser("imgv", APP_VERSION);

    parser.add_description("Image viewer no one asked for.\n\n");

    parser.add_argument("-s", "--session")
        .help("Open session file providing name of the session or it's path")
        .nargs(1)
        .metavar("session-name/session-path");

    parser.add_argument("-i", "--input")
        .help("Open Image files(s)")
        .nargs(argparse::nargs_pattern::at_least_one)
        .metavar("files")
        .append();

    parser.add_argument("-S", "--new-session")
        .help("Open in new session")
        .nargs(1)
        .metavar("session-name");

    parser.add_argument("-n", "--no-config")
        .help("Load without any lua configuration")
        .flag();

    parser.add_argument("-l", "--list-sessions")
        .help("List all the session files")
        .flag();

    parser.add_argument("-")
        .help("Read from STDIN")
        .flag();

    parser.add_argument("files")
        .remaining();

    parser.add_epilog("For more information about the program usage and for the list of all supported image formats please check the project page at https://github.com/dheerajshenoy/IMGV");


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
