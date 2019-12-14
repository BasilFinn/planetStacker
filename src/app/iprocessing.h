#ifndef IPROCESSING_H
#define  IPROCESSING_H

class Iprocessing
{
public:
    virtual bool dataReady() = 0;

    virtual void updateBar(int currentFrame) = 0;
    virtual void initBar(int maxFrames) = 0;
};

#endif // IPROCESSING_H
