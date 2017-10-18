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
//    cvReleaseCapture(capture);
    delete ui;
}

void image_process::on_btn_work_with_image_pressed()
{
    cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("gray", CV_WINDOW_AUTOSIZE);

    while (1)
        {
        capture>>original;
        cv::imshow("original",original);
        char c = cvWaitKey(33);

        if (c==27)
            break;
        }
    destroyWindow("original");
    destroyWindow("gray");
}
