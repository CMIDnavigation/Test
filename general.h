#ifndef GENERAL_H
#define GENERAL_H

#include <QMainWindow>
#include "motorcontrol.h"
#include "image_process.h"
#include "logbrowser.h"
#include <QCloseEvent>
#include <QEvent>

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
    LogBrowser* log;
protected :
    virtual void closeEvent(QCloseEvent* event);
private:
    Ui::general *ui;

};

#endif // GENERAL_H
