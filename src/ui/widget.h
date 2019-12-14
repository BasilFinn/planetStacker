#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "planetprocessing.h"


namespace Ui {
class Widget;
}

class Widget : public QWidget, public Iprocessing
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;
    bool dataReady() override;
    void updateBar(int currentFrame) override;
    void initBar(int maxFrames) override;

public slots:
    void getDataPath();
    void setSharpGauss(int inInt);
    void setSharpWeightOrg(double weightOrg);
    void setSharpWeightBlurr(double weightBlurr);
    void updateStackThres(double newThres);
    void resetView();
    void saveImage();

private:
    Ui::Widget *ui;
    PlanetProcessing m_pp;

};

#endif // WIDGET_H
