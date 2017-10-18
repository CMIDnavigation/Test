#include "image_process.h"
#include "ui_image_process.h"

using namespace std;
using namespace cv;



image_process::image_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_process)
{
    ui->setupUi(this);
    capture = VideoCapture(CV_CAP_ANY);
}

image_process::~image_process()
{
    capture.release();
    delete ui;
}


void image_process::slot_cycle_get_images()
{
    cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);

    while (1)
        {
        capture>>original;
        imshow("original",original);
        cvtColor(original, gray, CV_RGB2GRAY);
        char c = cvWaitKey(33);


        imshow("gray", gray);
        if (c==27)
            break;
        }
    destroyWindow("original");
    destroyWindow("gray");
}
