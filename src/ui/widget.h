#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//#include "iprocessing.h"
#include "planetprocessing.h"


namespace Ui {
class Widget;
}

class Widget : public QWidget, public Iprocessing
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    virtual bool dataReady() override;

public slots:
    void getDataPath();

private:
    Ui::Widget *ui;
    PlanetProcessing m_pp;
};

#endif // WIDGET_H
