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

namespace Ui {
class MotorControl;
}

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QTextBrowser *LogTextWindow;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnExit;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        LogTextWindow = new QTextBrowser(Dialog);
        LogTextWindow->setObjectName(QStringLiteral("LogTextWindow"));

        verticalLayout->addWidget(LogTextWindow);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btnExit = new QPushButton(Dialog);
        btnExit->setObjectName(QStringLiteral("btnExit"));

        horizontalLayout_2->addWidget(btnExit);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Log browser", Q_NULLPTR));
        btnExit->setText(QApplication::translate("Dialog", "Exit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui


class Ctrl_loop : public QObject
{
    Q_OBJECT
    QThread *hThread;
public:

    device_t * Device;
    status_t * State;
    bool stopThreadFlag;
    bool Ready;
    explicit Ctrl_loop(QObject *parent = 0);
    Ctrl_loop( device_t * D );
    ~Ctrl_loop();
    void stopThread();
    void startThread();

signals:
    void SendStatus(status_t * Status);
    void ConnectionError( void );
    void MoveDone( void );
public slots:
    void StartLoop(void);
    void GetAngleFromCam(float angle);
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

    void InitMotorDrive();
    //void printStateToStr (QString string,  status_t* state);
    const wchar_t * error_string (result_t result)
    {
        switch (result)
        {
            case result_error:				return L"error";
            case result_not_implemented:	return L"not implemented";
            case result_nodevice:			return L"no device";
            default:						return L"success";
        }
    }

signals:
    void NoMotorConnection();
    void MotorConnectionOK();
    void AppendTextToLog(const QString &Message, const QString &Group = "UNDEF");
    void SendMoveDone();
    void SendAngleFromCam(float Angle);

public slots:
    void GetAngleFromCam( float Angle);
private slots:
    void NoMotorConnectionProcess();
    void MotorConnectionOKProcess();
    void UpdateStatus(status_t * State);
    void CloseLogDialog();
    void GetMoveDone();

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
