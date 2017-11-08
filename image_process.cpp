#include "image_process.h"
#include "ui_image_process.h"

using namespace std;
using namespace cv;

image_recv::image_recv(QPixmap *total_pixmap):total_pixmap(total_pixmap)
{

}

void image_recv::get_and_calc_pict()
{
      capture = VideoCapture(CV_CAP_ANY);
      state_recv = get_pict;

      while (state_recv != end_recv)
      {
      cvWaitKey(33);
      }

//      if (capture.isOpened())
//      capture.release();
}

void image_recv::stop_recv()
{
    state_recv = end_recv;
}

float image_recv::integral_intensity(const Mat &Mat_to_count)
{
    long double itt = 0;
    for (int x = 0; x<Mat_to_count.rows;++x)
        for (int y = 0; y<Mat_to_count.cols;++y)
        itt+=Mat_to_count.at<unsigned char>(x,y);
    return itt/(Mat_to_count.cols*Mat_to_count.rows);
}

void image_recv::mat_to_pixmap(const Mat &src)
{
    mutex_pict.lock();
    QImage img;
    if (src.channels()==3)
        img = QImage((uchar*)(src.data), src.cols, src.rows, QImage::Format_RGB888);
    else
        img = QImage((uchar*)(src.data), src.cols, src.rows, QImage::Format_Indexed8);

    if (total_pixmap) delete total_pixmap;
    total_pixmap = new QPixmap(QPixmap::fromImage(img));
    mutex_pict.unlock();
}


image_process::image_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_process)
{
    ui->setupUi(this);
    image = 0;
//    capture = VideoCapture(CV_CAP_ANY);
//    if (!capture.isOpened())
//        ui->chk_capture_image->setEnabled(false);




    ui->combo_type_pict->addItem("Оригинальный");
    ui->combo_type_pict->addItem("Черно-белый");
    ui->combo_type_pict->addItem("После фильтра яркости");
    ui->combo_type_pict->addItem("После расширения");
    ui->combo_type_pict->addItem("После Канни");
    ui->combo_type_pict->addItem("Черно-белый");
    ui->combo_type_pict->addItem("Полигон");
}

image_process::~image_process()
{
    if (ui->chk_capture_image->isChecked())
        stop_thread();
    if (image) delete image;
    delete ui;
}



/*
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
*/

/*
void image_process::slot_get_and_calc_image()
{
    capture>>original;
    if (ui->combo_type_pict->currentText()=="Оригинальный")
        slot_mat_to_widget(original);

    cvtColor(original, gray, CV_RGB2GRAY);
    if (ui->combo_type_pict->currentText()=="Черно-белый")
        slot_mat_to_widget(gray);

    ufter_plus = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));
    cv::blur(gray,gray,cv::Size(3,3),cv::Point(-1,-1));

//    float integral_gray = integral_intensity(gray);
//    if (integral_gray>170)
//       threshold(gray, ufter_plus,60,255,CV_THRESH_BINARY);
//    else if (integral_gray>100)
//       threshold(gray, ufter_plus,45,255,CV_THRESH_BINARY);
//    else
//       threshold(gray, ufter_plus,25,255,CV_THRESH_BINARY);



    int radius = 1;
    Mat element = getStructuringElement( CV_SHAPE_ELLIPSE,
                                           Size( 2*radius + 1, 2*radius+1 ),
                                           Point( radius, radius ) );
    threshold(gray, ufter_plus,ui->slider_Y->value(),255,CV_THRESH_BINARY);

    Mat buffer1;//Буфферная зона1
    threshold(gray, buffer1,ui->slider_Y->value(),255,CV_THRESH_BINARY);

    if (ui->combo_type_pict->currentText()=="После фильтра яркости")
        slot_mat_to_widget(buffer1);

//    cvNamedWindow("ufter_treshhold", CV_WINDOW_AUTOSIZE);
//    imshow("ufter_treshhold",buffer1);

    for (int i = 0; i<2;++i)
    {
    dilate(ufter_plus, ufter_plus, element);
    }

    Mat buffer2;//Буфферная зона2
    erode(ufter_plus, buffer2, element);
    if (ui->combo_type_pict->currentText()=="После расширения")
        slot_mat_to_widget(buffer2);

//    cvNamedWindow("ufter erode", CV_WINDOW_AUTOSIZE);
//    imshow("ufter erode",buffer2);

    erode(ufter_plus, ufter_plus, element);

    vector<vector<Point>> contours;
    vector<cv::Vec4i> hierarchy;

   Canny( ufter_plus, gray, 1, 3, 3 );

   Mat buffer3;//Буфферная зона3
   Canny( ufter_plus, buffer3, 1, 3, 3 );
   if (ui->combo_type_pict->currentText()=="После Канни")
       slot_mat_to_widget(buffer3);
//   cvNamedWindow("ufter Canny", CV_WINDOW_AUTOSIZE);
//   imshow("ufter Canny",buffer3);



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

    Mat buffer4 = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));;//Буфферная зона3
    fillConvexPoly(buffer4, point_rect_to_draw, 4, Scalar(255,255,255));

    if (ui->combo_type_pict->currentText()=="Полигон")
        slot_mat_to_widget(buffer4);
//    cvNamedWindow("Poly", CV_WINDOW_AUTOSIZE);
//    imshow("Poly",buffer4);


    gray = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));
    fillConvexPoly(gray, point_rect_to_draw, 4, Scalar(255,255,255));


    gray^=(~ufter_plus);

    double intensivity = integral_intensity(gray);
    ui->spin_intensivity->setValue(intensivity);


    if (intensivity<(ui->slider_intesivity->value()))//Эмперически)
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
                {
                ui->line_angle->setEnabled(true);
                }
            }
        }

}
*/


void image_process::on_chk_capture_image_stateChanged(int arg1)
{
    if (arg1)
        start_thread();
    else
        stop_thread();
}


void image_process::slot_command_to_motor()
{
    flag_wait_answer = false;
}



void image_process::slot_close()
{
    ui->chk_capture_image->setChecked(false);
}

void image_process::draw_pict()
{
    ui->lable_Image->setPixmap(*image);
}



void image_process::on_line_angle_editingFinished()
{
    bool okay_translate = false;
    float angle = ui->line_angle->text().toFloat(&okay_translate);
    if (okay_translate)
        if (angle>=0 && angle <90)
        need_andle = angle;
    ui->line_angle->setEnabled(false);
}

void image_process::on_slider_Y_valueChanged(int value)
{
    ui->value_Y->setText(QString::number(value));
}

void image_process::on_slider_intesivity_valueChanged(int value)
{
    ui->value_intesiv->setText(QString::number(value/100));
}

void image_process::start_thread()
{
    thread_pict = new QThread();
    image_recv_object = new image_recv(image);
    image_recv_object->moveToThread(thread_pict);

    connect(thread_pict, SIGNAL(started()), image_recv_object, SLOT(get_and_calc_pict()));


    thread_pict->start();

}

void image_process::stop_thread()
{
      image_recv_object->stop_recv();
      thread_pict->quit();
      thread_pict->wait();
      delete thread_pict;
      delete image_recv_object;
}


