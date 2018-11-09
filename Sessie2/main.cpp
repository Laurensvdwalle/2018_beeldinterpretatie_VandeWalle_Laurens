#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Global Variables
const int alpha_slider_max1 = 180;
const int alpha_slider_max2 = 255;
int alpha_slider1 = 0;
double h1;
int alpha_slider2 = 100;
double s1;
int alpha_slider3 = 0;
double v1;
int alpha_slider4 = 180;
double h2;
int alpha_slider5 = 255;
double s2;
int alpha_slider6 = 255;
double v2;

void on_trackbar( int, void* )
{
 h1 = (double) alpha_slider1;
 s1 = (double) alpha_slider2;
 v1 = (double) alpha_slider3;
 h2 = (double) alpha_slider4;
 s2 = (double) alpha_slider5;
 v2 = (double) alpha_slider6;

}


int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message    }"
        "{ i1 sign          |   | first sign            }"
        "{ i2 sign2         |   | second sign           }"
        "{ i3 sign3         |   | third sign            }"
        "{ i4 sign4         |   | fourth sign           }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("sign"));
    string i2(parser.get<string>("sign2"));
    string i3(parser.get<string>("sign3"));
    string i4(parser.get<string>("sign4"));

    if(i1.empty() || i2.empty() || i3.empty() || i4.empty())
    {
        parser.printMessage();
        return -1;
    }

// images inlezen
    Mat sign;
    Mat sign2;
    Mat sign3;
    Mat sign4;

    sign = imread(i1);
    sign2 = imread(i2);
    sign3 = imread(i3);
    sign4 = imread(i4);


    if(sign.empty() || sign2.empty() || sign3.empty() || sign4.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    imshow("Sign", sign);
    imshow("Sign2", sign2);
    imshow("Sign3", sign3);
    imshow("Sign4", sign4);
    waitKey(0);

    // inputimage maken, deze aanpassen voor andere figuur
    Mat input = sign;

    // segmenteren in BGR
    Mat mask;
    Mat signbgr;
    inRange(input, Scalar(0, 0, 160), Scalar(255, 255, 255), mask);  // masker aanmaken
    input.copyTo(signbgr, mask);                                     // masker op figuur toepassen
    imshow( "BGR segmentatie van input", signbgr );
    waitKey(0);

    // figuur omvormen naar hsv
    Mat hsv_input;
    cvtColor(input, hsv_input, COLOR_BGR2HSV);
    imshow("HSV image", hsv_input);
    waitKey(0);

    // window aanmaken
    namedWindow("image met trackbar", 1);

    // trackbars aanmaken
    createTrackbar( "h1", "image met trackbar", &alpha_slider1, alpha_slider_max1, on_trackbar );
    createTrackbar( "s1", "image met trackbar", &alpha_slider2, alpha_slider_max2, on_trackbar );
    createTrackbar( "v1", "image met trackbar", &alpha_slider3, alpha_slider_max2, on_trackbar );
    createTrackbar( "h2", "image met trackbar", &alpha_slider4, alpha_slider_max1, on_trackbar );
    createTrackbar( "s2", "image met trackbar", &alpha_slider5, alpha_slider_max2, on_trackbar );
    createTrackbar( "v2", "image met trackbar", &alpha_slider6, alpha_slider_max2, on_trackbar );

    // segmenteren in HSV colorspace
    Mat signhsv;
    Mat hsvmask;
    Mat hsvmask1;
    Mat hsvmask2;
    inRange(hsv_input, cv::Scalar(0, 100, 0), cv::Scalar(h1, s1, v1), hsvmask1);      // eerste masker aanmaken
    inRange(hsv_input, cv::Scalar(h2, s2, v2), cv::Scalar(180, 255, 255), hsvmask2);  // tweede masker aanmaken
    hsvmask = hsvmask1 | hsvmask2;                                                    // totaal masker aanmaken

    // via erode en dilate heel het verkeersbord selecteren
    dilate(hsvmask.clone(), hsvmask, Mat(), Point(-1, -1), 10);
    erode(hsvmask.clone(), hsvmask, Mat(), Point(-1, -1), 10);

    // masker op figuur toepassen
    input.copyTo(signhsv, hsvmask);
    imshow( "image met trackbar", signhsv );
    waitKey(0);


    return 0;
}
// --i1=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie2/sign.jpg --i2=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie2/sign2.jpg --i3=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie2/sign3.jpg --i4=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie2/sign4.jpg
