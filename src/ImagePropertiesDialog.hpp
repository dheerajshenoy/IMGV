#ifndef IMAGE_PROPERTIES_DIALOG_HPP
#define IMAGE_PROPERTIES_DIALOG_HPP

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
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
