#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>

using namespace std;
using namespace cv;

// window names
char video_window[30] = "Donkey Kong - Arcade";
char statistics_window[30] = "Statistics";

// primitives
Mat createStatFrame(double time_total, double time_ladder, int hammers);
int track_Mario(Mat frame, Mat frame_gray, Mat temp, Mat temp2, Mat ladd, Mat ladd2);
Point match(Mat frame, Mat temp);
int countHammers(Mat frame_gray, Mat hamm);

// booleans
bool was_jumping = false;

vector<Point> locations;

int main(int argc, const char** argv)
{
    // CommandlineParser
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message            }"
        "{ dk donkeyKong    |   | video Donkey Kong - Arcade    }"
        "{ tmp template     |   | Mario template                }"
        "{ lad ladder       |   | Mario on ladder               }"
        "{ ham hammer       |   | Hammer template               }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputvideo
    string dk(parser.get<string>("donkeyKong"));

    // templates for mario
    string tmp(parser.get<string>("template"));
    string lad(parser.get<string>("ladder"));
    string ham(parser.get<string>("hammer"));

    if(dk.empty() || tmp.empty() || lad.empty() || ham.empty())
    {
        parser.printMessage();
        return -1;
    }

    // read templates
    Mat temp; Mat temp2; Mat ladd; Mat ladd2; Mat hamm;
    temp = imread(tmp);
    ladd = imread(lad);
    hamm = imread(ham);
    cvtColor(temp, temp, CV_BGR2GRAY);
    cvtColor(ladd, ladd, CV_BGR2GRAY);
    cvtColor(hamm, hamm, CV_BGR2GRAY);
    flip(temp, temp2, +1);
    flip(ladd, ladd2, +1);

    // read video
    VideoCapture capture(dk);
    Mat frame;                                          // Mat for videoframe
    Mat stat = Mat::zeros(Size(350, 320), CV_64FC1);    // Mat for statistics frame

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    double fps = capture.get(CAP_PROP_FPS);

    namedWindow( video_window, 1);
    namedWindow( statistics_window, 1);

    // initialize some parameters
    double time_total = 0;
    double time_ladder = 0;
    int hammers = 0;
    int hammers_begin = -1;
    int hammers_present = 0;


    while( true )
    {
        capture >> frame;

        if(frame.empty())
            break;

        // grayscale frame
        Mat frame_gray;
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        int i = track_Mario(frame, frame_gray, temp, temp2, ladd, ladd2);

        // total time will always be increased
        time_total += 1.0/fps;

        if(i == 2)
        {
            time_ladder += 1.0/fps;
        }

        hammers_present = countHammers(frame_gray, hamm);

        if(hammers_begin = -1)
        {
            hammers_begin = hammers_present;
        }

        hammers = hammers_begin-hammers_present;

        stat = createStatFrame(time_total, time_ladder, hammers);

        polylines(frame, locations, false, Scalar(255, 255, 255));

        imshow(statistics_window, stat);
        imshow(video_window, frame);

        waitKey(20);
    }

    return 0;
}

Mat createStatFrame(double time_total, double time_ladder, int hammers)
{
    // make stat a black image again
    Mat stat = Mat::zeros(Size(350, 320), CV_64FC1);

    // total time
    int time_total_int = (int) time_total;
    stringstream ss;
    ss << time_total_int;
    string toprint = ss.str();

    putText(stat, "Total time elapsed: ", cvPoint(10, 30), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(stat, toprint + " second(s)", cvPoint(220, 30), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);


    // ladder time
    int time_ladder_int = (int) time_ladder;
    ss.str("");
    ss.clear();
    ss << time_ladder_int;
    toprint = ss.str();

    putText(stat, "Time on ladders: ", cvPoint(10, 80), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(stat, toprint + " second(s)", cvPoint(220, 80), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);

    // hammers
    ss.str("");
    ss.clear();
    ss << hammers;
    toprint = ss.str();

    putText(stat, "Hammers used: ", cvPoint(10, 130), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(stat, toprint + " hammer(s)", cvPoint(220, 130), FONT_HERSHEY_COMPLEX_SMALL,
        0.8, cvScalar(200, 200, 250), 1, CV_AA);

    return stat;
}

// tracks mario and returns his action: 0 = nothing special (standing r walking), 1 = jumping, 2 = on ladder an -1 means we can't find Mario;
int track_Mario(Mat frame, Mat frame_gray, Mat temp, Mat temp2, Mat ladd, Mat ladd2)
{
    // try to find mario's face
    Point loc = match(frame_gray, temp);
    Point loc2 = match(frame_gray, temp2);

    // try to find mario on a ladder
    Point loc3 = match(frame_gray, ladd);
    Point loc4 = match(frame_gray, ladd2);

    Point locfinal;

    int value = 1;

    if ( loc.x != -1 && loc.y != -1)
    {
        locfinal = loc;
    }
    else
    {
        if ( loc2.x != -1 && loc2.y != -1)
        {
            locfinal = loc2;
        }
        else
        {
            value = 2;
            if ( loc3.x != -1 && loc3.y != -1)
            {
                locfinal = loc3;
            }
            else
            {
                if ( loc4.x != -1 && loc.y != -1)
                {
                    locfinal = loc4;
                }
                else
                {
                    return -1;
                }
            }
        }
    }

    locations.push_back(Point(locfinal.x + 10, locfinal.y + 10));
    Mat imageMatched;
    frame.copyTo(imageMatched);
    rectangle( imageMatched, Point(locfinal.x - 5, locfinal.y), Point(locfinal.x + 25, locfinal.y + 25), Scalar(255, 255, 255), 2, 8, 0 );
    imshow("", imageMatched);

    return value;

}

Point match(Mat frame, Mat temp)
{
    // create result
    Mat result;

    // match template with frame
    matchTemplate(frame, temp, result, TM_CCORR_NORMED);
    //normalize(result.clone(), result, 0, 1, NORM_MINMAX, -1, Mat());

    // find local minimum and maximum
    double minValue; double maxValue; Point minLoc; Point maxLoc;
    minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc, Mat());

    if( maxValue < 0.9)
    {
        Point nomatch;
        nomatch.x = -1;
        nomatch.y = -1;
        return nomatch;
    }

    return maxLoc;
}

int countHammers(Mat frame_gray, Mat hamm)
{
    double minValue; double maxValue;
    double thresh = 0.9;

    Mat result;

    matchTemplate(frame_gray, hamm, result, TM_CCORR_NORMED);
    //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    minMaxLoc(result, &minValue, &maxValue);

    Mat match_result_thresh = Mat::zeros(Size(frame_gray.cols, frame_gray.rows), CV_32FC1);
    inRange(result, maxValue * thresh, maxValue, match_result_thresh);

    match_result_thresh.convertTo(match_result_thresh, CV_8UC1);

    vector<vector<Point>> contouren;
    findContours(match_result_thresh, contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    return contouren.size();
}
