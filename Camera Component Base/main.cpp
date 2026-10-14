#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"
#include "MarkerDetector.hpp"


int main()
{
    int width = 640;
    int height = 480;

    cv::Mat frame;

    int minArea = 500;
    int maxArea = 100000;

    int minQuadArea = 1000;


    if (!CameraInput::init(0, width, height)) 
    {
        return 1;
    }

    while (CameraInput::getFrame(frame)) 
    {
        std::vector<QuadWithID> detectedMarkers = MarkerDetector::detectMarkers(frame, minArea, maxArea, minQuadArea);
        
        for (int i = 0; i < detectedMarkers.size(); ++i) 
        {
            std::cout << "Detected Marker ID: " << detectedMarkers[i].id << std::endl;
        }
        
        cv::imshow("Camera Feed", frame);

        if (cv::waitKey(20) == 27) 
        {
            break;
        }
    }

    CameraInput::release();
    cv::destroyAllWindows();
    return 0;



    
}