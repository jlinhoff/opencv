// file: main.cpp -- for simple OpenCV experiments
#include <stdio.h>
#include <string.h>
#include "main.h"

////////////////////////////////////////////////////////////////////////////////
// MAIN

Main::Main()
{
} // Main::Main()

Main* Main::instance()
{
    static Ptr<Main> singleton = 0;
    if(!singleton)
        singleton = Ptr<Main>(new Main());
    return singleton;
} // Main::instance()

void Main::setExeName(const char* name)
{
    const char* s1;
    for(;;) { // find last slash
        if(!(s1 = strchr(name, '/')))
            s1 = strchr(name, '\\');
        if(!s1)
            break;
        name = s1 + 1; // skip slash
    } // for
    exeName = std::string(name);
}

const std::string& Main::getExeName()
{
    return exeName;
}

int Main::registerImgOp(Ptr<ImgOp> imgOp) {
    imgOpCollection.push_back(imgOp);
    return 0;
}

ImgOp* Main::getImgOp(std::string &name) {
    for(int i=0;i<imgOpCollection.size();i++) {
        if(imgOpCollection[i]->name == name)
            return imgOpCollection[i]; // return raw ptr (not shared)
    } // for
    return 0;
}

ImgOp* Main::getImgOpIndex(int i) {
    if(i<0 || i>=imgOpCollection.size())
        return 0;
    return imgOpCollection[i];
}

////////////////////////////////////////////////////////////////////////////////

std::string ImgOp::getHelp() {
    return std::string("<no help>");
}

int ImgOp::setImgSrc(Mat imgSrc)
{
    return 0;
}

int ImgOp::addArgs(std::string &params)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

extern int iorectRegister(Main *main);
static int mainRegisterImgOps(Main *main) {
    int fail=0,r;
    
    // register all the image operations
    
    if((r=iorectRegister(main)<0))
        fail=r;

    return fail;
} // mainRegisterImgOps()

////////////////////////////////////////////////////////////////////////////////

static void help()
{
    Main* main = Main::instance();
    printf("Usage: %s [OPTIONS] pathtoimage\n", main->getExeName().c_str());
    printf(" OPTIONS: -? --help    display this help\n");
    ImgOp *imgop;
    for(int i=0;;i++) {
        imgop = main->getImgOpIndex(i);
        if(!imgop)
            break;
        printf("%s\n",imgop->getHelp().c_str());
    } // for
} // help()

int main(int argc, char* argv[])
{
    int r = 0;
    const char* fname = 0;
    const char* iopname = 0;
    Mat image;
    ImgOp *imgop = 0;
    const char* s1;
    Main* main = Main::instance();
    std::vector<std::string> argCollection;

    //
    // REGISTER IMAGE OPERATIONS
    //
    
    if((r=mainRegisterImgOps(main))<0)
    {
        printf("* error registering img ops %d\n",r);
        ret(-1);
    }
    
    //
    // SCAN ARGS
    //

    main->setExeName(*argv++), argc--;
    
    help();
    
    for(; argc > 0;) {
        s1 = *argv++, argc--;
        if(*s1 == '-') {
            switch(s1[1]) {
            case '?':
                help();
                ret(ERR_OK);
            case '-':    // long option
                s1 += 2; // skip --
                if(!strcmp("help", s1)) {
                    help();
                    ret(ERR_OK);
                }
                break;
            case 'o': // op
                if(argc)
                    iopname=*argv++,argc--;
                break;
            case 'p': // params
                if(argc) {
                    std::string arg(*argv);
                    argv++,argc--;
                    argCollection.push_back(arg);
                }
                    
            } // switch
        } else {
            if(!fname) {
                fname = s1;
            } else {
                printf("* fname already set\n");
                help();
                ret(ERR_SYNTAX);
            }
        }
    } // for

    //
    // CHECK ARGS
    //

    if(!fname) {
        printf("* must set fname\n");
        help();
        ret(ERR_MISSING_FNAME);
    }
    
    if(iopname) {
        std::string name(iopname);
        imgop = main->getImgOp(name);
        if(!imgop) {
            printf("* image op '%s' not found\n",iopname);
            ret(ERR_UNKNOWN_OPERATION);
        }
        imgop->reset();
        
        // add all the arguments
        for(int i=0;i<argCollection.size();i++)
            imgop->addArgs(argCollection[i]);
    }

    //
    //
    //

    image = imread(fname, 1);

    if(!image.data) {
        printf("No image data \n");
        ret(ERR_NODATA);
    }
    
    if(image.depth() != CV_8U) {
        printf("* image format not supported\n");
        ret(ERR_IMAGE_FORMAT_NOT_SUPPORTED);
    }

    namedWindow(main->getExeName(), WINDOW_AUTOSIZE);
    
    if(imgop) {
        
        //
        // APPLY IMAGE OPERATION
        //
        
        imgop->setImgSrc(image);
        imgop->run();
    }

    imshow(main->getExeName(), image);
    
    //
    // FINAL
    //
    
    r = ERR_OK;
FINAL:
    waitKey(0); // supposed to wait N milliseconds -- doesn't seem correct
    return r;
} // main()

// EOF
