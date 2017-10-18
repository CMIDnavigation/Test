#ifndef GENERAL_H
#define GENERAL_H

#include <QMainWindow>
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
private:
    Ui::general *ui;
};

#endif // GENERAL_H
