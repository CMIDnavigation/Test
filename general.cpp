#include "general.h"
#include "ui_general.h"

general::general(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::general)
{
    ui->setupUi(this);
    motor = new MotorControl(this);
    image = new image_process(this);
    log = new LogBrowser(this);

    connect(image,image_process::rotate_motor, motor, MotorControl::GetAngleFromCam);
    connect(motor, MotorControl::SendMoveDone, image, image_process::slot_command_to_motor );
    connect(motor, MotorControl::AppendTextToLog,log,LogBrowser::AppendTextToLog);
    connect(image, image_process::write_to_log,log,LogBrowser::AppendTextToLog);

    ui->layout_image->addWidget(image);
    ui->layout_motor->addWidget(motor);
    ui->layout_log->addWidget(log);
}

general::~general()
{
    delete ui;
}

void general::closeEvent(QCloseEvent *event)
{
    image->slot_close();
}
