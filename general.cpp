#include "general.h"
#include "ui_general.h"

general::general(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::general)
{
    ui->setupUi(this);
    image = new image_process(this);
    ui->layout_image->addWidget(image);
}

general::~general()
{
    delete ui;
}
