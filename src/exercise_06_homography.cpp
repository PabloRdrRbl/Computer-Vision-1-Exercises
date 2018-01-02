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
 * -----
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>

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

int main(int argc, const char **argv) {

  // Read source imagen
  
  Mat img_src = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

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
  Mat H = findHomography(data.points, pts_dst);

  // Warp source image to destination
  warpPerspective(img_src, img_dst, H, size);

  // Show image
  imshow("Image", img_dst);

  waitKey(0);

  return 0;
}
