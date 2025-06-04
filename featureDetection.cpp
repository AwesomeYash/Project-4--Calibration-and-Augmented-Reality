/*

PRCV_Project 4
Code by - Priyanshu Ranka
Subject - Pattern Recognition and Computer Vision
Professor - Prof. Bruce Maxwell
Date - 03/13/2025
Description - This code is for Task 7. It compares the Harris detection, Shi-Tomasi and ORB.

*/

// Include the necessary libraries
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <stdio.h>
#include "utils.h"

// Namespaces
using namespace std;
using namespace cv;

int main() 
{
    // For video capture
    VideoCapture cap(0);  
    
	// Check if the webcam is opened - DEBUGGING
    if (!cap.isOpened()) 
    {
        cerr << "Error: Cannot open webcam!" << endl;
        return -1;
    }

	// Variables for the frame and gray image 
    Mat frame, gray;
    
	// Method for feature detection
    int method = 0;  // Default to Harris Corners
    cout << "Press '1' for Harris Corners, '2' for Shi-Tomasi, '3' for ORB." << endl;

	int frameNumber = 0;

    while (true) 
    {
		// Read the frame
        cap >> frame;
        if (frame.empty()) 
            break;

		// Convert to gray
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        Mat output = frame.clone();

		// Variables for the corners and keypoints
        vector<Point2f> corners;
        vector<KeyPoint> keypoints;

		string filename;

        // Harris Corners
        if (method == 1) 
        { 
			// Detect corners using in-build function for Harris
            Mat harris;
            cornerHarris(gray, harris, 2, 3, 0.08);
            normalize(harris, harris, 0, 255, NORM_MINMAX);

            for (int i = 0; i < harris.rows; i++) 
            {
                for (int j = 0; j < harris.cols; j++) 
                {
                    if ((int)harris.at<float>(i, j) > 150) // Threshold
                        circle(output, Point(j, i), 3, Scalar(0, 0, 255), -1);
                }
            }

			filename = "Harris" + std::to_string(frameNumber);
        }

        // Shi-Tomasi
        else if (method == 2) 
        { 
			// Detect corners/features using in-build function
            goodFeaturesToTrack(gray, corners, 200, 0.01, 10);
            for (auto& corner : corners)
                circle(output, corner, 3, Scalar(255, 0, 0), -1);

            filename = "Shi-Tomasi" + std::to_string(frameNumber);
        }

		// ORB
        else if (method == 3) 
        { 
			// Detect keypoints using in-build function of ORB
            Ptr<ORB> orb = ORB::create(300);
            orb->detect(gray, keypoints);
            drawKeypoints(frame, keypoints, output, Scalar(0, 255, 0), DrawMatchesFlags::DEFAULT);

            filename = "ORB " + std::to_string(frameNumber);
        }

        putText(output, "Press 1: Harris, 2: Shi-Tomasi, 3: ORB", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        imshow("Feature Detection", output);

        char key = waitKey(10);
        if (key == 'a')                         // Normal frame
            imshow("Feature Detection", frame);
            
        if(key == 'q') 
            break;  // ESC or 'q' to exit

        else if (key == 's' )
        {
            // Save corners and 3D world points
            saveFrame(output, ".", filename);
            cout << "Saved frame #" << frameNumber << endl;
			frameNumber++;
        }
		else if (key == '1') method = 1;    // Harris
		else if (key == '2') method = 2;	// Shi-Tomasi
		else if (key == '3') method = 3;	// ORB
    }

    cap.release();
    return 0;
}