#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

#include <iostream>
#include "iprocessing.h"
#include "planetprocessing.h"

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_pp(this)
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select video"),tr("."),tr("Movie Files (*.mp4 *.avi)"));
    cout << path.toStdString() << endl;
    m_pp.savePath(path.toStdString());

    ui->setupUi(this);
    connect(ui->pushButton_loadData, SIGNAL (clicked()),this, SLOT(getDataPath()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::getDataPath()
{
//    QString path = QFileDialog::getOpenFileName(this, tr("Select video"),tr("."),tr("Movie Files (*.mp4 *.avi)"));
//    cout << path.toStdString() << endl;
//    m_pp.savePath(path.toStdString());
    cout << "Path: " << m_pp.getPath() << endl;
}

bool Widget::dataReady()
{
    // Dummy function
    return true;
}
