#ifndef IMAGE_PROPERTIES_DIALOG_HPP
#define IMAGE_PROPERTIES_DIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QGridLayout>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QMessageBox>
#include <qt6/QtWidgets/QLabel>
#include "utils.hpp"
#include "exif.hpp"

class ImagePropertiesDialog : public QDialog
{

public:
    ImagePropertiesDialog(QString filename, QWidget *parent = nullptr);
    ~ImagePropertiesDialog() {}



private:

    void showEXIFProperties(const QString filename) noexcept;
};

#endif
