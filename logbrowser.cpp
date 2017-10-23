#include "logbrowser.h"
#include "ui_logbrowser.h"

LogBrowser::LogBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogBrowser)
{
    ui->setupUi(this);
}

LogBrowser::~LogBrowser()
{
    delete ui;
}
