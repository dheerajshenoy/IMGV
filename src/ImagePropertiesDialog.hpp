#ifndef IMAGE_PROPERTIES_DIALOG_HPP
#define IMAGE_PROPERTIES_DIALOG_HPP

#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include "utils.hpp"
#include "exif.hpp"

class ImagePropertiesDialog : public QDialog
{

public:
    ImagePropertiesDialog(const QString& filename, QWidget *parent = nullptr);

private:

    void showEXIFProperties(const QString& filename) noexcept;
};

#endif
