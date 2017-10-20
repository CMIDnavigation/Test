#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QWidget>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <QString>
#include <QVector>


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
    void slot_cycle_get_images();
    void slot_get_and_calc_image();
    void on_chk_capture_image_stateChanged(int arg1);

private:
    Ui::image_process *ui;
    cv::VideoCapture capture;
    cv::Mat original;
    cv::Mat gray;
    float integral_intensity(const cv::Mat &Mat_to_count);

};

#endif // IMAGE_PROCESS_H
