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
    if (originalImage.empty())
    {
        cout << "Cannot load original image!" << endl;
        return -1;
    }

    Size size(800,600); 
    resize(originalImage, resizedImage, size);
	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", resizedImage);

/*------------------------------konwersja na odcienie szarosci-----------*/
Mat gray_image;
 cvtColor( resizedImage, gray_image, COLOR_BGR2GRAY );
 namedWindow( "Gray image", WINDOW_AUTOSIZE );
 imshow( "Gray image", gray_image );
//--------------------------threshold------------------------


Mat thresholdImage;
    threshold(gray_image, thresholdImage, 140, 255, THRESH_BINARY); //minimalna ustawiona na 130-140, zalezne od aparatu i oswietlenia
    imshow("Threshold", thresholdImage);

//----------------------------------morp - erozja ------------------
Mat morp;
int erosion_size = 1; 
    Mat element = getStructuringElement( MORPH_CROSS, Size(3, 3), Point(erosion_size, erosion_size)); //
    erode(thresholdImage, morp, element);
    imshow("erosion window", morp);
    
    
//----------------------------------Rozmycie Gaussa ------------------    
	Mat blur;
	GaussianBlur( thresholdImage, blur, Size( 1, 1 ), 0, 0 );
	imshow("Blur", blur);
	

//------------------------------------------wyznaczenie konturow -------------
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( thresholdImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

//----------------------------------Wyznaczenie prostokatow ------------------
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { 
		 approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true ); //
		 boundRect[i] = boundingRect( Mat(contours_poly[i]) ); //wyznaczenie prostokatow okalajacych kontury
     }


  Mat drawing = Mat::zeros( blur.size(), CV_8UC3 );
  
  for( int i = 0; i< contours.size(); i++ )
     {
		 Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		 drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//rysuje kontury
		 rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );//rysuje prostokaty
     }

  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing ); 
  
	Mat mask_image;
	Mat croppedImage;
	
	
 for (int i = 0; i < contours.size(); i++)
    {
         
        int s_x = boundRect[i].x;
        int s_y = boundRect[i].y;
        float width = boundRect[i].width;
        float height = boundRect[i].height;     
        
		if(width/height>3.8 && width/height<5 && width*height>6000 && width*height<20000) //wyznaczenie prostokata o wymiarach odpowadajacych tablicom
		{
			//cout<<width<<" "<<height;
			rectangle( resizedImage, boundRect[i].tl(), boundRect[i].br(), CV_RGB(255, 0, 0), 2, 8, 0 ); 
			imshow("masked", resizedImage);
			Rect myROI(s_x+0.1*width,s_y-0.07*height  ,0.9*width,0.93*height); //tworzy prostokat z wlasciwa tablica (bez wyznacznika kraju i tej reklamy warsztatu z dolu)
			croppedImage = thresholdImage(myROI); //wycina tablice
			Size powieksz((width)*1.5,(height)*1.5); 
			resize(croppedImage, croppedImage, powieksz); //powiekszenie tablicy
			dilate(croppedImage, croppedImage, element);
			GaussianBlur(croppedImage, croppedImage, Size( 3, 3 ), 0, 0 );
			erode(croppedImage, croppedImage, element); //dylatacja -> rozmycie -> erozja

//Canny( croppedImage, croppedImage, 3, 3, 3 );


			imshow("cropped", croppedImage);

//----------------------------------Tesseract - rozpoznawanie tekstu ----------------/
			tesseract::TessBaseAPI *tess = new tesseract::TessBaseAPI();
			if (tess->Init(NULL, "arklas"))  //"arklas" to nazwa fontu zblizonego do polskich tablic rejestracyjnych, uzyta do treningu
				{
					fprintf(stderr, "Could not initialize tesseract.\n");
					exit(1);
				} 
		
			tess->SetImage((uchar*)croppedImage.data, croppedImage.size().width, croppedImage.size().height, croppedImage.channels(), croppedImage.step1());
			tess->Recognize(0);
			const char* out = tess->GetUTF8Text();
			cout<<"Nr rejestracyjny: "<<out;
			delete [] out; //chyba do wycieku, nie pamietam
		}
	}
 waitKey(0);
}
