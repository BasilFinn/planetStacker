#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

#include <iostream>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_pp(this)
{
    ui->setupUi(this);
    connect(ui->pushButton_loadData, SIGNAL (clicked()),this, SLOT(getDataPath()));
    // Sharpening
    connect(ui->spinBox_gauss, SIGNAL(valueChanged(int)), this, SLOT(setSharpGauss(int)));
    connect(ui->doubleSpinBox_weightBlurr,SIGNAL(valueChanged(double)), this, SLOT(setSharpWeightBlurr(double)));
    connect(ui->doubleSpinBox_weightOrg,SIGNAL(valueChanged(double)), this, SLOT(setSharpWeightOrg(double)));
    // Stacking
    connect(ui->doubleSpinBox_stackThres,SIGNAL(valueChanged(double)), this, SLOT(updateStackThres(double)));
    connect(ui->pushButton_resetView,SIGNAL(clicked()),this, SLOT(resetView()));
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
    m_pp.startProcessing();
}

void Widget::sharpen()
{
    m_pp.sharpenFrame();
}

void Widget::setSharpGauss(int inInt)
{
    m_pp.m_sharp_gauss = inInt;
    sharpen();
}

void Widget::setSharpWeightBlurr(double weightBlurr)
{
    m_pp.m_sharp_weightBlurr = weightBlurr;
    sharpen();
}

void Widget::updateStackThres(double newThres)
{
    m_pp.m_stackCorrThres = newThres;
    m_pp.stackFrames();
}

void Widget::resetView()
{
    m_pp.m_outMat = m_pp.m_stackedFrame.clone();
    dataReady();
}

void Widget::setSharpWeightOrg(double weightOrg)
{
    m_pp.m_sharp_weightOrg = weightOrg;
    sharpen();
}

// Callbacks
bool Widget::dataReady()
{
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
