#include "AboutDialog.hpp"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowIcon(QIcon(":/icons/imgv.png"));
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel *label_name = new QLabel("IMGV");
    QLabel *label_desc = new QLabel("Image Viewer no one asked for");
    QLabel *label_git_link = new QLabel("Check out this project at <a href='https://github.com/dheerajshenoy/IMGV'>link</a>");
    QLabel *label_version = new QLabel(QString("Version %1").arg(QString::fromStdString(APP_VERSION)));
    QPushButton *done_btn = new QPushButton("Done");

    label_git_link->setTextFormat(Qt::RichText);
    label_git_link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label_git_link->setOpenExternalLinks(true);

    connect(done_btn, &QPushButton::clicked, this, [&]() {
        this->close();
    });

    layout->addWidget(label_name);
    layout->addWidget(label_desc);
    layout->addWidget(label_version);
    layout->addWidget(label_git_link);
    layout->addWidget(done_btn);

    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint);

    QFont label_name_font, label_desc_font;

    label_desc_font.setItalic(true);
    label_desc_font.setPointSize(14);
    label_desc->setFont(label_desc_font);

    label_name_font.setPointSize(18);
    label_name_font.setBold(true);
    label_name->setFont(label_name_font);

    this->setMinimumSize(QSize(400, 150));
    this->setMaximumSize(QSize(400, 150));
    this->setFixedSize(QSize(400, 150));

    this->setLayout(layout);
}

AboutDialog::~AboutDialog()
{}
