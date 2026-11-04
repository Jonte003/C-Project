The Project reads a 4x4 Aruco code and displays its coresponding object in Unity, the object can rotate/grow/shrink depending on the distance to marker and the rotation of the marker. The Project only supports codes ranging from 0-2.

To use the program, start both program and have them running at the same time, use the Instuctions below.
Display a 4x4 ArUco (Digitaly or printed) code with id ranging from 0-2, an object will be spawned in Unity and will have the same rotation as the marker. If the marker is lost, the object will disappear.

The folder both includes builded ready to use alternatives for both applications, I will give instructions that works for both.

Using builded files:
1. Open build file folder
2. Start the unity.exe file
3. Start the camera_grab.exe file

Using source code files
1. Build and run C++ application using the following steps:
	1. Open VS dev. terminal
	2. Set directory as active directory: cd ....\C-Project\Source Code\OpenCV Application

	3. Run the following commands, file location may need to be changed:
		cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="%USERPROFILE%/vcpkg/scripts/buildsystems/vcpkg.cmake"

		cmake --build build --config debug

		.\build\debug\camera_grab.exe

2. Start unity application through unity hub.


Requirments:
 - Computer with Windows 10/11
 - Webcam
 - 4x4 ArUco Marker (printed or digitaly) ranging from ID 0-2
If ran with the source code:
 	- Unity Editor 6000.3.11f1
 	- OpenCV (manged through vcpkg)

