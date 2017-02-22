// iofill.cpp
#include "main.h"

class ImgOpFill : public ImgOp {
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
    uchar rgbArg[3];
};

int iofillRegister(Main *main) {
    Ptr<ImgOpFill> io = Ptr<ImgOpFill>(new ImgOpFill);
    io->name = "fill";
    main->registerImgOp(io);
    return ERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

int ImgOpFill::reset()
{
    xArg=yArg=0;
    rgbArg[0]=0; // default to pink
    rgbArg[1]=255;
    rgbArg[2]=0;
    return ERR_OK;
}

int ImgOpFill::setImgSrc(Mat isrc)
{
    imgSrc = isrc;
    return ERR_OK;
}

std::string ImgOpFill::getHelp() {
    std::string help;
    help = " FLOOD FILL\n -o fill -p xy=<XSTART>,<YSTART>";
    return help;
}

int ImgOpFill::addArgs(std::string &params)
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
    
    return ERR_OK;
}

int ImgOpFill::run()
{
    // make sure we can handle the format
    if(!imgSrc.data)
        return ERR_NODATA;
    if(imgSrc.depth() != CV_8U)
        return ERR_IMAGE_FORMAT_NOT_SUPPORTED;
    int nchan = imgSrc.channels();
    if(nchan<3)
        return ERR_IMAGE_FORMAT_NOT_SUPPORTED;        

    //
    //
    //
    
    int r;
    int ncols = imgSrc.cols;
    int nrows = imgSrc.rows;
    int x,y,wx0,wy0,wx1,wy1;
    wx0 = 0, wy0 = 0;
    wx1 = ncols - 1; // including x1
    wy1 = nrows - 1; // including y1
    
    if((wx0>=wx1)||(wy0>=wy1))
        return ERR_DATA_ERROR;
    
    #define RANGEOK(_x_,_y_) (((_x_)>=wx0)&&((_x_)<=wx1)&&((_y_)>=wy0)&&((_y_)<=wy1))
    
    x = xArg, y = yArg; // initial values passed as args
    if(!RANGEOK(x,y))
        return ERR_OUT_OF_RANGE;
    #define PIX(_x_,_y_) imgSrc.row(_y_).data + (_x_)*nchan
    #define ISDSTCOLOR(_bgr_) (((_bgr_)[0]==dstbgr[0]) \
        && ((_bgr_)[1]==dstbgr[1]) && ((_bgr_)[2]==dstbgr[2]))
    #define ISSRCCOLOR(_bgr_) (((_bgr_)[0]==srcbgr[0]) \
        && ((_bgr_)[1]==srcbgr[1]) && ((_bgr_)[2]==srcbgr[2]))
    #define PUSH(_xl_,_xr_,_y_,_dy_) { \
        if( (((_y_)+(_dy_))>=wy0) && (((_y_)+(_dy_))<=wy1) ) { \
        Seg seg = {.xl=(_xl_),.xr=(_xr_),.y=(_y_),.dy=(_dy_)}; \
        stack.push_back(seg); } \
    }
    #define POP(_xl_,_xr_,_y_,_dy_) { \
        Seg seg = stack.back(); stack.pop_back(); \
        _xl_ = seg.xl; _xr_ = seg.xr; \
        _dy_ = seg.dy;_y_ = seg.y + seg.dy; \
    }
    #define SETCOLOR(_bgr_,_fr_) \
        (_bgr_)[0]=(_fr_)[0],(_bgr_)[1]=(_fr_)[1],(_bgr_)[2]=(_fr_)[2]
    
    // flood fill from Paul Heckbert's seed fill
    // https://github.com/erich666/GraphicsGems/blob/master/gems/SeedFill.c
    uchar srcbgr[3],dstbgr[3];
    uchar *bgr;
    dstbgr[0] = rgbArg[2]; // dst is color we are going to set
    dstbgr[1] = rgbArg[1];   
    dstbgr[2] = rgbArg[0];
   
    struct Seg {
        int xl, xr, y, dy;
    };
    
    int x1,x2,dy,l;
    
    std::vector<Seg> stack;
    
    bgr = PIX(x,y);
    if(ISDSTCOLOR(bgr))
        return ERR_OK;

    // src is original color we are starting fill from
    srcbgr[0]=bgr[0],srcbgr[1]=bgr[1],srcbgr[2]=bgr[2];

    PUSH(x,x,y,1);
    PUSH(x,x,y+1,-1);
    
    while(stack.size()) {
        POP(x1,x2,y,dy);
        
        for(x=x1;x>=wx0;x--) {
            bgr = PIX(x,y);
            if(!ISSRCCOLOR(bgr))
                break;
            SETCOLOR(bgr,dstbgr);
        } // for
        
        if(x>=x1) goto skip;
        l = x + 1;
        if(l<x1)
            PUSH(l,x-1,y,-dy);
        x = x1 + 1;
        do {
            for(;x<=wx1;x++) {
                bgr = PIX(x,y);
                if(!ISSRCCOLOR(bgr))
                    break;
                SETCOLOR(bgr,dstbgr);
            } // for
            PUSH(l,x-1,y,dy);
            if(x>x2+1)
                PUSH(x2+1,x-1,y,-dy);
        
        skip:
            for(x++;x<=x2;x++) {
                bgr = PIX(x,y);
                if(ISSRCCOLOR(bgr))
                    break;
            } // for
            l = x;
        } while(x<=x2);
    } // while
    
    return 0;
}

// EOF