Group number and project title: 
 - Basic object rotation

Team members:
 - Kalle Lööf, Jonathan Hettinger

Repository URL: 
 - https://github.com/Jonte003/C-Project

Final tag:
 - final-submission

Assessed commit hash:
 - 

Main interaction:
 - Rotate an 4x4 Aruco Code ranging from id 0-2 and move it around to rotate it on screen.

Required equipment or software:
 - Unity 6000.3.11.f1 (with project files OR built Unity .exe file)
 - C++ source code (to be compiled into camera_grab.exe using CMake, a C++ compiler, vcpkg, and OpenCV)
 - Webcam 
 
Known limitations: 
 - The program can only accept one marker, if more than one is displayed, only one will be read.
 - The program can reads all 4x4 ArUco codes, but only codes with id between 0-2 will be recoginzed and displayed in unity.
 - The rotation of the marker can be a bit unstable at times, somethimes jumping +-5 degrees (depending a bit on marker size, bigger works      
   better), The ID and zoom is both extremly reliable tho, and that was our goal from the beginning (rotation was something we just added at the end because we had the data for it).

Presentation date and time: 
 - Friday 6/11-2025 14:00
