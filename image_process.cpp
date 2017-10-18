#include "image_process.h"
#include "ui_image_process.h"

using namespace std;
using namespace cv;



image_process::image_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_process)
{
    ui->setupUi(this);
    capture = VideoCapture(1);
}

image_process::~image_process()
{
    delete ui;
}
