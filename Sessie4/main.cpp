#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat KeyPointDet(Mat image, int soort);

int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?           |   | print this message    }"
        "{ i1 fitness_image         |   | recht image           }"
        "{ i2 fitness_object        |   | geroteerd image       }"
        "{ i3 kinderbueno_image     |   | template image        }"
        "{ i4 kinderbueno_object    |   | template image        }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("fitness_image"));
    string i2(parser.get<string>("fitness_object"));
    string i3(parser.get<string>("kinderbueno_image"));
    string i4(parser.get<string>("kinderbueno_object"));

    if(i1.empty() || i2.empty() || i3.empty() || i4.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images inlezen
    Mat fitImage;
    Mat fitObject;
    Mat kinImage;
    Mat kinObject;

    fitImage = imread(i1);
    fitObject = imread(i2);
    kinImage = imread(i3);
    kinObject = imread(i4);

    if(fitImage.empty() || fitObject.empty() || kinImage.empty() || kinObject.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    // imshow("fitness Image", fitImage);
    // waitKey(0);
    // imshow("fitness Object", fitObject);
    // waitKey(0);
    // imshow("kinder Image", kinImage);
    // waitKey(0);
    // imshow("kinder Object", kinObject);
    // waitKey(0);

    // ORB keypoints
    Mat fitImage_orb;
    Mat fitObject_orb;
    Mat kinImage_orb;
    Mat kinObject_orb;

    fitImage_orb = KeyPointDet(fitImage, 1);
    fitObject_orb = KeyPointDet(fitObject, 1);
    kinImage_orb = KeyPointDet(kinImage, 1);
    kinObject_orb = KeyPointDet(kinObject, 1);

    imshow("fitness Image ORB", fitImage_orb);
    imshow("fitness Object ORB", fitObject_orb);
    imshow("kinder Image ORB", kinImage_orb);
    imshow("kinder Object ORB", kinObject_orb);
    waitKey(0);

    // BRISK keypoints
    Mat fitImage_brisk;
    Mat fitObject_brisk;
    Mat kinImage_brisk;
    Mat kinObject_brisk;

    fitImage_brisk = KeyPointDet(fitImage, 2);
    fitObject_brisk = KeyPointDet(fitObject, 2);
    kinImage_brisk = KeyPointDet(kinImage, 2);
    kinObject_brisk = KeyPointDet(kinObject, 2);

    imshow("fitness Image BRISK", fitImage_brisk);
    imshow("fitness Object BRISK", fitObject_brisk);
    imshow("kinder Image BRISK", kinImage_brisk);
    imshow("kinder Object BRISK", kinObject_brisk);
    waitKey(0);

    // AKAZE keypoints
    Mat fitImage_akaze;
    Mat fitObject_akaze;
    Mat kinImage_akaze;
    Mat kinObject_akaze;

    fitImage_akaze = KeyPointDet(fitImage, 2);
    fitObject_akaze = KeyPointDet(fitObject, 2);
    kinImage_akaze = KeyPointDet(kinImage, 2);
    kinObject_akaze = KeyPointDet(kinObject, 2);

    imshow("fitness Image AKAZE", fitImage_akaze);
    imshow("fitness Object AKAZE", fitObject_akaze);
    imshow("kinder Image AKAZE", kinImage_akaze);
    imshow("kinder Object AKAZE", kinObject_akaze);
    waitKey(0);

    // We kiezen ORB keypoints
}

Mat KeyPointDet(Mat image, int soort)
{
    Mat image_kp;
    image.copyTo(image_kp);

    Ptr<FeatureDetector> detector;

    if(soort == 1)
    {
        detector = ORB::create();
    }
    if(soort == 2)
    {
        detector = BRISK::create();
    }
    else
    {
        detector = AKAZE::create();
    }

    vector<KeyPoint> keypoints;

    detector->detect(image, keypoints);
    drawKeypoints(image, keypoints, image_kp, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    return image_kp;
}
//--i1=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/fitness_image.png --i2=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/fitness_object.png --i3=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/kinderbueno_image.png --i4=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/kinderbueno_object.png
