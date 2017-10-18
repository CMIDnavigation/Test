#include "motorcontrol.h"
#include "ui_motorcontrol.h"

MotorControl::MotorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotorControl)
{
    ui->setupUi(this);

}

MotorControl::~MotorControl()
{
    delete ui;
}
