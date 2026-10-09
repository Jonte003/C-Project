#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"
#include "MarkerDetector.hpp"



cv::Mat GetMainWarpedMarker(cv::Mat& sourceFrame, std::vector<std::vector<cv::Point>>& allSortedQuads, int markerSize = 140) 
{
    if (allSortedQuads.empty()) {
        return cv::Mat();
    }

    std::vector<cv::Point2f> dstCorners = {
        cv::Point2f(0.0f, 0.0f),
        cv::Point2f(static_cast<float>(markerSize), 0.0f),
        cv::Point2f(static_cast<float>(markerSize), static_cast<float>(markerSize)),
        cv::Point2f(0.0f, static_cast<float>(markerSize))
    };

    // Convert list to point2f from point
    std::vector<cv::Point2f> cornersFloat(allSortedQuads[0].begin(), allSortedQuads[0].end());

    cv::Mat transformMatrix = cv::getPerspectiveTransform(cornersFloat, dstCorners);

    cv::Mat warpedOutput;
    cv::warpPerspective(sourceFrame, warpedOutput, transformMatrix, cv::Size(markerSize, markerSize));

    return warpedOutput;
}



int main()
{
    int width = 320;
    int height = 240;

    cv::Mat frame;

    int minArea = 500;
    int maxArea = 100000;

    int minQuadArea = 1000;

    std::vector<std::vector<cv::Point>> quadrilaterals;



    if (!CameraInput::init(0, width, height)) 
    {
        return 1;
    }

    while (CameraInput::getFrame(frame)) 
    {
        quadrilaterals.clear();        

        MarkerDetector::detectMarkers(frame, minArea, maxArea, minQuadArea, quadrilaterals);

        cv::Mat mat = GetMainWarpedMarker(frame, quadrilaterals, 140);
        
        if (!mat.empty()) 
        {
            cv::imshow("Warped Marker", mat);
        } 
        else if (cv::getWindowProperty("Warped Marker", cv::WND_PROP_VISIBLE) >= 1)
        {
            cv::destroyWindow("Warped Marker");
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

