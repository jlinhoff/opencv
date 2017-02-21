// file: main.cpp -- for simple OpenCV experiments
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace cv;

class Main
{
    public:
        static Main *instance();
        const char *getExeName() {return exeName;}
        void setExeName(const char *name);
    private:
        Main();
        const char *exeName;
}; // class Main

Main::Main() : exeName(0)
{
} // Main::Main()

Main* Main::instance() {
    static Ptr<Main> singleton=0;
    if(!singleton)
        singleton = Ptr<Main>(new Main());
    return singleton;
}

void Main::setExeName(const char *name) {
    const char *s1;
    for(;;) { // find last slash
        if(!(s1=strchr(name,'/')))
            s1=strchr(name,'\\');
        if(!s1)
            break;
        name=s1+1;
    } // for
    exeName=name;
}

static void help() {
    Main *main = Main::instance();
    printf("Usage: %s [OPTIONS] pathtoimage\n",main->getExeName());
    printf(" OPTIONS: -? --help    display this help\n");
} // help()

int main(int argc, char* argv[])
{
    int r = 0;
    const char *fname=0;
    Mat image;
    const char *s1;
    Main *main = Main::instance();
    
    //
    // SCAN ARGS
    //
    
    main->setExeName(*argv++),argc--;
    for(;argc>0;) {
        s1 = *argv++,argc--;
        if(*s1=='-') {
            switch(s1[1]) {
            case '?':
                help();
                r=0;
                goto BAIL;
            case '-': // long option
                s1+=2; // skip --
                if(!strcmp("help",s1)) {
                    help();
                    r=0; goto BAIL;
                }
                break;
            } // switch
        } else {
            if(!fname) {
                fname=s1;
            } else {
                printf("* fname already set\n");
                help();
                r=-1;
                goto BAIL;
            }
        }
    } // for
    
    //
    // CHECK ARGS
    //

    if(!fname) {
        printf("* must set fname\n");
        help();
        r = -1;
        goto BAIL;
    }
    
    //
    //
    //

    image = imread(fname, 1);

    if(!image.data) {
        printf("No image data \n");
        r = -1;
        goto BAIL;
    }
    namedWindow(main->getExeName(), WINDOW_AUTOSIZE);
    imshow(main->getExeName(), image);

BAIL:
    waitKey(500); // supposed to wait N milliseconds -- doesn't seem correct
    return r;
} // main()

// EOF
