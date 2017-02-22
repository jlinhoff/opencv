// iorect.cpp
#include "main.h"

class ImgOpRect : public ImgOp {
public:

    // reset to defaults, return ERR_ code
    int reset() override;
    
    // get help for the operation
    std::string getHelp() override;
    
    // set source image, return ERR_ code
    int setImgSrc(Mat imgSrc) override;
    
    // add paramters
    int addArgs(std::string &params);
    
    // run the operation, return ERR_ code
    int run() override;
    
protected:
    Mat imgSrc;
    int xArg;
    int yArg;
    int wArg;
    int hArg;
};

int iorectRegister(Main *main) {
    Ptr<ImgOpRect> io = Ptr<ImgOpRect>(new ImgOpRect);
    io->name = "rect";
    main->registerImgOp(io);
    return ERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

int ImgOpRect::reset()
{
    xArg=yArg=wArg=hArg=0;
    return ERR_OK;
}

int ImgOpRect::setImgSrc(Mat isrc)
{
    imgSrc = isrc;
    return ERR_OK;
}

std::string ImgOpRect::getHelp() {
    std::string help;
    help = " FILL RECT\n -o rect -p xy=<XSTART>,<YSTART> -p wh=<WIDTH>,<HEIGHT>";
    return help;
}

int ImgOpRect::addArgs(std::string &params)
{
    int i = params.find('=');
    if(i<=0)
        return ERR_SYNTAX;
    const char *s1 = params.c_str()+i+1; // string after =
    char *s2;
    if(!params.compare(0,i,"xy")) {
        xArg=strtol(s1,&s2,0);
        if(*s2) s2++; // skip any delim
        yArg=strtol(s2,0,0);
    }
    else if(!params.compare(0,i,"wh")) {
        wArg=strtol(s1,&s2,0);
        if(*s2) s2++; // skip any delim
        hArg=strtol(s2,0,0);
    }
    
    return ERR_OK;
}

int ImgOpRect::run()
{
    // make sure we can handle the format
    if(!imgSrc.data)
        return ERR_NODATA;
    if(imgSrc.depth() != CV_8U)
        return ERR_IMAGE_FORMAT_NOT_SUPPORTED;
    int nchan = imgSrc.channels();
    if(nchan<3)
        return ERR_IMAGE_FORMAT_NOT_SUPPORTED;        

    int r;
    int ncols = imgSrc.cols;
    int nrows = imgSrc.rows;
    int x,y,x0,y0,x1,y1;
    x0 = xArg; x1 = x0 + wArg;
    y0 = yArg; y1 = y0 + hArg;
    x0 = CLAMP(x0,0,ncols);
    y0 = CLAMP(y0,0,nrows);
    x1 = CLAMP(x1,x0,ncols);
    y1 = CLAMP(y1,y0,nrows);
    if((x0>=x1)||(y0>=y1))
        ret(ERR_OK);

    for(y = y0; y<y1;y++) {
        uchar* p = imgSrc.row(y).data; // BGR
        p += x0*nchan;
        for(int x=x0;x<x1;x++,p+=nchan) {
            p[0]=0;     // B
            p[1]=255;   // G
            p[2]=255;   // R
        } // for
    } // for
   
   r=ERR_OK;
FINAL:
    return r;
}

// EOF