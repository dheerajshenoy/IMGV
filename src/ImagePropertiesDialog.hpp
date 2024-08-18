#ifndef IMAGE_PROPERTIES_DIALOG_HPP
#define IMAGE_PROPERTIES_DIALOG_HPP

#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QFileDialog>
#include "utils.hpp"
#include <Magick++.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

class ImagePropertiesDialog : public QDialog
{

public:
    ImagePropertiesDialog(const QString& filename, QWidget *parent = nullptr);

private:

    void showEXIFProperties(const QString& filename) noexcept;
};

#endif
