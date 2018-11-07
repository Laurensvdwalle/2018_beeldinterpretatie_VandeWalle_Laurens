#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{

    // CommandlineParser opstarten
    CommandLineParser parser(argc, argv,
        "{ help h usage ?           |   | print this message    }"
        "{ i1 image_bimodal         |   | bimodal image         }"
        "{ i2 image_color           |   | color image           }"
        "{ i3 image_color_adapted   |   | adapted color image   }"
    );

    if (parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    // inputparameters
    string i1(parser.get<string>("image_bimodal"));
    string i2(parser.get<string>("image_color"));
    string i3(parser.get<string>("image_color_adapted"));

    if(i1.empty() || i2.empty() || i3.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images inlezen
    Mat bimodalImage;
    Mat colorImage;
    Mat adaptedColorImage;

    bimodalImage = imread(i1);
    colorImage = imread(i2);
    adaptedColorImage = imread(i3);

    if(bimodalImage.empty() || colorImage.empty() || adaptedColorImage.empty())
    {
        parser.printMessage();
        return -1;
    }

    // images tonen
    //imshow("Bimodal Image", bimodalImage);
    //waitKey(0);
    //imshow("Color Image", colorImage);
    //waitKey(0);
    //imshow("Adapted Color Image", adaptedColorImage);
    //waitKey(0);

    // kleurafbeelding opslpitsen in kleurkanalen en tonen
    vector<Mat> kleurkanalen;
    split(adaptedColorImage, kleurkanalen);
    imshow("Blauw", kleurkanalen[0]);
    imshow("Groen", kleurkanalen[1]);
    imshow("Rood", kleurkanalen[2]);
    waitKey(0);

    // Masker aanmaken
    Mat mask = Mat::zeros(adaptedColorImage.rows, adaptedColorImage.cols, CV_8UC1);
    Mat BLUE = kleurkanalen[0]; Mat GREEN = kleurkanalen[1]; Mat RED = kleurkanalen[2];

    mask = (RED>95) & (GREEN>40) & (BLUE>20) & ((max(RED,max(GREEN,BLUE)) - min(RED,min(GREEN,BLUE)))>15) &
    (abs(RED-GREEN)>15) & (RED>GREEN) & (RED>BLUE);

    mask = mask *255;
    imshow("Mask", mask);
    waitKey(0);

    //Pixelwaarden tonen
    //for(int i=0; i<colorImage.rows; i++){
    //    for(int j=0; j<colorImage.cols; j++){
    //        cout << (int)colorImage.at<uchar>(i,j)<< endl;
    //    }
    //}
    //waitKey(0);

    // dilating van mask
    erode( mask.clone(), mask, Mat(), Point(-1, -1), 2 );
    imshow("masker eroded", mask);
    waitKey(0);

    // dilating van mask
    dilate( mask.clone(), mask, Mat(), Point(-1, -1), 2 );
    imshow("masker dilated", mask);
    waitKey(0);

    // hand en arm terug samenvoegen
    dilate(mask.clone(), mask, Mat(), Point(-1, -1), 10);
    erode(mask.clone(), mask, Mat(), Point(-1, -1), 5);
    imshow("finaal masker", mask);
    waitKey(0);

    // Masker combineren met originele figuur

    Mat finaal(adaptedColorImage.rows, adaptedColorImage.cols, CV_8UC3);
    Mat pixels_b = BLUE & mask;
    Mat pixels_g = GREEN & mask;
    Mat pixels_r = RED & mask;

    vector<Mat> channels_mix;
    channels_mix.push_back(pixels_b);
    channels_mix.push_back(pixels_g);
    channels_mix.push_back(pixels_r);
    merge(channels_mix, finaal);

    imshow("Afbeelding met masker", finaal);
    waitKey(0);

    // OTSU thresholding
    Mat grayImage;
    Mat otsuImage;
    cvtColor(bimodalImage, grayImage, CV_RGB2GRAY);
    //imshow("Grijswaarden ticket", grayImage);
    //waitKey(0);

    threshold(grayImage, otsuImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("OTSU ticket", otsuImage);
    waitKey(0);

    // Met histogram equalization
    Mat grayImageHist;
    equalizeHist(grayImage.clone(), grayImageHist);
    imshow("Hist ticket", grayImageHist);
    waitKey(0);

    // OTSU thresholding
    Mat grayImageHistOtsu;
    threshold(grayImageHist, grayImageHistOtsu, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("OTSU ticket met Histogram equalization", grayImageHistOtsu);
    waitKey(0);

    // Met CLAHE
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(4);

    Mat grayImageClahe;
    clahe->apply(grayImage.clone(),grayImageClahe);
    imshow("CLAHE ticket",grayImageClahe);
    waitKey(0);

    // OTSU thresholding
    Mat grayImageClaheOtsu;
    threshold(grayImageClahe, grayImageClaheOtsu, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("OTSU ticket met CLAHE", grayImageClaheOtsu);
    waitKey(0);

    return 0;
}

// --i1=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie1/imageBimodal.jpg --i2=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie1/imageColor.jpg --i3=/home/student/Documents/Laurens/2018_beeldinterpretatie_VandeWalle_Laurens/Sessie1/imageColorAdapted.jpg
