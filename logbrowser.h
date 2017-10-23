#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QWidget>

namespace Ui {
class LogBrowser;
}

class LogBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LogBrowser(QWidget *parent = 0);
    ~LogBrowser();

private:
    Ui::LogBrowser *ui;
};

#endif // LOGBROWSER_H
