#include <iostream>
#include <opencv2/opencv.hpp>

#define GOED 0
#define SLECHT 1

using namespace std;
using namespace cv;

char window_name[20] = "Aarbeien";
vector<Point> punten[2];
int welkePunten;

static void onMouse(int event, int x, int y, int flags, void* userdata);

int main(int argc, const char** argv)
{
    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?   |   | print this message        }"
        "{ i1 eerste        |   | eerste strawberry image   }"
        "{ i2 tweede        |   | tweede strawberry image   }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("eerste"));
    string i2(parser.get<string>("tweede"));

    if(i1.empty() || i2.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images inlezen
    Mat strawIn1;
    Mat strawIn2;

    strawIn1 = imread(i1);
    strawIn2 = imread(i2);

    if(strawIn1.empty() || strawIn2.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    imshow("strawberry 1", strawIn1);
    imshow("strawberry 2", strawIn2);
    waitKey(0);
    destroyAllWindows();

    // image waar we mee willen werken
    Mat img;
    strawIn1.copyTo(img);

    // window aanmaken
    namedWindow(window_name, 1);

    Point punt;

    // callback functie voor muis setten
    setMouseCallback(window_name, onMouse, NULL);

    cout << "Klik op de goede punten, en klik dan op enter" << endl;

    welkePunten = GOED;
    // werkimage tonen voor goede punten
    imshow(window_name, img);
    waitKey(0);

    cout << "Klik op de slechte punten, en klik dan op enter" << endl;

    welkePunten = SLECHT;
    // werkimage tonen voor slechte punten
    imshow(window_name, img);
    waitKey(0);

    // descriptor bepalen op basis van de punten in HSV

    return 0;
}

static void onMouse(int event, int x, int y, int flags, void* userdata)
{
    char puntchar[20];
    if ( welkePunten == GOED)
    {
        char puntchar[20] = "de goede punten";
    }
    else
    {
        char puntchar[20] = "de slechte punten";
    }
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        Point punt = Point(x,y);
        punten[welkePunten].push_back(punt);
        cout << "Punt op (x=" << punt.x << " y=" << punt.y << ") is toegevoegd aan " << puntchar << endl;
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << puntchar << endl;
        if( punten[welkePunten].size() > 0 )
        {
            Point punt = punten[welkePunten].back();
            punten[welkePunten].pop_back();
            cout << "Punt op (x=" << punt.x << " y=" << punt.y << ") is verwijdert van " << puntchar << endl;
        }
        else
        {
            cout << "Geen punten meer in lijst van " << endl;
        }
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        cout << "lijst van " << puntchar << endl;
        for(int punt = 0 ; punt < punten[welkePunten].size() ; punt++)
        {
            cout << "Punt op (x=" << punten[welkePunten][punt].x << " y=" << punten[welkePunten][punt].y << ")" << endl;
        }
    }
}

void trainKNN(Mat img)
{
    int K = 3;

    // image converteren naar hsv
    Mat hsvImage;
    cvtColor(img,hsvImage, CV_BGR2HSV);

    // goede waarden inlezen
    Mat trainingDataGoed(punten[GOED].size(), 3, CV_32FC1);     // trainingdata, met 3 waarden: h, s en v
    Mat labelsGoed(punten[GOED].size(), 1, CV_32SC1);           // labels, met 1 waarden: GOED of SLECHT

    for( int i=0 ; i<punten[GOED].size() ; i++)
    {
        Vec3b descriptor = hsvImage.at<Vec3b>(punten[GOED][i].y, punten[GOED][i].x);
        trainingDataGoed.at<float>(i,0) = descriptor[0];
        trainingDataGoed.at<float>(i,1) = descriptor[1];
        trainingDataGoed.at<float>(i,2) = descriptor[2];
        labelsGoed.at<float>(i) = GOED;
    }

    // slecht waarden inlezen
    Mat trainingDataSlecht(punten[SLECHT].size(), 3, CV_32FC1); // trainingdata, met 3 waarden: h, s en v
    Mat labelsSlecht(punten[SLECHT].size(), 1, CV_32SC1);       // labels, met 1 waarden: GOED of SLECHT

    for( int i=0 ; i<punten[SLECHT].size() ; i++)
    {
        Vec3b descriptor = hsvImage.at<Vec3b>(punten[SLECHT][i].y, punten[SLECHT][i].x);
        trainingDataSlecht.at<float>(i,0) = descriptor[0];
        trainingDataSlecht.at<float>(i,1) = descriptor[1];
        trainingDataSlecht.at<float>(i,2) = descriptor[2];
        labelsSlecht.at<float>(i) = SLECHT;
    }

    // samenvoegen goede en slechte trainingdata
    Mat trainingData, labels;
    vconcat(trainingDataGoed, trainingDataSlecht, trainingData);
    vconcat(labelsGoed, labelsSlecht, labels);

    // CvKNearest knn( trainingData, labels, 0, false, K );
    // CvMat* nearests = cvCreateMat( 1, K, CV_32FC1);
}
//--i1=/home/laurens/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie5/strawberry1.tif --i2=/home/laurens/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie5/strawberry1.tif
