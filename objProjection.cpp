/* 

PRCV_Project 4
Code by - Priyanshu Ranka
Subject - Pattern Recognition and Computer Vision
Professor - Prof. Bruce Maxwell
Date - 03/13/2025
Description - This code covers tasks 4, 5 and 6 which are pose-estimation, projecting on 3D axis and creating a virtual object.

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

// Chessboard parameters - Global variables
Size boardSize(9, 6);
float squareSize = 1.0; 

// Function to load calibration data from CSV
bool loadCalibrationFromCSV(Mat& cameraMatrix, Mat& distCoeffs) 
{
    ifstream file("calibration.csv");
	// Check if file is open - DEBUGGING
    if (!file.is_open()) 
    {
        cerr << "Error: Could not open calibration.csv!" << endl;
        return false;
    }

	// Read data from CSV
    string line;
    vector<double> cameraParams, distParams;

    // Read Camera Matrix
    getline(file, line); // Skip header "Camera Matrix"
    for (int i = 0; i < 3; i++) 
    {
        getline(file, line);
        stringstream ss(line);
        double val;
        while (ss >> val) 
        {
            cameraParams.push_back(val);
            if (ss.peek() == ',') ss.ignore();
        }
    }

    // Read Distortion Coefficients
    getline(file, line); // Skip empty line
    getline(file, line); // Skip header "Distortion Coefficients"
    getline(file, line);
    stringstream ss(line);
    double val;
    while (ss >> val) 
    {
        distParams.push_back(val);
        if (ss.peek() == ',') ss.ignore();
    }

	// Read Reprojection Error from CSV
    cameraMatrix = Mat(3, 3, CV_64F, cameraParams.data()).clone();
    distCoeffs = Mat(1, distParams.size(), CV_64F, distParams.data()).clone();

	// Close the file
    file.close();
    return true;
}

// Generate 3D chessboard object points
vector<Point3f> generateWorldFrame() 
{
	// Generate 3D points
    vector<Point3f> points;
    for (int i = 0; i < boardSize.height; i++) 
    {
        for (int j = 0; j < boardSize.width; j++) 
        {
            points.push_back(Point3f(j * squareSize, -i * squareSize, 0));
        }
    }
    return points;
}

// Generate 3D spiral building virtual object
vector<Point3f> generateVirtualObject() 
{
    vector<Point3f> obj;

	// Base square
    for(int i = 0; i <= 4; i = i + 1 )
    {
        obj.push_back(Point3f(2 * squareSize, -2 * squareSize, i)); // Top-right
        obj.push_back(Point3f(2 * squareSize, -4 * squareSize, i)); // Bottom-right
        obj.push_back(Point3f(4 * squareSize, -4 * squareSize, i)); // Bottom-left
        obj.push_back(Point3f(4 * squareSize, -2 * squareSize, i)); // Top-left        
    }
    
    // Base diamond
    for (int i = 1; i <= 5; i = i + 2)
    {
        obj.push_back(Point3f(1.5 * squareSize, -3 * squareSize, i)); // Right
        obj.push_back(Point3f(3 * squareSize, -4.5 * squareSize, i)); // Bottom
        obj.push_back(Point3f(4.5 * squareSize, -3 * squareSize, i)); // Left
        obj.push_back(Point3f(3 * squareSize, -1.5 * squareSize, i)); // Top
    }
    

	// Returning all points
    return obj;
}

// Draw the virtual object in image space
void drawVirtualObject(Mat& frame, vector<Point2f>& projectedPoints) 
{
	// To make the bases - 6 bases = 3 sqare + 3 diamond
    for (int i = 0; i < 23; i = i + 4)
    {
        line(frame, projectedPoints[i + 0], projectedPoints[i + 1], Scalar(255, 0, 0), 2);
        line(frame, projectedPoints[i + 1], projectedPoints[i + 2], Scalar(255, 0, 0), 2);
        line(frame, projectedPoints[i + 2], projectedPoints[i + 3], Scalar(255, 0, 0), 2);
        line(frame, projectedPoints[i + 3], projectedPoints[i + 0], Scalar(255, 0, 0), 2);
    }
    
    // To connect the bases
    for (int i = 0; i <= 11; i = i + 4)
    {
        line(frame, projectedPoints[i + 0], projectedPoints[i + 12], Scalar(0, 255, 0), 2);
        line(frame, projectedPoints[i + 1], projectedPoints[i + 13], Scalar(0, 0255, 0), 2);
        line(frame, projectedPoints[i + 2], projectedPoints[i + 14], Scalar(0, 0255, 0), 2);
        line(frame, projectedPoints[i + 3], projectedPoints[i + 15], Scalar(0, 0255, 0), 2);
    }
}

int main() 
{
	// Camera calibration variables
    Mat cameraMatrix, distCoeffs;
    // DEBUGGING
    if (!loadCalibrationFromCSV(cameraMatrix, distCoeffs)) 
    {
        return -1; // Exit if loading fails
    }

	// Open webcam
    VideoCapture cap(0);
	// DEBUGGING
    if (!cap.isOpened()) 
    {
        cerr << "Error: Could not open webcam" << endl;
        return -1;
    }

	// Frame and Grayscale Variables
    Mat frame, gray;
    vector<Point2f> corners;
    namedWindow("Pose Estimation", WINDOW_AUTOSIZE);

	// Generate 3D points and 3D Object
    vector<Point3f> objectPoints = generateWorldFrame();
    vector<Point3f> virtualObject = generateVirtualObject();
    Mat rvec, tvec;
    bool isPoseEstimated = false;

	int frameNumber = 0;

    while (true) 
    {
		// Capture frame
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        bool found = findChessboardCorners(gray, boardSize, corners);

        if (found) 
        {
            drawChessboardCorners(frame, boardSize, corners, found);

            // Estimate pose using solvePnP
            isPoseEstimated = solvePnP(objectPoints, corners, cameraMatrix, distCoeffs, rvec, tvec);
        }

        if (isPoseEstimated) 
        {
            // Define 3D axes points
            vector<Point3f> axesPoints = {
                Point3f(0, 0, 0),               // Origin
                Point3f(3 * squareSize, 0, 0),  // X-axis (red)
                Point3f(0, -3 * squareSize, 0), // Y-axis (green)
                Point3f(0, 0, 3 * squareSize)   // Z-axis (blue)
            };

            vector<Point2f> imagePoints;
            projectPoints(axesPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

            // Draw 3D axes
            line(frame, imagePoints[0], imagePoints[1], Scalar(0, 0, 255), 3); // X-axis (red)
            line(frame, imagePoints[0], imagePoints[2], Scalar(0, 255, 0), 3); // Y-axis (green)
            line(frame, imagePoints[0], imagePoints[3], Scalar(255, 0, 0), 3); // Z-axis (blue)

            // Project virtual object
            vector<Point2f> projectedVirtualObject;
            projectPoints(virtualObject, rvec, tvec, cameraMatrix, distCoeffs, projectedVirtualObject);

            // Draw virtual object
            drawVirtualObject(frame, projectedVirtualObject);
        }

		// Display the frame
        imshow("Pose Estimation", frame);
        
		// Save the frame
		if (waitKey(10) == 's')
		{
			saveFrame(frame, "output", to_string(frameNumber));
            frameNumber++;
		}

		// 'Esc' or 'q' to exit
        if (waitKey(10) == 27)
            break; // Exit on 'Esc'
    }

    cap.release();
    return 0;
}