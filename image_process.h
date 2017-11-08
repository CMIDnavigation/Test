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


class image_recv : public QObject
{
    Q_OBJECT
public :
    image_recv();
    QMutex mutex_pict;
    QPixmap image;
private :
    cv::VideoCapture capture;
    enum state_thread{end_recv,get_pict,get_pict_and_count_angle}state_recv;
    enum type_image_to_show{original,gray}image_show;
public slots :
    void get_and_calc_pict();
    void stop_recv();
    void change_type_foto(QString type_foto);
private slots :
    float integral_intensity(const cv::Mat &Mat_to_count);
    void mat_to_pixmap(const cv::Mat &src);
signals :
    draw_pict();
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
//    void slot_cycle_get_images();
//    void slot_get_and_calc_image();
    void on_chk_capture_image_stateChanged(int arg1);
    void on_line_angle_editingFinished();
    void on_slider_Y_valueChanged(int value);
    void on_slider_intesivity_valueChanged(int value);
//    void slot_mat_to_widget(cv::Mat image_to_show);
    void start_thread();
    void stop_thread();
private:
    QThread* thread_pict;
    image_recv* image_recv_object;
    float need_andle = 0;
    bool flag_wait_answer = false;
    Ui::image_process *ui;    
public slots :
    void slot_command_to_motor();
    void slot_close();
    void draw_pict();
signals:
    rotate_motor(float angle); 
};

#endif // IMAGE_PROCESS_H
