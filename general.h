#ifndef GENERAL_H
#define GENERAL_H

#include <QMainWindow>

namespace Ui {
class general;
}

class general : public QMainWindow
{
    Q_OBJECT

public:
    explicit general(QWidget *parent = 0);
    ~general();

private:
    Ui::general *ui;
};

#endif // GENERAL_H
