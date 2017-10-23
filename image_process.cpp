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
    cvNamedWindow("Canny", CV_WINDOW_AUTOSIZE);

    while (ui->chk_capture_image->isChecked())
    {
        slot_get_and_calc_image();
        {
            //Показываем
            imshow("original",original);
            imshow("gray", gray);
            imshow("Canny", ufter_plus);
        }

        char c = cvWaitKey(33);

        if (c==27)
            break;

    }
    ui->chk_capture_image->setChecked(false);
    destroyWindow("original");
    destroyWindow("gray");
    destroyWindow("Canny");
}

void image_process::slot_get_and_calc_image()
{
    capture>>original;
    cvtColor(original, gray, CV_RGB2GRAY);
    ufter_plus = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));
    cv::blur(gray,gray,cv::Size(3,3),cv::Point(-1,-1));

    float integral_gray = integral_intensity(gray);
    if (integral_gray>170)
       threshold(gray, ufter_plus,60,255,CV_THRESH_BINARY);
    else if (integral_gray>100)
       threshold(gray, ufter_plus,45,255,CV_THRESH_BINARY);
    else
       threshold(gray, ufter_plus,25,255,CV_THRESH_BINARY);

    vector<vector<Point>> contours;
    vector<cv::Vec4i> hierarchy;

   Canny( ufter_plus, gray, 1, 3, 3 );

   findContours( gray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));

   int num_max = 0; double s_max = 0;
   for (unsigned int i = 0; i<contours.size();++i)
        {
        Size2f size_now = minAreaRect(contours[i]).size;
        if (size_now.height*size_now.width>s_max)
            {
            num_max = i;
            s_max = size_now.height*size_now.width;
            }
        }

    if (contours.empty())
        return;

    RotatedRect rect = cv::minAreaRect(contours[num_max]);
    Point2f point_rect[4]; rect.points(point_rect);

    cv::Point point_rect_to_draw[4];

    for(int i = 0; i < 4; ++i)
        {
        point_rect_to_draw[i] = point_rect[i];
        }

    gray = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));
    fillConvexPoly(gray, point_rect_to_draw, 4, Scalar(255,255,255));

    gray^=(~ufter_plus);

    double intensivity = integral_intensity(gray);



    if (intensivity<1)//Эмперически)
        {
        float now_angle = abs(rect.angle);
        QString to_form; to_form.setNum(now_angle);
        ui->lineEdit->setText(to_form);

        line(original,Point(point_rect[0].x,point_rect[0].y),Point(point_rect[1].x,point_rect[1].y),
            cvScalar(0,0,255),1);
        line(original,Point(point_rect[2].x,point_rect[2].y),Point(point_rect[1].x,point_rect[1].y),
                cvScalar(0,0,255),1);
        line(original,Point(point_rect[3].x,point_rect[3].y),Point(point_rect[2].x,point_rect[2].y),
                cvScalar(0,0,255),1);
        line(original,Point(point_rect[0].x,point_rect[0].y),Point(point_rect[3].x,point_rect[3].y),
                cvScalar(0,0,255),1);

        if ((!flag_wait_answer)&&ui->check_correct_pos->isChecked())
            {
            if (abs(need_andle - now_angle)>1.0)
                {
                flag_wait_answer = true;
                emit rotate_motor(now_angle- need_andle);
                }
            else
                {}
            }
        }

}



void image_process::on_chk_capture_image_stateChanged(int arg1)
{
    if (arg1)
        {
        slot_cycle_get_images();
        }
}


float image_process::integral_intensity(const Mat &Mat_to_count)
{
    long double itt = 0;
    for (int x = 0; x<Mat_to_count.rows;++x)
        for (int y = 0; y<Mat_to_count.cols;++y)
        itt+=Mat_to_count.at<unsigned char>(x,y);
    return itt/(Mat_to_count.cols*Mat_to_count.rows);
}

void image_process::slot_command_to_motor()
{
    flag_wait_answer = false;
}



void image_process::slot_close()
{
    ui->chk_capture_image->setChecked(false);
}



void image_process::on_line_angle_editingFinished()
{
    bool okay_translate = false;
    float angle = ui->line_angle->text().toFloat(&okay_translate);
    if (okay_translate)
        need_andle = angle;
}
