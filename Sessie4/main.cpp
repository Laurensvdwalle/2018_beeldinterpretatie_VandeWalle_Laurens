#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat KeyPointDet(Mat image, int soort);
Mat BruteForceMatch(Mat image, Mat object);

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

    //imshow("fitness Image ORB", fitImage_orb);
    //imshow("fitness Object ORB", fitObject_orb);
    //imshow("kinder Image ORB", kinImage_orb);
    //imshow("kinder Object ORB", kinObject_orb);
    //waitKey(0);

    // BRISK keypoints
    Mat fitImage_brisk;
    Mat fitObject_brisk;
    Mat kinImage_brisk;
    Mat kinObject_brisk;

    fitImage_brisk = KeyPointDet(fitImage, 2);
    fitObject_brisk = KeyPointDet(fitObject, 2);
    kinImage_brisk = KeyPointDet(kinImage, 2);
    kinObject_brisk = KeyPointDet(kinObject, 2);

    //imshow("fitness Image BRISK", fitImage_brisk);
    //imshow("fitness Object BRISK", fitObject_brisk);
    //imshow("kinder Image BRISK", kinImage_brisk);
    //imshow("kinder Object BRISK", kinObject_brisk);
    //waitKey(0);

    // AKAZE keypoints
    Mat fitImage_akaze;
    Mat fitObject_akaze;
    Mat kinImage_akaze;
    Mat kinObject_akaze;

    fitImage_akaze = KeyPointDet(fitImage, 2);
    fitObject_akaze = KeyPointDet(fitObject, 2);
    kinImage_akaze = KeyPointDet(kinImage, 2);
    kinObject_akaze = KeyPointDet(kinObject, 2);

    //imshow("fitness Image AKAZE", fitImage_akaze);
    //imshow("fitness Object AKAZE", fitObject_akaze);
    //imshow("kinder Image AKAZE", kinImage_akaze);
    //imshow("kinder Object AKAZE", kinObject_akaze);
    //waitKey(0);

    // We kiezen ORB keypoints
    // brute force matching
    Mat fitMatchImage = BruteForceMatch(fitImage, fitObject);
    Mat kinMatchImage = BruteForceMatch(kinImage, kinObject);

    imshow("matches fitness image", fitMatchImage);
    imshow("matches kinder image", kinMatchImage);
    waitKey(0);

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

// uses brute force matching and applies RANSAC
Mat BruteForceMatch(Mat image, Mat object)
{
    // keypoints
    Ptr<ORB> detector;
    detector = ORB::create();

    vector<KeyPoint> image_keypoints;
    vector<KeyPoint> object_keypoints;

    detector->detect(image, image_keypoints);
    detector->detect(object, object_keypoints);

    Mat image_orb;
    Mat object_orb;

    image_orb = KeyPointDet(image, 1);
    object_orb = KeyPointDet(object, 1);

    // keypointdescription
    Mat image_descr;
    Mat object_descr;

    Ptr<DescriptorExtractor> descriptor;
    descriptor=ORB::create();

    descriptor->compute(image, image_keypoints, image_descr);
    descriptor->compute(object, object_keypoints, object_descr);

    // brute force matching
    BFMatcher matcher(NORM_L2);
    std::vector<DMatch> matches;

    matcher.match(object_descr, image_descr, matches);

    // ransac
    // min_dist op hoog getal zetten
    double min_dist = 500000;
    double max_dist = 0;

    for(int i=0 ; i<matches.size() ; i++)
    {
        if(matches[i].distance>max_dist)
        {
            max_dist=matches[i].distance;
        }
        if(matches[i].distance<min_dist)
        {
            min_dist=matches[i].distance;
        }
    }

    // matches die korter zijn dan 3*min_match er uithalen
    std::vector<DMatch> good_matches;
    for(int i=0 ; i<matches.size() ; i++)
    {
        if(matches[i].distance<(3*min_dist))
        {
            good_matches.push_back(matches[i]);
        }
    }


    std::vector<Point2f> image_good, object_good;
    for(int i=0 ; i<good_matches.size() ; i++)
    {
        object_good.push_back(object_keypoints[good_matches[i].queryIdx].pt);
        image_good.push_back(image_keypoints[good_matches[i].trainIdx].pt);
    }

    // homography toepassen
    //Mat h = findHomography(object_good, image_good, RANSAC);

    //vector<Point2f> object_hoeken(4);
    //object_hoeken[0] = cvPoint(0,0); object_hoeken[1] = cvPoint(object.cols,0);
    //object_hoeken[2] = cvPoint(object.cols, object.rows);
    //object_hoeken[3] = cvPoint(0, object.rows);

    //vector<Point2f> image_hoeken(4);
    //perspectiveTransform(object_hoeken, image_hoeken, h);

    Mat matchImage;

    //line(matchImage, image_hoeken[0] + Point2f(object.cols, 0), image_hoeken[1] + Point2f(object.cols, 0), Scalar::all(255), 5);
    //line(matchImage, image_hoeken[1] + Point2f(object.cols, 0), image_hoeken[2] + Point2f(object.cols, 0), Scalar::all(255), 5);
    //line(matchImage, image_hoeken[2] + Point2f(object.cols, 0), image_hoeken[3] + Point2f(object.cols, 0), Scalar::all(255), 5);
    //line(matchImage, image_hoeken[3] + Point2f(object.cols, 0), image_hoeken[0] + Point2f(object.cols, 0), Scalar::all(255), 5);

    // draw matchlines
    drawMatches(object, object_keypoints, image, image_keypoints, good_matches, matchImage);

    return matchImage;
}
//--i1=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/fitness_image.png --i2=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/fitness_object.png --i3=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/kinderbueno_image.png --i4=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie4/kinderbueno_object.png
