#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class LogBrowser;
}

class LogBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LogBrowser(QWidget *parent = 0);
    ~LogBrowser();

public slots:
    void AppendTextToLog( QString Message, QString Group = "UNDEF");

private slots:
    void on_pushButton_clicked();

    void on_btnClearLog_clicked();

private:

    QFile * LogFile;
    QString addr;

    Ui::LogBrowser *ui;
};

#endif // LOGBROWSER_H
