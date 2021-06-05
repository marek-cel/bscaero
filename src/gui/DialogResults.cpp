#include "DialogResults.h"
#include "ui_DialogResults.h"

DialogResults::DialogResults(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogResults)
{
    ui->setupUi(this);
}

DialogResults::~DialogResults()
{
    delete ui;
}
