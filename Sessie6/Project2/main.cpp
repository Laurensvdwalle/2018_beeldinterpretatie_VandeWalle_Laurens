#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

char window_name[20] = "people";

void HOGSVM(Mat frame, HOGDescriptor hog);

int main(int argc, const char** argv)
{
    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message    }"
        "{ f people         |   | video faces.mp4       }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string f(parser.get<string>("people"));

    if(f.empty())
    {
        parser.printMessage();
        return -1;
    }

    // video inlezen
    VideoCapture capture(f);
    Mat videoInFrame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    std::vector<Point> punten;
    namedWindow( window_name, 1);

    while( true )
    {
        capture >> videoInFrame;

        if(videoInFrame.empty())
            break;

        // convert frame to gray values
        Mat frame_gray;
        cvtColor( videoInFrame, frame_gray, CV_BGR2GRAY );

        // detect people
        std::vector<Rect> people;
        hog.detectMultiScale( frame_gray, people );

        // draw circles on faces
        for( unsigned i = 0; i < people.size(); i++ )
        {
            Point center(people[i].x + 0.5*people[i].width, people[i].y + 0.5*people[i].height);
            punten.push_back(center);

            Point p1( people[i].x, people[i].y );
            Point p2( people[i].x + people[i].width, people[i].y + people[i].height );

            rectangle(videoInFrame, p1, p2, Scalar(0, 255, 0));
        }

        polylines(videoInFrame, punten);

        imshow(window_name, videoInFrame);

        waitKey(20);
    }

    return 0;
}
// --f=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie6/Project2/people.mp4
