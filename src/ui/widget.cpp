#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

#include <iostream>
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->pushButton_loadData, SIGNAL (clicked()),this, SLOT(getDataPath()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::getDataPath()
{
    cout << "idiot" << endl;
    QString path = QFileDialog::getExistingDirectory();
    cout << path.toStdString() << endl;
}
