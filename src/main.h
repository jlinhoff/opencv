// main.h
#ifndef MAIN_H
#define MAIN_H

#include <opencv2/opencv.hpp>
using namespace cv;

//
// HELPFUL MACROS & DEFINES
//

// ret(ec) 
#define ret(_r_) do{r=(_r_);goto FINAL;}while(0)

// clamp macro- beware of multiple evaluations
#define CLAMP(_x_,_lo_,_hi_) (_x_)<(_lo_)?(_lo_):(_x_)>(_hi_)?(_hi_):(_x_)

enum {
    // error codes -- <0 signals error, >=0 is OK
    ERR_OK=0, // success, no error
    ERR_NOT_IMPLEMENTED=-1000,
    ERR_NODATA,
    ERR_IMAGE_FORMAT_NOT_SUPPORTED,
    ERR_UNKNOWN_OPERATION,
    ERR_SYNTAX,
    ERR_MISSING_FNAME,
    ERR_OUT_OF_RANGE,
    ERR_DATA_ERROR,
};

//
//
//

class ImgOp
{
public:
    // name of this operation
    std::string name;
    
    // get help for the operation
    virtual std::string getHelp();
    
    // reset to defaults, return ERR_ code
    virtual int reset() = 0;
    
    // set the source image for the operation
    // return ERR_ code
    virtual int setImgSrc(Mat imgSrc);
    
    // add arguments, add one argument line at a time
    // arguments depend on operation
    // return ERR_ code
    virtual int addArgs(std::string &args);
    
    // run the operation
    // reutrn ERR_ code
    virtual int run() = 0;
    
}; // class ImgOp

// Main class is a singleton, used to
class Main
{
public:
    static Main* instance();
    const std::string& getExeName();
    void setExeName(const char* name);
    int registerImgOp(Ptr<ImgOp> imgOp);
    ImgOp* getImgOp(std::string &name);
    ImgOp* getImgOpIndex(int index);
private:
    Main();
    std::string exeName;
    std::vector<Ptr<ImgOp>> imgOpCollection;
}; // class Main

#endif // MAIN_H
