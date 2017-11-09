#include "image_process.h"
#include "ui_image_process.h"

using namespace std;
using namespace cv;

image_recv::image_recv()
{
    capture = VideoCapture(CV_CAP_ANY);
}

void image_recv::get_and_calc_pict()
{
      state_recv = get_pict_find_green;
      if (!capture.isOpened())
          return;

      emit state_change("Поиск зеленого");
      while (state_recv != end_recv)
      {
      Mat original;
      capture >> original;

      if (state_recv == get_pict_find_green || state_recv == get_pict_find_no_green)
      count_RGB(original);

      Mat gray;
      cvtColor(original, gray, CV_RGB2GRAY);

      Mat blur_and_treshhold;
      blur(gray,blur_and_treshhold,Size(3,3),Point(-1,-1));
      threshold(blur_and_treshhold, blur_and_treshhold,threshold_intensiv,255,CV_THRESH_BINARY);

      int radius = 1;
      Mat element = getStructuringElement( CV_SHAPE_ELLIPSE,
                                             Size( 2*radius + 1, 2*radius+1 ),
                                             Point( radius, radius ));
      Mat dilate_erade;
      for (int i = 0; i<2;++i)
      {
      dilate(blur_and_treshhold, dilate_erade, element);
      }
      erode(dilate_erade, dilate_erade, element);

      Mat Canny_image;
      cv::Canny(dilate_erade, Canny_image,1, 3, 3);

      vector<vector<Point>> contours;
      vector<cv::Vec4i> hierarchy;

      findContours( Canny_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0, 0));

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
           continue;

       RotatedRect rect = cv::minAreaRect(contours[num_max]);
       Point2f point_rect[4]; rect.points(point_rect);

       cv::Point point_rect_to_draw[4];

       for(int i = 0; i < 4; ++i)
           {
           point_rect_to_draw[i] = point_rect[i];
           }

       Mat find_polygon = Mat(gray.rows,gray.cols,CV_8UC1, Scalar(0,0,0));
       fillConvexPoly(find_polygon, point_rect_to_draw, 4, Scalar(255,255,255));

       Mat Mat_xor = find_polygon^(~dilate_erade);

       if (state_recv == get_pict_count_angle)
           {
           float error_value = integral_intensity(Mat_xor);
           if (error_value < threshold_work)
                {
                float angle = rect.angle;
                emit find_angle(angle, 0, 0);
                }
           }



       switch (image_show)
        {
        case type_image_to_show::original : mat_to_pixmap(original);break;
        case type_image_to_show::gray     : mat_to_pixmap(gray);break;
        case type_image_to_show::ufter_treshhold : mat_to_pixmap(blur_and_treshhold);break;
        case type_image_to_show::dilate_erade : mat_to_pixmap(dilate_erade);break;
        case type_image_to_show::Canny : mat_to_pixmap(Canny_image);break;
        case type_image_to_show::Poly : mat_to_pixmap(find_polygon);break;
        case type_image_to_show::Ufter_xor : mat_to_pixmap(Mat_xor);break;
        }


      }

      if (capture.isOpened())
      capture.release();
}


void image_recv::stop_recv()
{
    state_recv = end_recv;
}

void image_recv::change_type_foto(QString type_foto)
{
    if (type_foto=="Оригинальный")
        {image_show = original;return;}
    if (type_foto=="Черно-белый")
        {image_show = gray;return;}
    if (type_foto=="После фильтра яркости")
        {image_show = ufter_treshhold;return;}
    if (type_foto=="После расширения")
        {image_show = dilate_erade;return;}
    if (type_foto=="После Канни")
        {image_show = Canny;return;}
    if (type_foto=="Полигон")
        {image_show = Poly;return;}
    if (type_foto=="Сложение с картинкой")
        {image_show = Ufter_xor;return;}
}

void image_recv::change_intesivity(uchar intesiv)
{
    threshold_intensiv = intesiv;
}

void image_recv::change_error(uchar procent)
{
    threshold_work = procent;
}

void image_recv::begin_find_green()
{
    state_recv = get_pict_find_green;
    emit state_change("Поиск зеленого");
}

float image_recv::integral_intensity(const Mat &Mat_to_count)
{
    long double itt = 0;
    for (int x = 0; x<Mat_to_count.rows;++x)
        for (int y = 0; y<Mat_to_count.cols;++y)
        itt+=Mat_to_count.at<unsigned char>(x,y);
    return itt/(Mat_to_count.cols*Mat_to_count.rows);
}

