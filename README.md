# Camera Calibration and Object Projection

**Author**: Priyanshu Ranka  
**Course**: Pattern Recognition and Computer Vision (PRCV)  
**Instructor**: Prof. Bruce Maxwell  
**Semester**: Spring 2025 | Northeastern University  
**OS Used**: Windows 11 | Visual Studio 2022  
**Time Travel Days Used**: 3 🚀  

---

## 📌 Overview

This project demonstrates **camera calibration**, **3D object projection**, and **feature detection** using C++ and OpenCV. The goal is to estimate intrinsic and extrinsic camera parameters and apply them to project 3D objects onto a 2D plane. Additionally, the project includes detection of keypoints using modern feature extraction algorithms.

---

## 🗂️ File Structure

Project/

├── calibration.csv # Intrinsic & extrinsic camera parameters

├── cameraCalibration.cpp # Estimates camera matrix & distortion coefficients

├── objProjection.cpp # Projects 3D objects onto 2D image using calibration data

├── featureDetection.cpp # Detects image features using SIFT, ORB, or SURF

├── Project 4 Report.pdf # Project report detailing methods and results

└── README.md # Project documentation (this file)


---

## 🧠 Core Concepts

- **Camera Calibration**:  
  Uses multiple calibration images to compute the camera matrix and distortion coefficients.

- **Object Projection**:  
  Applies computed calibration data to transform and render 3D points in 2D space.

- **Feature Detection**:  
  Implements algorithms such as **SIFT**, **ORB**, or **SURF** to extract and visualize keypoints from images.

---

## 💻 How to Run

### 🧱 Compile the Programs

Make sure OpenCV is installed and properly configured.

```bash

g++ cameraCalibration.cpp -o cameraCalibration `pkg-config --cflags --libs opencv4`

g++ featureDetection.cpp -o featureDetection `pkg-config --cflags --libs opencv4`

g++ objProjection.cpp -o objProjection `pkg-config --cflags --libs opencv4`
```

## ▶️ Execute Programs

./cameraCalibration      # Perform camera calibration

./featureDetection       # Detect keypoints in input images

./objProjection          # Project a 3D object into a 2D image

---

## 📦 Requirements

C++ compiler (GCC, Clang, or Visual Studio)

OpenCV       (version 4.x recommended)

CMake        (optional, for structured builds)


---

## 📍 Tasks Covered

Task 1–4: Camera calibration using real images

Task 5–6: Projection of 3D objects onto 2D image space

Task 7:   Feature detection with visualized keypoints

---

## 📬 Contact
For collaboration, discussion, or feedback, feel free to connect with me on LinkedIn.

---

## 🔖 License
This project was developed as part of an academic course. For educational and demonstration purposes only.

Let me know if you'd like help converting this to a LaTeX report or adding sample images/gifs for better documentation!
