#include "imgv.hpp"

int main (int argc, char *argv[]) {
    QApplication a(argc, argv);
    IMGV *imgv = new IMGV();
    a.exec();
}
