#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <QWidget>

namespace Ui {
class MotorControl;
}

class MotorControl : public QWidget
{
    Q_OBJECT

public:
    explicit MotorControl(QWidget *parent = 0);
    ~MotorControl();

private:
    Ui::MotorControl *ui;
};

#endif // MOTORCONTROL_H
