#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtGui/QDesktopServices>
#include <qt6/QtCore/QString>

const std::string APP_VERSION = "1.0";

class AboutDialog : public QDialog
{

public:
    AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

};

#endif
