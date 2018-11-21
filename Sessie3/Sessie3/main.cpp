#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message    }"
        "{ i1 recht         |   | recht image           }"
        "{ i2 rot           |   | geroteerd image       }"
        "{ i3 template      |   | template image        }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("recht"));
    string i2(parser.get<string>("rot"));
    string i3(parser.get<string>("template"));

    if(i1.empty() || i2.empty() || i3.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images inlezen
    Mat rechtImage;
    Mat rotImage;
    Mat tempImage;

    rechtImage = imread(i1);
    rotImage = imread(i2);
    tempImage = imread(i3);

    if(rechtImage.empty() || rotImage.empty() || tempImage.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    //imshow("recht Image", rechtImage);
    //waitKey(0);
    //imshow("geroteerd Image", rotImage);
    //waitKey(0);
    //imshow("Template Image", tempImage);
    //waitKey(0);

    // afbeeldingen omzetten in grijswaarden
    Mat tempGray;
    cvtColor(tempImage, tempGray, COLOR_BGR2GRAY);

    Mat rechtGray;
    cvtColor(rechtImage, rechtGray, COLOR_BGR2GRAY);

    Mat rotGray;
    cvtColor(rotImage, rotGray, COLOR_BGR2GRAY);

    // matchtemplate uitvoeren
    Mat resultaat;

    matchTemplate(rechtGray, tempGray, resultaat, TM_CCORR);
    normalize( resultaat.clone(), resultaat, 0, 1, NORM_MINMAX, -1, Mat());

    // resultaat tonen
    imshow("resultaat", resultaat);
    waitKey(0);

    // Lokaal naar maxima zoeken
    // variablen voor minMaxLoc
    double minValue; double maxValue; Point minLoc; Point maxLoc;

    minMaxLoc(resultaat, &minValue, &maxValue, &minLoc, &maxLoc, Mat());

    Mat ImageMatched;
    rechtImage.copyTo(ImageMatched);
    rectangle( ImageMatched, maxLoc, Point(maxLoc.x+tempImage.cols, maxLoc.y+tempImage.rows), Scalar::all(0), 2, 8, 0 );

    imshow("image met template matching 1 match", ImageMatched);
    waitKey(0);

    Mat thresh;
    threshold(resultaat, thresh, 0.1, 255, THRESH_BINARY_INV);

    thresh=255*thresh;
    thresh.convertTo(thresh, CV_8UC1);

    imshow("Threshold", thresh);
    waitKey(0);

    Mat ImageMatched2;
    rechtImage.copyTo(ImageMatched2);

    vector<vector<Point>> contours;
    findContours(thresh.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    for(int i = 0; i < contours.size(); i++){
        Rect region = boundingRect(contours[i]);
        Mat huidig = resultaat(region);
        Point  maxLoc;
        minMaxLoc(huidig, NULL, NULL, NULL, &maxLoc, Mat());
        rectangle(ImageMatched2, Point(region.x, region.y), Point(region.x + tempImage.cols, region.y + tempImage.rows), Scalar::all(0), 2, 8, 0 );
    }

    imshow("image met template matching", ImageMatched2);
    waitKey(0);
}
//--i1=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie3/recht.jpg --i2=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie3/rot.jpg --i3=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie3/template.jpg
