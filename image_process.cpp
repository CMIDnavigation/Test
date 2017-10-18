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

    vector<vector<Point>> contours;
    vector<cv::Vec4i> hierarchy;

   Canny( gray, gray, 1, 3, 3 );
   findContours( gray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));

   int num_max = 0; double s_max = 0;
   for (int i = 0; i<contours.size();++i)
        {
        Size2f size_now = minAreaRect(contours[i]).size;
        if (size_now.height*size_now.width>s_max)
            {
            num_max = i;
            s_max = size_now.height*size_now.width;
            }
        }

    if (!contours.empty())
        {
        RotatedRect rect = cv::minAreaRect(contours[num_max]);
        Point2f vertices2f[4];
        rect.points(vertices2f);
        line(original,Point(vertices2f[0].x,vertices2f[0].y),Point(vertices2f[1].x,vertices2f[1].y),
            cvScalar(0,0,255));
        line(original,Point(vertices2f[2].x,vertices2f[2].y),Point(vertices2f[1].x,vertices2f[1].y),
            cvScalar(0,0,255));
        line(original,Point(vertices2f[3].x,vertices2f[3].y),Point(vertices2f[2].x,vertices2f[2].y),
            cvScalar(0,0,255));
        line(original,Point(vertices2f[0].x,vertices2f[0].y),Point(vertices2f[3].x,vertices2f[3].y),
            cvScalar(0,0,255));
        }
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


