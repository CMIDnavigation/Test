#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QSet>
#include <QVector>
#include <QPair>
#include <QTime>
#include <QDate>
#include <QTextStream>
#include <QDir>

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
    void on_btnSaveLog_clicked();
    void on_comboFilter_currentIndexChanged(const QString &arg1);

private:

    QFile * LogFile;
    QTextStream * LogToFileStream;

    QVector< QPair<QString,QString> > MessageArray;
    Ui::LogBrowser *ui;
};

#endif // LOGBROWSER_H