void image_recv::count_RGB(const Mat &Mat_to_count)
{
    double R = 0, G = 0, B = 0;

    for (int x = 0; x<Mat_to_count.rows;++x)
         for (int y = 0;y<Mat_to_count.cols;++y)
            {
            R += Mat_to_count.at<Vec3b>(x,y)[0];
            G += Mat_to_count.at<Vec3b>(x,y)[1];
            B += Mat_to_count.at<Vec3b>(x,y)[2];
            }
    R/= (Mat_to_count.rows*Mat_to_count.cols);
    G/= (Mat_to_count.rows*Mat_to_count.cols);
    B/= (Mat_to_count.rows*Mat_to_count.cols);

    emit intensiv_RGB(R,G,B);

    if (state_recv == get_pict_find_green && (G-(R+B)/2)>50)
        {
        state_recv = get_pict_find_no_green;
        emit state_change("Поиск отсутствия зеленого");
        return;
        }
    if (state_recv == get_pict_find_no_green && (G-(R+B)/2)<50)
        {
        state_recv = get_pict_count_angle;
        emit state_change("Поиск и корректировка угла");
        return;
        }
}

void image_recv::mat_to_pixmap(const Mat &src)
{
    mutex_pict.lock();
    QImage img;
    if (src.channels()==3)
        img = QImage((uchar*)(src.data), src.cols, src.rows, QImage::Format_RGB888);
    else
        img = QImage((uchar*)(src.data), src.cols, src.rows, QImage::Format_Indexed8);

    image = QPixmap::fromImage(img);
    mutex_pict.unlock();
    draw_pict();
}


image_process::image_process(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_process)
{
    ui->setupUi(this);

    ui->combo_type_pict->addItem("Оригинальный");
    ui->combo_type_pict->addItem("Черно-белый");
    ui->combo_type_pict->addItem("После фильтра яркости");
    ui->combo_type_pict->addItem("После расширения");
    ui->combo_type_pict->addItem("После Канни");
    ui->combo_type_pict->addItem("Полигон");
    ui->combo_type_pict->addItem("Сложение с картинкой");
}

image_process::~image_process()
{
    if (ui->chk_capture_image->isChecked())
        stop_thread();
    delete ui;
}




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
    if (image_recv_object)
        {
        image_recv_object->mutex_pict.lock();
        ui->lable_Image->setPixmap(image_recv_object->image);
        image_recv_object->mutex_pict.unlock();
        }
}

void image_process::intensiv_recved(uchar R, uchar G, uchar B)
{
    ui->line_R->setText(QString::number(R));
    ui->line_G->setText(QString::number(G));
    ui->line_B->setText(QString::number(B));
}

void image_process::angle_recved(float angle, int x, int y)
{
    float now_angle = abs(angle);
    QString to_form; to_form.setNum(now_angle);
    ui->lineEdit->setText(to_form);

    if (!flag_wait_answer)
        if (abs(need_andle - now_angle)>1.0)
            {
            flag_wait_answer = true;
            emit rotate_motor(now_angle- need_andle);
            }
        else
            {
            image_recv_object->begin_find_green();
            }
}

void image_process::state_changed(QString state)
{
    ui->line_state->setText(state);
}

void image_process::error_on_pict(float error)
{
    ui->spin_error->setValue(error);
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
    image_recv_object->change_intesivity(value);
}

void image_process::on_slider_intesivity_valueChanged(int value)
{
    ui->value_intesiv->setText(QString::number(value));
    image_recv_object->change_error(value);
}

void image_process::start_thread()
{
    thread_pict = new QThread();
    image_recv_object = new image_recv();
    image_recv_object->change_type_foto(ui->combo_type_pict->currentText());
    image_recv_object->moveToThread(thread_pict);
    flag_wait_answer = false;

    connect(thread_pict, SIGNAL(started()), image_recv_object, SLOT(get_and_calc_pict()));
    connect(image_recv_object, SIGNAL(draw_pict()),this, SLOT(draw_pict()));
    connect(image_recv_object, SIGNAL(intensiv_RGB(uchar,uchar,uchar)),this, SLOT(intensiv_recved(uchar,uchar,uchar)));
    connect(image_recv_object, SIGNAL(find_angle(float,int,int)), this, SLOT(angle_recved(float,int,int)));
    connect(image_recv_object, SIGNAL(state_change(QString)), this, SLOT(state_changed(QString)));
    connect(image_recv_object, SIGNAL(error_value(float)), this, SLOT(error_on_pict(float)));

    thread_pict->start();

}

void image_process::stop_thread()
{
      image_recv_object->stop_recv();
      thread_pict->quit();
      thread_pict->wait();
      delete thread_pict;
      delete image_recv_object;
      image_recv_object = 0;
}



void image_process::on_combo_type_pict_currentTextChanged(const QString &arg1)
{
    if (image_recv_object)
    image_recv_object->change_type_foto(arg1);
}
