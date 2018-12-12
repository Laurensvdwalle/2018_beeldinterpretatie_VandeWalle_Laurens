#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

char window_name[20] = "faces";

void violaJones(Mat frame);
void LBP(Mat frame);

int main(int argc, const char** argv)
{
    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message    }"
        "{ f faces          |   | video faces.mp4       }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string f(parser.get<string>("faces"));

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

    namedWindow( window_name, 1);

    while( true )
    {
        capture >> videoInFrame;

        if(videoInFrame.empty())
            break;

        violaJones(videoInFrame);
        LBP(videoInFrame);

        waitKey(20);
    }

    return 0;
}

void violaJones(Mat frame)
{
    // convert frame to gray values
    Mat frame_gray;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );

    // load face cascade
    CascadeClassifier face_cascade;
    face_cascade.load( "haarcascade_frontalface_alt.xml" );

    // detect faces
    std::vector<Rect> faces;
    std::vector<int> level_weights;
    face_cascade.detectMultiScale( frame_gray, faces, level_weights);

    // draw circles on faces
    for( unsigned i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        ellipse( frame, center, Size( faces[i].width*0.7, faces[i].height*0.7), 0, 0, 360, Scalar( 255, 255, 255 ), 4, 8, 0 );
        putText(frame, std::to_string(level_weights[i]), Point(faces[i].x+10, faces[i].y+10), 1, 1, Scalar(0,255,0));
    }

    imshow(window_name, frame);
}

void LBP(Mat frame)
{
    // convert frame to gray values
    Mat frame_gray;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );

    // load face cascade
    CascadeClassifier face_cascade;
    face_cascade.load( "lbpcascade_frontalface_improved.xml" );

    // detect faces
    std::vector<Rect> faces;
    std::vector<int> level_weights;
    face_cascade.detectMultiScale( frame_gray, faces, level_weights);

    // draw circles on faces
    for( unsigned i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        ellipse( frame, center, Size( faces[i].width*0.7, faces[i].height*0.7), 0, 0, 360, Scalar( 255, 255, 255 ), 4, 8, 0 );
        putText(frame, std::to_string(level_weights[i]), Point(faces[i].x+10, faces[i].y+10), 1, 1, Scalar(255,0,0));
    }

    imshow(window_name, frame);
}
// --f=/home/laurens/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie6/faces.mp4
