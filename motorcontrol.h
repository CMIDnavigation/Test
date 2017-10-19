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
#include <QThread>
#include <QDialog>
#include <QPushButton>

#include "ximc.h"
#include "ui_motorcontrol.h"
#include "ui_logbox.h"

namespace Ui {
class MotorControl;
}

void XIMC_CALLCONV my_logging_callback(int loglevel, const wchar_t* message, void* user_data);
const wchar_t* loglevel_string (int loglevel);


class Ctrl_loop : public QObject
{
    Q_OBJECT
public:
    device_t * Device;
    status_t * State;
    bool Ready;
    explicit Ctrl_loop(QObject *parent = 0);
    Ctrl_loop( device_t * D )
    {
        State = new status_t;
        Device = D;
        Ready = true;
    }
    ~Ctrl_loop()
    {
        //
    }
signals:
    void SendStatus(status_t * Status);
    void ConnectionError( void );
    void MoveDone( void );
public slots:
    void StartLoop(void);
    void AdjustAngle(float angle);
};

class MotorControl : public QWidget
{
    Q_OBJECT

public:

    device_t * Device;
    Ctrl_loop * ControlLoop;
    QDialog * LogDialogBox;

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
    void AppendTextToLog(const QString &String);

private slots:
    void NoMotorConnectionProcess();
    void MotorConnectionOKProcess();
    void UpdateStatus(status_t * State);
    void CloseLogDialog();

    void on_btnInitDrive_clicked();
    void on_btnHomeDrive_clicked();
    void on_btnZeroDrive_clicked();
    void on_StartFixedPosMove_clicked();
    void on_btnRight_clicked();
    void on_btnLeft_clicked();
    void on_btnStopMotor_clicked();
    void on_btnGetStatus_clicked();

    void on_btnOpenLog_clicked();

private:
    Ui::MotorControl *ui;
    Ui::Dialog       *LogBox;

};

#endif // MOTORCONTROL_H
