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
    QString path = QFileDialog::getOpenFileName(this, tr("Select video"),/*tr("."),*/tr("Movie Files (*.mp4 *.avi)"));
    cout << path.toStdString() << endl;
}

