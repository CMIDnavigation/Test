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
      state_recv = get_pict_find_blue;
      if (!capture.isOpened())
          return;

      emit state_change("Поиск зеленого");
      while (state_recv != end_recv)
      {
      Mat original;
      capture >> original;

      if ((state_recv == get_pict_find_blue) || (state_recv == get_pict_find_no_blue))
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
                emit find_angle(angle, rect.center.x, rect.center.y);
                emit state_change("Я посчиталь и отправил");
                }
           else
               emit state_change("Слишком большая ошибка");
           }
        else
            {
            if ((state_recv == get_pict_find_blue)||(state_recv == get_pict_find_no_blue))
                {}
            else
                emit state_change("Странное состояние");
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
    state_recv = get_pict_find_blue;
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
            R += Mat_to_count.at<Vec3b>(x,y)[2];
            G += Mat_to_count.at<Vec3b>(x,y)[1];
            B += Mat_to_count.at<Vec3b>(x,y)[0];
            }
    R/= (Mat_to_count.rows*Mat_to_count.cols);
    G/= (Mat_to_count.rows*Mat_to_count.cols);
    B/= (Mat_to_count.rows*Mat_to_count.cols);

    emit intensiv_RGB(R,G,B);

    if (state_recv == get_pict_find_blue && B>170 && G<200 )
        {
        state_recv = get_pict_find_no_blue;
        emit state_change("Поиск отсутствия зеленого");
        return;
        }
    if (state_recv == get_pict_find_no_blue && B<100)
        {
        state_recv = get_pict_count_angle;
        emit state_change("Поиск и корректировка угла");
        return;
        }
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
    write_to_log("count pict RGB "+
                 QString::number(R)+" "+QString::number(G)+
                 " "+QString::number(B), "pict");
    ui->line_R->setText(QString::number(R));
    ui->line_G->setText(QString::number(G));
    ui->line_B->setText(QString::number(B));
}

void image_process::angle_recved(float angle, float x, float y)
{

    float now_angle = abs(angle);
    QString to_form; to_form.setNum(now_angle);
    ui->line_count_angle->setText(to_form);
    write_to_log("angle count "+to_form, "pict");

    if (!flag_wait_answer)
        {
        if (abs(need_andle - now_angle)>1.0)
            {
            flag_wait_answer = true;
            emit rotate_motor(now_angle- need_andle);
            }
        else
            {

            QString x_val; x_val.setNum(x);
            QString y_val; y_val.setNum(y);

            write_to_log("x y rect "+x_val+" "+y_val, "pict");

            QPoint pos_now = QCursor::pos();

            mouse_move_and_click(290,410);
            mouse_move_and_click(290,410);

            press_value(x_val);
            press_value(y_val);
            press_value("0");

            mouse_move_and_click(200,340);

            this->show();

            QCursor::setPos(pos_now);

            image_recv_object->begin_find_green();
            state_changed("Поиск зеленого");
            }
        }
}

void image_process::state_changed(QString state)
{
    write_to_log("state changed "+state, "pict");
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
    emit write_to_log("begin thread recv","pict");
    thread_pict = new QThread();
    image_recv_object = new image_recv();
    image_recv_object->change_type_foto(ui->combo_type_pict->currentText());
    image_recv_object->moveToThread(thread_pict);
    flag_wait_answer = false;

    connect(thread_pict, SIGNAL(started()), image_recv_object, SLOT(get_and_calc_pict()));
    connect(image_recv_object, SIGNAL(draw_pict()),this, SLOT(draw_pict()));
    connect(image_recv_object, SIGNAL(intensiv_RGB(uchar,uchar,uchar)),this, SLOT(intensiv_recved(uchar,uchar,uchar)));
    connect(image_recv_object, SIGNAL(find_angle(float,float,float)), this, SLOT(angle_recved(float,float,float)));
    connect(image_recv_object, SIGNAL(state_change(QString)), this, SLOT(state_changed(QString)));
    connect(image_recv_object, SIGNAL(error_value(float)), this, SLOT(error_on_pict(float)));

    thread_pict->start();

}

void image_process::stop_thread()
{
    emit write_to_log("stop thread recv","pict");
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

void image_process::on_btn_test_pressed()
{
//    this->hide();

    QPoint pos_now = QCursor::pos();

    mouse_move_and_click(290,410);

    press_value("100");
    press_value("200");
    press_value("1");

    mouse_move_and_click(200,340);

    this->show();

    QCursor::setPos(pos_now);

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

void image_process::press_value(QString message)
{
    for (int i = 0; i<message.size();++i)
    {
    char char_buffer =  message.toStdString()[i];
    if (char_buffer == '.')
    {
        keybd_event(VK_DECIMAL, 0,0,0);
        keybd_event(VK_DECIMAL, 0,KEYEVENTF_KEYUP,0);
    }
    else if (char_buffer == '-')
        {
        keybd_event(VK_SUBTRACT, 0,0,0);
        keybd_event(VK_SUBTRACT, 0,KEYEVENTF_KEYUP,0);
        }
    else
        {
        keybd_event(char_buffer, 0,0,0);
        keybd_event(char_buffer, 0,KEYEVENTF_KEYUP,0);
        }
    }
    keybd_event(VK_RETURN, 0,0,0);
    keybd_event(VK_RETURN, 0,KEYEVENTF_KEYUP,0);
}

void image_process::mouse_move_and_click(int x, int y)
{
    QCursor::setPos(x,y);
    //SetCursorPos(x, y);
    for (int i = 0; i<2; i++)
    {
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);//нажатие левой кнопки мыши
        Sleep(1);
    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);//отпускание кнопки
        Sleep(1);
    }
}
