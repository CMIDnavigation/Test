#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QWidget>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QImage>
#include <QThread>
#include <QMutex>
#include <windows.h>
#include <QCursor>

class image_recv : public QObject
{
    Q_OBJECT
public :
    image_recv();
    QMutex mutex_pict;
    QPixmap image;
private :
    cv::VideoCapture capture;
    enum state_thread{end_recv,get_pict_find_green, get_pict_find_no_green, get_pict_count_angle,get_pict_wait}state_recv;
    enum type_image_to_show{original,gray,ufter_treshhold,dilate_erade,Canny,Poly,Ufter_xor}image_show;
    uchar threshold_intensiv = 20;
    uchar threshold_work = 1;
public slots :
    void get_and_calc_pict();
    void stop_recv();
    void change_type_foto(QString type_foto);
    void change_intesivity(uchar intesiv);
    void change_error(uchar procent);
    void begin_find_green();
private slots :
    float integral_intensity(const cv::Mat &Mat_to_count);
    void count_RGB(const cv::Mat &Mat_to_count);
    void mat_to_pixmap(const cv::Mat &src);
signals :
    intensiv_RGB(uchar R,uchar G, uchar B);
    draw_pict();
    find_angle(float angle, int x, int y);
    error_value(float value_error);
    state_change(QString state);    
};


namespace Ui {
class image_process;
}

class image_process : public QWidget
{
    Q_OBJECT

public:
    explicit image_process(QWidget *parent = 0);
    ~image_process();

private slots:
    void on_chk_capture_image_stateChanged(int arg1);
    void on_line_angle_editingFinished();
    void on_slider_Y_valueChanged(int value);
    void on_slider_intesivity_valueChanged(int value);
    void start_thread();
    void stop_thread();
    void on_combo_type_pict_currentTextChanged(const QString &arg1);
    void on_btn_test_pressed();
    void press_value(QString message);
    void mouse_move_and_click(int x, int y);
private:
    QThread* thread_pict;
    image_recv* image_recv_object =0;
    float need_andle = 0;
    bool flag_wait_answer = false;
    Ui::image_process *ui;    
public slots :
    void slot_command_to_motor();
    void slot_close();
    void draw_pict();
    void intensiv_recved(uchar R, uchar G, uchar B);
    void angle_recved(float angle, int x, int y);
    void state_changed(QString state);
    void error_on_pict(float error);
signals:
    rotate_motor(float angle); 
};

#endif // IMAGE_PROCESS_H
