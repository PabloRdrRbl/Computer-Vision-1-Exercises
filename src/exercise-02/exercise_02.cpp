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
int nb_points=0;

Mat MapCurveimage512;
Mat image, mapped_result_img;
CvPoint    SrcPtInt[MaxPoints];

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

void on_mouse( int event, int x, int y, int flags, void* param )
{
    switch( event )
    {
        case CV_EVENT_LBUTTONDOWN:
        {
            // ad new point to polygon
            if (nb_points < MaxPoints) 
            {  
               SrcPtInt[nb_points].x=x;
               SrcPtInt[nb_points].y=y;
               if (nb_points)
               {   // second point in SrcPtInt
				   
                  MapCurveimage512 = 0;

                  // read the two extrema points
                  int x1 = SrcPtInt[0].x;	
				      int x2 = SrcPtInt[1].x;	
                  int y1 = 511 - SrcPtInt[0].y; // "511 -" cause we want "0" in lower left corner
                  int y2 = 511 - SrcPtInt[1].y;

                  // determine polynomial coefficients of y(x):=a*(x-x0)³-b*x+c 
                  // (want to cheat? see end of file)

                  // create the LUT for that polynom and 
                  // draw the polynom in the MapCurveimage (pixelwise)
				      // your code for the polynomial and color transform (instead of the following line)
                  line(MapCurveimage512, SrcPtInt[0], SrcPtInt[1], CV_RGB(255, 255, 255));	// remove that line example 

                  // use the lookup table (LUT) to map the input image to the result image
                  // use the same LUT for each color channel (or fantasize) 

				      // show non-linear mapping curve
                  imshow("GreyCurve", MapCurveimage512);
				   
				      // show the result
				      imshow( "result image", mapped_result_img );

				      // mapped_result_img = image.clone();
                      nb_points = 0;
               } else nb_points++;
			   }
        }
        break;
    }
}  // void on_mouse( int event, int x, int y, int flags, void* param )


int main( int argc, char** argv )
{
	help();
    
	char* filename = argc == 3 ? argv[1] : (char*)"images//fruits.jpg";
    image = imread(filename, 1);
	mapped_result_img = image.clone();

	namedWindow( "GreyCurve");
	namedWindow( "Fruits!");
	imshow( "Fruits!", mapped_result_img );
	
   MapCurveimage512.create(512, 512, CV_8U);
   MapCurveimage512 = 0;
   imshow("GreyCurve", MapCurveimage512);

	cvSetMouseCallback( "GreyCurve", on_mouse, 0 );
	waitKey();
    
    return 0;
    // all the memory will automatically be released by Vector<>, Mat and Ptr<> destructors.
}




































































































/*
// determine polynomial coefficients of y(x):=a*(x-x0)³-b*x+c;
int x1 = SrcPtInt[0].x;	int y1 = 511 - SrcPtInt[0].y;
int x2 = SrcPtInt[1].x;	int y2 = 511 - SrcPtInt[1].y;
double        dx = x1 - x2;	double		  dy = y1 - y2;
float x0 = (float)(x1 + x2) / 2;
float a = (float)(-2.0*dy / pow(dx, 3.0));
float b = (float)(-3.0 / 2.0*dy / dx);
float c = (float)((y1 + y2) / 2.0 + b*x0);
*/
