/* 

PRCV_Project 4
Code by - Priyanshu Ranka
Subject - Pattern Recognition and Computer Vision
Professor - Prof. Bruce Maxwell
Date - 03/13/2025
Description - This code simply captures the chessboard images and saves them for calibration purposes. It also performs pose estimation.

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

// Global variables
Size boardSize(9, 6);
float squareSize = 1.0;

vector<vector<Point2f>> cornerList; // List of detected corners
vector<vector<Vec3f>> pointList;    // List of world points
vector<Mat> calibrationImages;      // List of calibration images

// Function to generate world frame from 2D points
vector<Vec3f> generateWorldFrame() 
{
    vector<Vec3f> points;
    for (int i = 0; i < boardSize.height; i++) 
    {
        for (int j = 0; j < boardSize.width; j++) 
			points.push_back(Vec3f(j * squareSize, -i * squareSize, 0));   // -i to flip the board because first corner is top-left
    }
    return points;
}

// Main function
int main()
{
	// Open the webcam (using phone camera)
    VideoCapture cap(0);
	
    // Check if the webcam is opened - DEBUGGING
    if (!cap.isOpened()) 
    {
        cerr << "Error: Could not open webcam" << endl;
        return -1;
    }

	// Variables for capturing frames
    Mat frame, gray;
    vector<Point2f> corners;
    
	// Camera matrix (intrinsic parameters) -  Specified in the assignment
	Mat cameraMatrix = (Mat_<double>(3, 3) << 1, 0, frame.rows / 2, 0, 1, frame.cols / 2, 0, 0, 1);      
	Mat distCoeffs;                 // Distortion coefficients 
	vector<Mat> rvecs, tvecs;       // Rotation and translation vectors

	int frameNumber = 0;            // Frame counter - To save frames
    bool isCalibrated = false;      // Flag to track calibration status
    Mat rvec, tvec;                 // Store rotation and translation vectors

    while (true) 
    {
		// Capture the frame
        cap >> frame;

		// Break if the frame is empty - DEBUGGING
        /*
        if (frame.empty())
            break;
        */

		// Convert the frame to grayscale and gives the corners
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        bool found = findChessboardCorners(gray, boardSize, corners);

		// Draw the corners on the frame (9x6 points)
        if (found) 
            drawChessboardCorners(frame, boardSize, corners, found);
       
        // If calibrated, project axes on each frame
        if (isCalibrated) 
        {
			// Define the axes points - local variable
            vector<Point3f> axesPoints = 
            {
                Point3f(0, 0, 0),               // Origin
                Point3f(3 * squareSize, 0, 0),  // X-axis (red)
                Point3f(0, -3 * squareSize, 0), // Y-axis (green)
                Point3f(0, 0, 3 * squareSize)   // Z-axis (blue)
            };

			// Project the axes points
            vector<Point2f> imagePoints;
            projectPoints(axesPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

            // Draw the axes on the frame
            line(frame, imagePoints[0], imagePoints[1], Scalar(0, 0, 255), 3); // X-axis (red)
            line(frame, imagePoints[0], imagePoints[2], Scalar(0, 255, 0), 3); // Y-axis (green)
            line(frame, imagePoints[0], imagePoints[3], Scalar(255, 0, 0), 3); // Z-axis (blue)
        }

        // Output
        namedWindow("Camera Calibration", WINDOW_AUTOSIZE);
        imshow("Camera Calibration", frame);

        string filename = "frame" + std::to_string(frameNumber);
        char key = waitKey(10);

        // To save the frames as result
        if (key == 's' && found) 
        {
            saveFrame(frame, ".", filename);
            frameNumber++;
            cornerList.push_back(corners);
            pointList.push_back(generateWorldFrame());
            calibrationImages.push_back(frame.clone());
            cout << "Saved calibration image " << cornerList.size() << endl;
            
            // Reset calibration flag
			isCalibrated = false;  
        }

		// Calibrate the camera if atleast 5 images are captured
        else if (key == 'c' && cornerList.size() >= 5) 
        {
            double error = calibrateCamera(pointList, cornerList, frame.size(), cameraMatrix, distCoeffs, rvecs, tvecs);

            cout << "Calibration Done!\n";
            cout << "Camera Matrix:\n" << cameraMatrix << endl;
            cout << "Distortion Coefficients:\n" << distCoeffs.t() << endl;
            cout << "Reprojection Error: " << error << endl;

            // Save calibration results
            saveCalibrationToCSV(cameraMatrix, distCoeffs, error);

            // Perform pose estimation after calibration
            bool success = solvePnP(pointList[0], cornerList[0], cameraMatrix, distCoeffs, rvec, tvec);
            if (success) 
            {
                isCalibrated = true;
                cout << "SolvePnP successful! Axes will be displayed.\n";
            }
            else 
            {
                cerr << "SolvePnP failed!\n";
            }
        }

        // Exit on 'Esc' or 'q'
        else if (key == 27 || key == 'q')
            break;
    }

    cap.release();
    return 0;
}