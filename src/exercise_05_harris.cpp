#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

// Global varialbles

double alpha;
Mat img, img_gray;
Mat dx, dy;
Mat dxdx, dydy, dxdy;
Mat dxdx_dydy, dxdy_dxdy;
Mat detA, traceA;
Mat harris_score, harris_score_norm, harris_score_abs;

const char* result_window = "Corner detection";
const char* score_window = "Harris score";

int main(int argc, char** argv)
{
  // Read image 
  img = imread(argv[1], IMREAD_COLOR);
  cvtColor(img, img_gray, COLOR_BGR2GRAY);

  // Calculate horizontal/vertical derivatives
  // (e.g. Sobel Filter)
  int scale = 1;  // Scale factor of the derivative values
  int delta = 0;  // Delta value added to the derivative values
  int ddepth = CV_16S;  // To avoid overflow

  Sobel(img_gray, dx, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  Sobel(img_gray, dy, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

  // Pre-calculate squares and products of derivatives as images
  multiply(dx, dx, dxdx);
  multiply(dy, dy, dydy);
  multiply(dx, dy, dxdy);

  // Apply a Gaussian filter on pre-calculated values (e.g. mask size 7)
  GaussianBlur(dxdx, dxdx, Size(7, 7), 0, 0, BORDER_DEFAULT);
  GaussianBlur(dydy, dydy, Size(7, 7), 0, 0, BORDER_DEFAULT);
  GaussianBlur(dxdy, dxdy, Size(7, 7), 0, 0, BORDER_DEFAULT);

  // Calculate determinant for each pixel
  // det(A) = a11 * a22 - a12 * a21
  multiply(dxdx, dydy, dxdx_dydy);
  multiply(dxdy, dxdy, dxdy_dxdy);
  subtract(dxdx_dydy, dxdy_dxdy, detA);
  
  // Calculate trace for each pixel
  // trace(A) = a11 + a22
  add(dxdx, dydy, traceA);

  // Calculate det(A) - alpha * trace(A)**2 for each pixel
  alpha = 0.06;  // reasonable parameter
  multiply(traceA, traceA, traceA);
  subtract(detA, alpha*traceA, harris_score);
  
  // Normalize aray
  normalize(harris_score, harris_score_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

  // Results bakc to CV_8U
  convertScaleAbs(harris_score_norm, harris_score_abs);

  // Apply threshold
  int thresh = 250;

  for(int i = 0; i < harris_score_norm.rows; i++){
    for(int j = 0; j < harris_score_norm.cols; j++){      
      if((int) harris_score_norm.at<float>(i, j) > thresh) {
        circle(img, Point(j, i), 5,  Scalar(0, 255, 0), 2, 8, 0);
      }
    }
  }

  // Create window
  namedWindow(result_window, WINDOW_AUTOSIZE);
  namedWindow(score_window, WINDOW_AUTOSIZE);

  
  // Show image
  imshow(result_window, img);
  imshow(score_window, harris_score_abs);


  waitKey(0);
  return 0;
}
