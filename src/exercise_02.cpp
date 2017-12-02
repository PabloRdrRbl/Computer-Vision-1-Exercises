// Solution for Exercise 2 - Computer Vision 1 Lecture
//
// Author: Pablo Rodriguez Robles
// Email: pablordrrbl@gmail.com
//
// Credits to the excercise propolsal:
// TU Dresden, Inf, CV1 Ex2, Holger Heidrich
//
//
// Exercise 2, non-linear grey value transformation, solarisation
//
// ----
// This program plays with colors simulating solarisation.	
// Clicking two times into the "GreyCurve"-image gives two points.
// Make these points be the extremals of a polynomial of degree three in
// the range 0,...,255 x 0,...,255.
// The "GreyCurve"-image itself has size (512,512). Point (0,0) is upper left corner.
// Use the polynomial y(x) to map values x of the input image to values y of an output
// image by creating and using a lookup table.
// Saturate to 0 and 255 if the mapped grey / color values exceed these values.
// ------
//

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <functional>
#include <queue>

using namespace cv;
using namespace std;

#define MaxPoints 2
int nb_points = 0;

Mat MapCurveImage512;
Mat image, color_img;
CvPoint  SrcPtInt[MaxPoints];
unsigned char LUtable[256];

void help()
{
	cout <<
	"\nTU Dresden, Inf, CV1 Ex2, Holger Heidrich\n"
	"This program plays with colors simulating solarisation.\n"
	"Clicking two times into the \"GreyCurve\"-image gives two points.\n"
	"Make these points be the extremals of a polynomial of degree three in the range 0..255 x 0..255.\n"
	"The \"GreyCurve\"-image itself has size (512,512). Point (0,0) is upper left corner. \n"
	"Use the polynomial y(x) to map values x of the input image to values y of an output image.\n"
	"Saturate to 0 and 255 if the mapped grey / color values exceed these values. \n"
	"Call:\n"
	"./image Image [image-name Default: fruits.jpg]\n" << endl;
}

void on_mouse( int event, int x, int y, int flags, void* param ){
  switch( event ){
        case CV_EVENT_LBUTTONDOWN:{
          // add new point to polygon
          // two points have to be clicked
          if (nb_points < MaxPoints){  

            // CvPoint type
            SrcPtInt[nb_points].x=x;
            SrcPtInt[nb_points].y=y;

            // second point in SrcPtInt
            if (nb_points){ 

              // Clear image for the curve
              MapCurveImage512 = 0;

              // read the two extrema points
              int x1 = SrcPtInt[0].x;	
              int x2 = SrcPtInt[1].x;
              // "511 -" because we want "0" in lower left corner
              int y1 = 551 - SrcPtInt[0].y; 
              int y2 = 551 - SrcPtInt[1].y;
              
              // determine polynomial coefficients of y(x):=a*(x-x0)**3-b*x+c
              double dx = x1 - x2;
              double dy = y1 - y2;
              
              float x0 = (float)(x1 + x2) / 2;
              
              const float a = (float)(-2.0*dy / pow(dx, 3.0));
              const float b = (float)(-3.0 / 2.0*dy / dx);
              const float c = (float)((y1 + y2) / 2.0 + b*x0);
 
              // create the LUT for that polynom and 
              // draw the polynom in the MapCurveimage (pixelwise)
              Point MappingCurvePoint_1 = Point(0,0);
              Point MappingCurvePoint_2;
              
              for (int i=0; i<256; i++){
                int k;
                k = (int) (a*pow(i-x0,3) - b*i + c);
                if (k<0) k=0;
                else if (k>511);

                if (!(i % 2)){
                  LUtable[i/2] = (char)(k/2);
                }

                
                // MapCurveImage512.at<char>(511-k,i)=(char)255;         // without lines between each point we would have the points only
                MappingCurvePoint_2 = Point(i, 511 - k);
                line(MapCurveImage512, MappingCurvePoint_1, MappingCurvePoint_2, CV_RGB(255, 255, 255));  // lines between each point
                MappingCurvePoint_1 = MappingCurvePoint_2;
				   }
              // Let opencv do the lookup table transform
              // LUT(image, Mat(1, 256, CV_8U, LUtable), color_img);

               // Or in this case, we want to do the lookup table transform ourselves
               uchar *pIm1 = image.data;
               uchar *pIm2 = color_img.data;
               int totalNumberSubPixels = image.rows * image.step[0]; 
               for (int i = 0; i<totalNumberSubPixels; i++)
               {
                  pIm2[i] = LUtable[pIm1[i]];
               }

				   // Show non-linear mapping curve
				   imshow( "GreyCurve", MapCurveImage512);

				   // Show the result
				   imshow( "result image", color_img );

                   nb_points = 0;
            } // end if

            else nb_points++;
          } // end if
          break;
        } // end case
    } // end switch
} // end void on_mouse(int event, int x, int y, int flags, void* param)


int main( int argc, char** argv ){
  // help();

  // Load input image
  char* filename = argv[1];
  image = imread(filename, 1);

  // Mat::clone
  // Creates a full copy of the array and the underlying data
  color_img = image.clone();

  // Create two windows
  namedWindow("Original Image");
  namedWindow("GreyCurve");

  // Show original image
  imshow("Original Image", color_img);

  // Mat::create
  // Allocates new array data if needed
  // void cv::Mat::create(int rows, int cols, int type)
  // CV_8U is unsigned 8bit/pixel
  // i.e. a pixel can have values 0-255, this is the normal
  // range for most image and video formats
  MapCurveImage512.create(512, 512, CV_8U);
  
  // Set all matrix values to zero
  MapCurveImage512 = 0;

  // Show image
  imshow("GreyCurve", MapCurveImage512);

  // Sets mouse handler for the specified window
  // void setMouseCallback(const string& winname, MouseCallback onMouse, void* userdata=0)
  //onMouse – Mouse callback. See OpenCV samples, such as
  // https://github.com/opencv/opencv/tree/master/samples/cpp/ffilldemo.cpp
  // on how to specify and use the callback
  cvSetMouseCallback("GreyCurve", on_mouse, 0);
  
  waitKey();
    
  return 0;
  
  // All the memory will automatically be released by Vector<>, Mat and Ptr<> destructors.
}

