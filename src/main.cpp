// file: main.cpp -- for simple OpenCV experiments
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{
    int r = 0;
    Mat image;
    printf("Hello Display Image\n");
    if(argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        r = -1;
        goto BAIL;
    }

    image = imread(argv[1], 1);

    if(!image.data) {
        printf("No image data \n");
        r = -1;
        goto BAIL;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);

BAIL:
    waitKey(0);
    return r;
}
