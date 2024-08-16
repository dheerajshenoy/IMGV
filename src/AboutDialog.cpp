#include "AboutDialog.hpp"
#include <qfontdatabase.h>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{

    int id = QFontDatabase::addApplicationFont(":/resources/fonts/ShureTechMono.ttf");

    QString shure_tech_font_family = QFontDatabase::applicationFontFamilies(id).at(0);
    setWindowIcon(QIcon(":/resources/images/logo.svg"));
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel *label_name = new QLabel("IMGV");
    QLabel *label_img = new QLabel();
    label_img->setPixmap(QPixmap(":/resources/images/logo.svg"));
    QLabel *label_desc = new QLabel("Image Viewer no one asked for");
    QLabel *label_git_link = new QLabel("Check out this project at <a href='https://github.com/dheerajshenoy/IMGV'>link</a>");
    QLabel *label_version = new QLabel(QString("Version %1").arg(QString::fromStdString(APP_VERSION)));
    QPushButton *done_btn = new QPushButton("Done");

    QHBoxLayout *img_layout = new QHBoxLayout();

    img_layout->addWidget(label_name);
    img_layout->addWidget(label_img);
    img_layout->addStretch(1);

    label_git_link->setTextFormat(Qt::RichText);
    label_git_link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label_git_link->setOpenExternalLinks(true);

    connect(done_btn, &QPushButton::clicked, this, [&]() {
        this->close();
    });

    label_img->setScaledContents(true);
    label_img->setBaseSize(512, 512);
    layout->addLayout(img_layout);
    layout->addWidget(label_desc);
    layout->addWidget(label_version);
    layout->addWidget(label_git_link);
    layout->addWidget(done_btn);

    label_img->setFixedSize(64, 64);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint);

    QFont label_name_font, label_desc_font;

    label_desc_font.setItalic(true);
    label_desc_font.setPointSize(14);
    label_desc->setFont(label_desc_font);

    label_name_font.setFamily(shure_tech_font_family);
    label_name_font.setPointSize(30);
    label_name_font.setBold(true);
    label_name->setFont(label_name_font);

    this->setFixedSize(300, 200);
    this->setLayout(layout);
}
