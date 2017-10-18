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

    while (ui->chk_capture_image->isChecked())
        {
        slot_get_and_calc_image();
        char c = cvWaitKey(33);
        {//Показываем
        imshow("original",original);
        imshow("gray", gray);
        }
        if (c==27)
            break;

        }
    destroyWindow("original");
    destroyWindow("gray");
}

void image_process::slot_get_and_calc_image()
{
    capture>>original;
    cvtColor(original, gray, CV_RGB2GRAY);
    cv::blur(gray,gray,cv::Size(3,3),cv::Point(-1,-1));

    float integral_gray = integral_intensity();
    if (integral_gray>170)
       threshold(gray, gray,60,255,CV_THRESH_BINARY);
    else if (integral_gray>100)
       threshold(gray, gray,45,255,CV_THRESH_BINARY);
    else
       threshold(gray, gray,25,255,CV_THRESH_BINARY);


}



void image_process::on_chk_capture_image_stateChanged(int arg1)
{
    if (arg1)
        {
        slot_cycle_get_images();
        }
}

float image_process::integral_intensity()
{
    long double itt = 0;
    for (int x = 0; x<gray.rows;++x)
        for (int y = 0; y<gray.cols;++y)
        itt+=gray.at<unsigned char>(x,y);
    return itt/(gray.cols*gray.rows);
}
