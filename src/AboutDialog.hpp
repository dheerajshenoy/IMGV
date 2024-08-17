#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QString>

const std::string APP_VERSION = "1.3";

class AboutDialog : public QDialog
{

public:
    AboutDialog(QWidget *parent = nullptr);

};

#endif
