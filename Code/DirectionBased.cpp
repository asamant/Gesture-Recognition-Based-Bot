//direction based gesture recognition
#include <iostream>
#include "tserial.h"
#include "bot_control.h"
#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;
RNG rng(12345);


int main()
{
	serial comm; // Serial Communication with arduino
	comm.startDevice("COM5", 9600);

	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;
	 //create a window called "MyVideo"

	Mat bframe_original(480, 640, CV_8UC3, Scalar(0, 0, 255));
	Mat bframe_processed(480, 640, CV_8UC1, Scalar(0));
	Mat median(480, 640, CV_8UC1, Scalar(0));
	
	Rect bounding_rect;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);
	namedWindow("ProcessedVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	namedWindow("Hull demo", CV_WINDOW_AUTOSIZE);

	vector<int> compression_params; //vector that stores the compression parameters of the image
	
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	
	compression_params.push_back(98); //specify the compression quality

	int erosion_size = 1;
	int dilation_size = 3;
	Mat element_erosion = getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));
	Mat element_dilation = getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		cv::Point(dilation_size, dilation_size));
	int k = 0;
	while (1)
	{
		Mat contour = Mat(480, 640, CV_8UC1, Scalar(0));
		Mat drawing = Mat(480, 640, CV_8UC1, Scalar(0));
		// read a new frame from video
		bool bSuccess = cap.read(bframe_original);
		 //show the frame in "MyVideo" window
		for (int i = 0; i < dHeight; i+=2)
		{
			for (int j = 0; j < dWidth; j+=2)
			{
				if (bframe_original.at<Vec3b>(i, j)[0] > 20 && bframe_original.at<Vec3b>(i, j)[1] > 40 && bframe_original.at<Vec3b>(i, j)[2] > 95)
				{
					if ((bframe_original.at<Vec3b>(i, j)[2] - fminf(bframe_original.at<Vec3b>(i, j)[1], bframe_original.at<Vec3b>(i, j)[0])) > 15)
					{
						if ((bframe_original.at<Vec3b>(i, j)[2] - bframe_original.at<Vec3b>(i, j)[1]) > 15 && bframe_original.at<Vec3b>(i, j)[2] > bframe_original.at<Vec3b>(i, j)[0])
						{
							/*bframe_processed.at<Vec3b>(i, j)[0] = 255;
							bframe_processed.at<Vec3b>(i, j)[1] = 255;
							bframe_processed.at<Vec3b>(i, j)[2] = 255;*/
							bframe_processed.at<uchar>(i, j) = 255;
							
							continue;
						}
					}
				}
				
				/*bframe_processed.at<Vec3b>(i, j)[0] = 0;
				bframe_processed.at<Vec3b>(i, j)[1] = 0;
				bframe_processed.at<Vec3b>(i, j)[2] = 0;*/
				bframe_processed.at<uchar>(i, j) = 0;
				

			}
		}
		GaussianBlur(bframe_processed, median, Size(7,7), 0.0, 0.0, BORDER_DEFAULT);

		threshold(median, median, 70, 255, THRESH_BINARY + THRESH_OTSU);
		erode(median, median, element_erosion);
		medianBlur(median, median, 3);
		dilate(median, median, element_dilation);
			
		findContours(median, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		if (contours.size() > 0)
		{
			vector<vector<Point>> hull(contours.size());
			//vector<vector<Vec4i>> convDef(contours.size());
			//vector<vector<int> >hullIdx(contours.size());
			//vector<vector<Point>> defect_points(contours.size());


			Scalar color = Scalar(255);
			double area = 0.0, max_area = 0.0;
			int i, ci = 0;
			vector <Point> cnt;
			for (i = 0; i < contours.size(); i++)
			{
				convexHull(Mat(contours[i]), hull[i], false);
				drawContours(contour, contours, i, color, 1, 8, hierarchy, 0, Point());
				cnt = contours[i];
				area = contourArea(cnt, false);
				if (area > max_area)
				{
					max_area = area;
					ci = i;
				}
			}
			bounding_rect = boundingRect(contours[ci]);
			/*Point centre;
			centre.x = bounding_rect.x + (bounding_rect.width / 2);
			centre.y = bounding_rect.y + (bounding_rect.height / 2);
			*/
			Moments mu = moments(contours[ci], false);
			Point centre(mu.m10 / mu.m00, mu.m01 / mu.m00);

			
			//color = Scalar(0, 255, 0);
			color = Scalar(127);
			drawContours(drawing, contours, ci, color, 1, 8, hierarchy, 0, Point());
			color = Scalar(63);
			drawContours(drawing, hull, ci, color, 1, 8, hierarchy, 0, Point());
			line(drawing, centre, centre, Scalar(255), 2, 8);
			circle(drawing, centre, 130, Scalar(255), 1, 8);

			int thresh = 150;
			int count = 5;
			for (i = 0; i < contours[ci].size(); i++)
			{
				if (centre.x - contours[ci][i].x > thresh)
				{
					//cout << "Right gesture" << endl;
					count = 1;
					break;
				}
				else if (contours[ci][i].x - centre.x > thresh)
				{
					//cout << "Left gesture" << endl;
					count = 2;
					break;
				}
				else if (centre.y - contours[ci][i].y > thresh)
				{
					//cout << "Up gesture" << endl;
					count = 3;
					break;
				}
				else if (contours[ci][i].y - centre.y > thresh)
				{
					//cout << "Down gesture" << endl;
					count = 4;
					break;
				}
			}

			k++;
			if (k == 12)
			{
				if(count == 4)
					cout << "Down gesture" << endl; 
				else if(count == 3)
					cout << "Up gesture" << endl; 
				else if(count == 2)
					cout << "Left gesture" << endl; 
				else if(count == 1)
					cout << "Right gesture" << endl; 
				comm.send_data((char)count);
				k = 0;
			}
			//vector <Vec4i> defects;
			////CvConvexityDefect defects;
			//convexityDefects(contours[ci], hull[ci], defects);
			//for (int i = 0; i < defects.size(); i++)
			//{
			//	line(drawing, defects[i].val[0], centre, Scalar(255), 2, 8);
			//}

			/*Size axes(1, 1);
			vector<Point> circle_points;
			ellipse2Poly(centre, axes, 0, 0, 360, 1, circle_points);
			if (circle_points.size() != 0)
			{
				for (int i = 0; i < circle_points.size(); i++){
					//unsigned char b = drawing.at<Vec3b>(circle_points[i].y, circle_points[i].x)[0];
					//unsigned char g = drawing.at<Vec3b>(circle_points[i].y, circle_points[i].x)[1];
					//unsigned char r = drawing.at<Vec3b>(circle_points[i].y, circle_points[i].x)[2];
					//if (b == 0 && g == 255 && r == 0){
					if (drawing.at<uchar>((int)circle_points[i].x, (int)circle_points[i].y) == 127){
					//	circle(drawing, circle_points[i], 2, Scalar(255), 1, 8); // filled circle at the position
					}
				}
			}*/
		}
		imshow("MyVideo", bframe_original);
		imshow("ProcessedVideo", median);
		imshow("Contour", contour);
		imshow("Hull demo", drawing);
		waitKey(30);
	}
	comm.stopDevice();
	return 0;
}



