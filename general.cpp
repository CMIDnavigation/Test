#include "general.h"
#include "ui_general.h"

general::general(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::general)
{
    ui->setupUi(this);
    motor = new MotorControl(this);
    image = new image_process(this);

    connect(image,image_process::rotate_motor, motor, MotorControl::GetAngleFromCam);
    connect(motor, MotorControl::SendMoveDone, image, image_process::slot_command_to_motor );

    ui->layout_image->addWidget(image);
    ui->layout_motor->addWidget(motor);
}

general::~general()
{
    delete ui;
}
