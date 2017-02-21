// iorect.cpp
#include "main.h"

class xyz {
public:
    xyz(){}
};

class ImgOpRect : public ImgOp {
public:
    // set source image, return <0 on error
    int setImgSrc(Mat *imgSrc) override;
    
    // run the operation, return <0 on error
    int run() override;
protected:
    Mat *imgSrc = 0;
};

int iorectRegister(Main *main) {
    Ptr<ImgOpRect> iorect = Ptr<ImgOpRect>(new ImgOpRect);
    main->registerImgOp(iorect);
    return 0;
}

int ImgOpRect::setImgSrc(Mat *imgSrc)
{
    return 0;
}

int ImgOpRect::run()
{
    return 0;
}

// EOF