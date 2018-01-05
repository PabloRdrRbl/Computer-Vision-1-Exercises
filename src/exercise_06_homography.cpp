/*
 * Computer Vision 1 - Assignment 6: Feature Matching
 *                   Homographies
 *
 * Author: Pablo Rodriguez Robles
 * Email: pablordrrbl@gmail.com
 * Instructor: Dipl.-Phys. Holger Heidrich
 * University: TU Dresden
 *
 * -----
 * Write a programm ("scanner app") that reshapes a perspective
 * distorted photo of a DIN A4 image into its original side ratio.
 * 
 *   a) By clicking the 4 corners.
 *   b) Automatically by finding and selecting feature points.
 *      What feature(s) would you use? Which additional constraints
 *      can be applied?
 *
 *      Poligonal contours are used as features. 
 *      The constraints applied are that the poligon must have 4 vertices,
 *      it must be convex and its area the biggest area detected being
 *      bigger than the 50% of the area of the picture.
 *
 * -----
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

struct userdata{
  Mat img;
  vector<Point2f> points;
};

void mouseHandler(int event, int x, int y, int flags, void* data_ptr)
{
  if  ( event == EVENT_LBUTTONDOWN )
    {
      userdata *data = ((userdata *) data_ptr);
      circle(data->img, Point(x,y),3,Scalar(0,0,255), 5, CV_AA);
      imshow("Image", data->img);
      if (data->points.size() < 4)
        {
          data->points.push_back(Point2f(x,y));
        }
    }
}

void cornerDetector(userdata* data)
{
  // Bilateral filter can reduce unwanted noise very well while keeping
  // edges fairly sharp
  Mat img_fil;
  bilateralFilter(data->img, img_fil, 9, 75, 75);

  // Create black and white image based on adaptive threshold
  Mat img_adap;
  adaptiveThreshold(img_fil, img_adap, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
                    THRESH_BINARY, 115, 4);
  
  // Median filter clears small details
  Mat img_blur;
  medianBlur(img_adap, img_blur, 11);
  
  // Canny edges detection
  Mat img_canny;
  Canny(img_blur, img_canny, 200, 250);
  
  // The contours touching the image border will be clipped by
  // the contour finding algorithm. Add a 5 px black border
  copyMakeBorder(img_canny, img_canny, 5, 5, 5, 5, BORDER_CONSTANT);

  // Find contours
  vector<vector<Point> > contours0;
  vector<Vec4i> hierarchy;
  findContours(img_canny, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

  // Find contour of biggest rectangle,
  // otherwise return corners of original image.
  // Remember 5 px border!
  double width = img_canny.cols;
  double height = img_canny.rows;
  double MAX_CONTOUR_AREA = (width - 10) * (height - 10);

  // Page fill at least half of image, then saving max area
  double MAX_AREA_FOUND = MAX_CONTOUR_AREA * 0.5;

  // Saving the page contour, from top-left to bottom-right
  vector<Point> pageContour;
  pageContour.push_back(Point(5,5));
  pageContour.push_back(Point(5, height - 5));
  pageContour.push_back(Point(width - 5, height - 5));
  pageContour.push_back(Point(width - 5, 5));

  vector<vector<Point> > contours;
  contours.resize(contours0.size());
  
  // Iterate all the detected contours
  for(int i=0; i<contours0.size(); i++)
    {
      double perimeter =  arcLength(contours0[i], true);
      approxPolyDP(Mat(contours0[i]), contours[i], 0.03*perimeter, true);

      // Page has 4 corners and it is convex
      // Page area must be bigger than MAX_AREA_FOUND
      if ((contours[i].size() == 4) &&
          isContourConvex(contours[i]) &&
          (MAX_AREA_FOUND < contourArea(contours[i])) &&
          (contourArea(contours[i]) < MAX_CONTOUR_AREA))
        {
          MAX_AREA_FOUND = contourArea(contours[i]);
          pageContour = contours[i];
       }
    }

  data->points.push_back(Point2f(pageContour[0].x, pageContour[0].y));
  data->points.push_back(Point2f(pageContour[1].x, pageContour[1].y));
  data->points.push_back(Point2f(pageContour[2].x, pageContour[2].y));
  data->points.push_back(Point2f(pageContour[3].x, pageContour[3].y));
}


int main(int argc, const char **argv) {

  // Read source imagen
  Mat img_src = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

  // Manual vs. automatic point selection
  bool manualPts = false;
  
  // Destination image.
  // The aspect ratio of a DIN A4 paper is approximately 1:1.4142 (1:sqrt(2))
  Size size(300, 424);
  Mat img_dst = Mat::zeros(size,CV_8U);

  // Vector containing the 4 corner points on the destination image.
  // The 4 corners of the image
  vector<Point2f> pts_dst;
 
  pts_dst.push_back(Point2f(0,0));
  pts_dst.push_back(Point2f(size.width - 1, 0));
  pts_dst.push_back(Point2f(size.width - 1, size.height -1));
  pts_dst.push_back(Point2f(0, size.height - 1 ));

  Mat H;
  
  if (manualPts)
    {
      // Set data for mouse event
      Mat img_temp = img_src.clone();
      userdata data;
      data.img = img_temp;

      cout << "Click on the four corners of the DIN A4" << endl
           << "Top left first and bottom left last" << endl
           << "Then hit ENTER" << endl;

      // Show image and wait for 4 clicks
      imshow("Image", img_temp);

      // Set the callback function for any mouse event
      setMouseCallback("Image", mouseHandler, &data);

      waitKey(0);

      // Calculate the homography      
      H = findHomography(data.points, pts_dst);
    }
  else
    {
      Mat img_temp = img_src.clone();
      userdata data;
      data.img = img_temp;
      
      cornerDetector(&data);
      H = findHomography(data.points, pts_dst);
    }

  // Warp source image to destination
  warpPerspective(img_src, img_dst, H, size);

  // Show image
  imshow("Image", img_dst);

  waitKey(0);

  return 0;
}
