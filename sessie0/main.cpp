#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ? |      | print this message   }"
        "{ image_gray     |      | image for compare   }"
        "{ image_color    |      | image for compare   }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
    }

    // inputparameters
    string image_gray_location(parser.get<string>("image_gray"));
    string image_color_location(parser.get<string>("image_color"));
    if(image_color_location.empty() || image_gray_location.empty())
    {
        parser.printMessage();
        return -1;
    }

    // image inlezen en tonen
    Mat image;
    image = imread(image_gray_location);
    if(image.empty())
    {
        parser.printMessage();
        return -1;
    }
    imshow("EAVISE Logo - grayscale", image);
    waitKey(0);

    return 0;
}
