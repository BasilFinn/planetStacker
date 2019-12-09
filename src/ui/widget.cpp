#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

#include <iostream>
//#include "iprocessing.h"
////#include "planetprocessing.h"

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_pp(this)
{
    ui->setupUi(this);
    connect(ui->pushButton_loadData, SIGNAL (clicked()),this, SLOT(getDataPath()));
    // Sharpening
    connect(ui->pushButton_sharpen, SIGNAL (clicked()), this, SLOT(sharpen()));
    connect(ui->spinBox_gauss, SIGNAL(valueChanged(int)), this, SLOT(setSharpGauss(int)));
    connect(ui->doubleSpinBox_weightBlurr,SIGNAL(valueChanged(double)), this, SLOT(setSharpGauss(double)));
    connect(ui->doubleSpinBox_weightOrg,SIGNAL(valueChanged(double)), this, SLOT(setSharpGauss(double)));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::getDataPath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select video"),tr("."),tr("Movie Files (*.mp4 *.avi)"));
    // TODO: make exception if no path selected
    cout << path.toStdString() << endl;
    m_pp.savePath(path.toStdString());
    cout << "Path: " << m_pp.getPath() << endl;
    m_pp.startProcessing();
}

void Widget::sharpen()
{
    m_pp.sharpenFrame();
}

void Widget::setSharpGauss(int inInt)
{
    m_pp.m_sharp_gauss = inInt;
}

void Widget::setSharpWeightBlurr(double weightBlurr)
{
    m_pp.m_sharp_weightBlurr = weightBlurr;
}

void Widget::setSharpWeightOrg(double weightOrg)
{
    m_pp.m_sharp_weightOrg = weightOrg;
}

bool Widget::dataReady()
{
    // Implement here (call in planetprocessing)
    cv::Mat img = m_pp.m_outMat.clone();
    img.convertTo(img, CV_8UC3);
    ui->label_image->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped()));
    return true;
}

void Widget::updateBar()
{
    ui->progressBar->setValue(m_pp.m_frameCnt);
}

void Widget::initBar(int maxFrames)
{
    ui->progressBar->setMaximum(maxFrames);
}
