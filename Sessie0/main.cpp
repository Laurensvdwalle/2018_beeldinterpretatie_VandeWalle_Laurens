#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ? |      | print this message       }"
        "{ i1 image_gray  |      | gray image for compare   }"
        "{ i2 image_color |      | color image for compare  }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("image_gray"));
    string i2(parser.get<string>("image_color"));

    if(i1.empty() || i2.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images inlezen
    Mat grayImage;
    Mat colorImage;

    grayImage = imread(i1);
    colorImage = imread(i2);

    if(grayImage.empty())
    {
        parser.printMessage();
        return -1;
    }

    if(colorImage.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    imshow("EAVISE Logo - grayscale", grayImage);
    waitKey(0);
    imshow("EAVISE Logo - color", colorImage);
    waitKey(0);

    // kleurafbeelding opslpitsen in kleurkanalen en tonen
    vector<Mat> kleurkanalen;
    split(colorImage, kleurkanalen);
    imshow("Blauw", kleurkanalen[0]);
    waitKey(0);
    imshow("Groen", kleurkanalen[1]);
    waitKey(0);
    imshow("Rood", kleurkanalen[2]);
    waitKey(0);

    // kleurafbeelding omzetten in grijswaardenbeeld
    Mat kleurgrijs;
    cvtColor(colorImage,kleurgrijs,COLOR_BGR2GRAY);
    imshow("kleur in grijswaarden", kleurgrijs );
    waitKey(0);

    // Waardes printen naar commandline
    for(int i=0; i<kleurgrijs.rows; i++){
        for(int j=0; j<kleurgrijs.cols; j++){
            cout << (int)kleurgrijs.at<uchar>(i,j)<< endl;
        }
    }
    waitKey(0);

    Mat canvas = Mat::zeros( 300, 300, CV_8UC3 );
    rectangle( canvas, Point( 20, 20), Point( 200, 200), Scalar(255, 255, 100), 5 );
    circle( canvas, Point( 100, 200 ), 32.0, Scalar( 255, 255, 255 ), 3 );
    line( canvas, Point( 50, 50 ), Point( 300, 300), Scalar( 100, 100, 100 ), 10 );
    imshow("canvas", canvas);
    waitKey(0);
    return 0;
}
