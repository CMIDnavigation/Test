#ifndef GENERAL_H
#define GENERAL_H

#include <QMainWindow>
#include "motorcontrol.h"
#include "image_process.h"

namespace Ui {
class general;
}

class general : public QMainWindow
{
    Q_OBJECT

public:
    explicit general(QWidget *parent = 0);
    ~general();
    image_process* image;
    MotorControl* motor;
private:
    Ui::general *ui;
};

#endif // GENERAL_H
