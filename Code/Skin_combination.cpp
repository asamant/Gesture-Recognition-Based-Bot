#include <iostream>
#include "tserial.h"
#include "bot_control.h"
#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;
RNG rng(12345);
serial comm; // serial communication with arduino

int main()
{
	
	comm.startDevice("COM4", 9600);

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
	namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	namedWindow("Hull demo", CV_WINDOW_AUTOSIZE);
	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);
	namedWindow("ProcessedVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	
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

	uchar r, g, b;
	while (1)
	{
		Mat contour = Mat(480, 640, CV_8UC1, Scalar(0));
		Mat drawing = Mat(480, 640, CV_8UC1, Scalar(0));

		// read a new frame from video
		bool bSuccess = cap.read(bframe_original);
		 //show the frame in "MyVideo" window
		for (int i = 0; i < dHeight; i+=2)
		{
			for (int j = 0; j < dWidth; j += 2)
			{

				b = bframe_original.at<Vec3b>(i, j)[0];
				g = bframe_original.at<Vec3b>(i, j)[1];
				r = bframe_original.at<Vec3b>(i, j)[2];
				if ((float)(r - g) / (r + g) <= 0.5 && (float)(r - g) / (r + g) >= 0.0 && (float)b / (r + g) <= 0.5)
				{
					if (b > 20 && g > 40 && r > 95)
					{
						if ((r - fminf(g, b)) > 15)
						{
							if ((r - g) > 15 && r > b)
							{
								/*bframe_processed.at<Vec3b>(i, j)[0] = 255;
								bframe_processed.at<Vec3b>(i, j)[1] = 255;
								bframe_processed.at<Vec3b>(i, j)[2] = 255;*/
								bframe_processed.at<uchar>(i, j) = 255;
													
								continue;
							}
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
				/*convexityDefects(contours[i], hullIdx[i], convDef[i]);
				
				for (int k = 0; k<hullIdx[i].size(); k++)
				{
					int ind = hullIdx[i][k];
					hull[i].push_back(contours[i][ind]);
				}*/

				//for (int k = 0; k<convDef[i].size(); k++)
				//{
				//	//if (convDef[i][k][3]>20 * 256)
				//	//{
				//		int ind_0 = convDef[i][k][0];
				//		int ind_1 = convDef[i][k][1];
				//		int ind_2 = convDef[i][k][2];
				//		defect_points[i].push_back(contours[i][ind_2]);
				//		cv::circle(contour, contours[i][ind_0], 5, Scalar(0, 255, 0), -1);
				//		cv::circle(contour, contours[i][ind_1], 5, Scalar(0, 255, 0), -1);
				//		cv::circle(contour, contours[i][ind_2], 5, Scalar(0, 0, 255), -1);
				//		cv::line(contour, contours[i][ind_2], contours[i][ind_0], Scalar(0, 0, 255), 1);
				//		cv::line(contour, contours[i][ind_2], contours[i][ind_1], Scalar(0, 0, 255), 1);
				//	//}
				//}
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

			int count = 0;
			int transition = 0;
			//cout << contours[ci].size() << endl;
			for (i = 0; i < contours[ci].size(); i++)
			{
				if (transition == 0 && norm(Mat(contours[ci][i]), Mat(centre)) > 140)
				{
					transition = 1;
					count++;
				}
				if (transition == 1 && norm(Mat(contours[ci][i]), Mat(centre)) < 140)
				{
					transition = 0;
					count++;
				}
			}
			count = (count / 2) - 1;
			if ((count)>=0)
				cout << "Gesture " << (count / 2) - 1 << endl;

			comm.send_data((unsigned char)count);
			
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


