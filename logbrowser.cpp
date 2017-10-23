#include "logbrowser.h"
#include "ui_logbrowser.h"

LogBrowser::LogBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogBrowser)
{
    ui->setupUi(this);
    ui->btnCloseLog->setVisible(0);
    ui->comboFilter->addItem(" ALL ");

    QFont myfont ("Monospace");
    myfont.setStyleHint(QFont::Monospace);
    myfont.setPixelSize(12);
    ui->LogBrowserBox->setFont(myfont);
}

LogBrowser::~LogBrowser()
{
    delete ui;
}

void LogBrowser::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Select log file"), "",
            tr("Text file (*.txt);;Log files (*.log);;All Files (*)"));

    if (fileName.length())
        ui->lineFileAddr->setText(fileName);
}

void LogBrowser::on_btnClearLog_clicked()
{
    ui->LogBrowserBox->clear();
    MessageArray.clear();
}


void LogBrowser::AppendTextToLog( QString Message, QString Group)
{
    if (ui->comboFilter->findText(Group) == -1)
    {
        ui->comboFilter->addItem(Group);
    }
    QTime CurTime = QTime::currentTime();
    QDate CurDay = QDate::currentDate();
    MessageArray.push_back(qMakePair(Group, CurDay.toString("dd.MM.yyyy") + "/" + CurTime.toString("hh:mm:ss") + " >>> " + Message));

    if (ui->comboFilter->currentText() == Group || ui->comboFilter->currentText() == " ALL ")
        ui->LogBrowserBox->append("["+Group+"]: " + CurDay.toString("dd.MM.yyyy") + "/" + CurTime.toString("hh:mm:ss") + " >>> " + Message);
}

void LogBrowser::on_btnSaveLog_clicked()
{
    AppendTextToLog( "QString Message", "ERROR");
}

void LogBrowser::on_comboFilter_currentIndexChanged(const QString &arg1)
{
    ui->LogBrowserBox->clear();

   for(auto CurrentLogItem = MessageArray.begin(); CurrentLogItem!= MessageArray.end(); CurrentLogItem++)
       if (arg1 == CurrentLogItem->first || arg1 == " ALL ")
            ui->LogBrowserBox->append("[" + CurrentLogItem->first + "]: " + CurrentLogItem->second);
}
