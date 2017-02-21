// main.h
#ifndef MAIN_H
#define MAIN_H

#include <opencv2/opencv.hpp>
using namespace cv;

class ImgOp
{
public:
    
    // set the source image, may be NULL
    // return <0 on error
    virtual int setImgSrc(Mat *imgSrc);
    
    // run the operation
    // reutrn <0 on error
    virtual int run();
}; // class ImgOp

// Main class is a singleton, used to
class Main
{
public:
    static Main* instance();
    const std::string& getExeName();
    void setExeName(const char* name);
    int registerImgOp(Ptr<ImgOp> imgOp);
private:
    Main();
    std::string exeName;
    std::vector<Ptr<ImgOp>> imgOpCollection;
}; // class Main

#endif // MAIN_H
