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
    void on_btn_work_with_image_pressed();
    void slot_cycle_get_images();
private:
    Ui::image_process *ui;
    cv::VideoCapture capture;
    cv::Mat original;
    cv::Mat gray;
};

#endif // IMAGE_PROCESS_H
