#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

#include <QWidget>

#include "ximc.h"

namespace Ui {
class MotorControl;
}

void XIMC_CALLCONV my_logging_callback(int loglevel, const wchar_t* message, void* user_data);
const wchar_t* loglevel_string (int loglevel);

class MotorControl : public QWidget
{
    Q_OBJECT

public:

    device_t * Device;

    explicit MotorControl(QWidget *parent = 0);
    ~MotorControl();

    void print_state (status_t* state);
    void string_print_state (wchar_t * string,  status_t* state);
    const wchar_t* error_string (result_t result);

    char* widestr_to_str (const wchar_t* str);

    void InitMotorDrive();

signals:
    void NoMotorConnection();
    void MotorConnectionOK();

private slots:
    void NoMotorConnectionProcess();
    void MotorConnectionOKProcess();
    void on_btnInitDrive_clicked();

    void on_btnHomeDrive_clicked();

    void on_btnZeroDrive_clicked();

    void on_StartFixedPosMove_clicked();

    void on_btnRight_clicked();

    void on_btnLeft_clicked();

    void on_btnStopMotor_clicked();

    void on_btnGetStatus_clicked();

private:
    Ui::MotorControl *ui;
};

#endif // MOTORCONTROL_H
