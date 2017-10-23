#include "logbrowser.h"
#include "ui_logbrowser.h"

LogBrowser::LogBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogBrowser)
{
    ui->setupUi(this);
    ui->btnCloseLog->setVisible(0);
    ui->comboFilter->addItem(" ALL ");
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
}


void LogBrowser::AppendTextToLog( QString Message, QString Group)
{
    int newGroup = 1;
    for (int i = 0; i< ui->comboFilter->count(); i++)
        if (Group == ui->comboFilter->itemData(i))
            newGroup = 0;
    if (newGroup == 1) ui->comboFilter->addItem(Group);
    ui->LogBrowserBox->append("["+Group+"]: " + Message);
}
