#include "opencv2/highgui/highgui.hpp" 
#include "iostream" 
#include "opencv2/core/core.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/opencv.hpp"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace cv;
using namespace std;
RNG rng(12345);

int main(int argc, char** argv){
    const char *absolutePath;
    absolutePath = "/home/michal/Downloads/IMG_5989.JPG";
	Rect bounding_rect;

Mat originalImage, resizedImage;
originalImage = imread(absolutePath,1);
    if (originalImage.empty()){
        cout << "Cannot load original image!" << endl;
        return -1;
    }

    Size size(400,300); 
    resize(originalImage, resizedImage, size);
	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", resizedImage);

/*------------------------------konwersja na odcienie szarosci-----------*/
Mat gray_image;
 cvtColor( resizedImage, gray_image, COLOR_BGR2GRAY );

 imwrite( "../../images/Gray_Image.jpg", gray_image );
 namedWindow( "Gray image", WINDOW_AUTOSIZE );


 imshow( "Gray image", gray_image );
//--------------------------threshold------------------------

Mat thresholdImage;
    threshold(gray_image, thresholdImage, 120, 255, THRESH_BINARY);
    imshow("Threshold", thresholdImage);

//----------------------------------morp - erozja ------------------
Mat morp;
int erosion_size = 1; //fra 1 e 2 è ok!
    Mat element = getStructuringElement( MORPH_CROSS,
                                       Size(3, 3),
        Point(erosion_size, erosion_size));
    erode(thresholdImage, morp, element);  
    //dilate(image,dst,element);
    imshow("erosion window", morp);
Mat blur;
GaussianBlur( morp, blur, Size( 3, 3 ), 0, 0 );
imshow("Blur", blur);

vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
 findContours( thresholdImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Point2f>center( contours.size() );
  vector<float>radius( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
     }


  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( blur.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
       circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
     }

  /// Show in a window
  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing ); 
Mat mask_image;
Mat croppedImage;
 for (int i = 0; i < contours.size(); i++)
    {
        //drawContours(drawing, contours_poly, i, CV_RGB(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
         
        int s_x = boundRect[i].x;
        int s_y = boundRect[i].y;
        float width = boundRect[i].width;
        float height = boundRect[i].height;     
if(width/height>3.7 && width/height<5.5 && width*height>1200 && width*height<4000){
cout<<width<<" "<<height;
rectangle( resizedImage, boundRect[i].tl(), boundRect[i].br(), CV_RGB(255, 0, 0), 2, 8, 0 );
imshow("masked", resizedImage);
	Rect myROI(s_x-10,s_y-10  ,width+18,height+10);
	croppedImage = thresholdImage(myROI);
imshow("cropped", croppedImage);


tesseract::TessBaseAPI *tess = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (tess->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    } 
tess->SetImage((uchar*)croppedImage.data, croppedImage.size().width, croppedImage.size().height, croppedImage.channels(), croppedImage.step1());
tess->Recognize(0);
const char* out = tess->GetUTF8Text();
cout<<out;
delete [] out;
}
}
 waitKey(0);
}
