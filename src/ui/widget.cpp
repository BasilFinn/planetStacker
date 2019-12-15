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
    // Saving
    connect(ui->pushButton_saveImage,SIGNAL(clicked()),this, SLOT(saveImage()));
    // Scaling
    connect(ui->spinBox_scaleFactor, SIGNAL(valueChanged(double)), this, SLOT(scaleImage(double)));
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

void Widget::setSharpGauss(int inInt)
{
    m_pp.m_sharp_gauss = inInt;
    m_pp.sharpenFrame();
}

void Widget::setSharpWeightBlurr(double weightBlurr)
{
    m_pp.m_sharp_weightBlurr = weightBlurr;
    m_pp.sharpenFrame();
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

void Widget::saveImage()
{
    std::string path = m_pp.m_dataPath;
    size_t found;
    found=path.find_last_of("/\\");
    std::string name =  path.substr(found+1);
    path = path.substr(0,found+1);
    found = name.find_last_of(".");
    name = name.substr(0,found);

    cout << "savepath: " << path + name + ".png" << endl;

    cv::imwrite(path + name + ".png", m_pp.m_outMat);
}

void Widget::scaleImage(double scaleFactor)
{
    m_pp.m_scaleFactor = scaleFactor;
    m_pp.stackFrames();
}

void Widget::setSharpWeightOrg(double weightOrg)
{
    m_pp.m_sharp_weightOrg = weightOrg;
    m_pp.sharpenFrame();
}

// Callbacks
bool Widget::dataReady()
{
    cv::Mat img = m_pp.m_outMat.clone();
    img.convertTo(img, CV_8UC3);
    ui->label_image->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped()));
    return true;
}

void Widget::updateBar(int currentFrame)
{
    ui->progressBar->setValue(currentFrame);
}

void Widget::initBar(int maxFrames)
{
    ui->progressBar->setMaximum(maxFrames);
}
