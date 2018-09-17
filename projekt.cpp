#include "opencv2/highgui/highgui.hpp" 
#include "iostream" 
#include "opencv2/core/core.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    const char *absolutePath;
    absolutePath = "index.jpeg";

Mat originalImage, resizedImage;
originalImage = imread(absolutePath,1);
    if (originalImage.empty()){
        cout << "Cannot load original image!" << endl;
        return -1;
    }

    Size size(400, 300); 
    resize(originalImage, resizedImage, size);
	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", resizedImage);

/*------------------------------konwersja na odcienie szarosci-----------*/
Mat gray_image;
 cvtColor( resizedImage, gray_image, COLOR_BGR2GRAY );

 imwrite( "../../images/Gray_Image.jpg", gray_image );
 namedWindow( "Gray image", WINDOW_AUTOSIZE );


 imshow( "Gray image", gray_image );

Mat thresholdImage;
    threshold(gray_image, thresholdImage, 100, 255, THRESH_BINARY);
    imshow("Threshold", thresholdImage);

 waitKey(0);
}
