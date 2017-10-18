#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QWidget>

namespace Ui {
class image_process;
}

class image_process : public QWidget
{
    Q_OBJECT

public:
    explicit image_process(QWidget *parent = 0);
    ~image_process();

private:
    Ui::image_process *ui;
};

#endif // IMAGE_PROCESS_H
