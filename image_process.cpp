#include "image_process.h"
#include "ui_image_process.h"

image_process::image_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_process)
{
    ui->setupUi(this);
}

image_process::~image_process()
{
    delete ui;
}
