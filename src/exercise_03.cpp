// Solution for Exercise 3 - Computer Vision 1 Lecture
//
// Author: Pablo Rodriguez Robles
// Email: pablordrrbl@gmail.com
//
// Credits to the excercise propolsal:
// TU Dresden, Inf, CV1 Ex3, Holger Heidrich
//
//
// Exercise 3, Orientation Histograms
//
// ----
// TODO: Drescription
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

// Image matrices
Mat image, gray_image;
Mat Ix, Iy;
Mat abs_Ix, abs_Iy;
Mat mag, angle;
Mat color_edges, mix;
Mat grad_hist;
Mat rot_image;

// Trackbar parameters
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;

void on_trackbar(int, void*)
{
  alpha = (double) alpha_slider/alpha_slider_max;
  beta = (1.0 - alpha);

  cvtColor(mag, color_edges, cv::COLOR_GRAY2BGR);
  
  addWeighted(image, alpha, color_edges, beta, 0.0, mix);

  // imshow("Linear Blend", mix);
}

int main( int argc, char** argv ){
 
  // Load input image
  char* filename = argv[1];
  image = imread(filename, 1);

  // Convert our image from BGR to Grayscale format
  cvtColor(image, gray_image, CV_BGR2GRAY);

  // --- Generate a image for the gradient intensity of the gay image ---

  // --- Compute image gradients ---
  int scale = 1;
  int delta = 0;
  int ddepth = CV_32F;  // To avoid overflow
  
  Sobel(gray_image, Ix, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  Sobel(gray_image, Iy, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

  cartToPolar(Ix, Iy, mag, angle, 1);

  // Convert partial results back to CV_8U
  convertScaleAbs(Ix, abs_Ix);
  convertScaleAbs(Iy, abs_Iy);
  convertScaleAbs(mag, mag);
  convertScaleAbs(angle, angle);

  // Establish the number of bins
  int histSize = 360;

  //
  float range[] = { 0, 360 } ;
  const float* histRange = { range };
  
  bool uniform = true;
  bool accumulate = false;

  // Compute histogram
  calcHist(&angle, 1, 0, Mat(), grad_hist, 1, &histSize, &histRange, uniform, accumulate);

  // Draw histogram
  int hist_w = 720; int hist_h = 400;
  int bin_w = cvRound((double) hist_w/histSize);  // Rounds floating-point number to the nearest integer
  
  // Show magtrix data
  // cout << mag << endl << endl;

  // Create image with withe background
  Mat histImage(hist_h, hist_w, CV_8U, Scalar(255, 255, 255));  // (rows, cols) --> (hist_h, hist_w)

  // Normalize the result to [0, histImage.rows]
  normalize(grad_hist, grad_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

  // Draw
  for(int i = 0; i < histSize; i++)
    {
      float hval = cvRound(grad_hist.at<float>(i));
      
      rectangle(histImage,
                Rect(bin_w*i, hist_h - hval, bin_w, hval),  // (x, y, width, height), top-left
                cv::Scalar::all(0),  // Black rectangle
                CV_FILLED
                );
    }

  int  maxId;
  Mat sortedHist;
  double rot_angle;
  
  sortIdx(grad_hist, sortedHist, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);

  maxId = sortedHist.at<int>(0, 1);
  rot_angle = 360 / histSize * maxId;
  cout << sortedHist << endl;
  cout << maxId << "   " << rot_angle << endl;
  
  Point2f pc(image.cols/2., image.rows/2.);
  Mat r = cv::getRotationMatrix2D(pc, rot_angle, 1.0);

  warpAffine(image, rot_image, r, image.size());
  
  // Display
  namedWindow("Gradient Histogram", CV_WINDOW_AUTOSIZE );
  imshow("Gradient Histogram", histImage );
  
  namedWindow("Original");
  //namedWindow("Ix");
  //namedWindow("Iy");

  imshow("Original", rot_image);
  //imshow("Ix", abs_Ix);
  //imshow("Iy", mag);

  // Initialize values
  alpha_slider = 0;

  // Create Windows
  //namedWindow("Linear Blend", 1);

  // Create Trackbars
  char TrackbarName[50];
  sprintf(TrackbarName, "Alpha x %d", alpha_slider_max);

  createTrackbar(TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar);
  
  on_trackbar(alpha_slider, 0);

  waitKey();
    
  return 0;
  
  // All the memory will automatically be released by Vector<>, Mat and Ptr<> destructors.
}

