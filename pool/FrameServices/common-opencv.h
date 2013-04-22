#pragma once

#include <exception>

#include <opencv2\core\core_c.h>
#include <opencv2\core\core.hpp>

#include "FrameProvider.h"

#define CHANNELS_N 3
#define DEPTH IPL_DEPTH_8U

namespace FrameServices
{
    inline void ImageFromProvider(IplImage **image, const FrameHandle &handle)
    {
        // Prepare source IplImage to be used by the alghorithm using the provider info
        CvSize size = cvSize(handle.Width, handle.Height);

        IplImage *imageToRevise;
        if (*image == NULL)
        {
            imageToRevise = cvCreateImageHeader(size, DEPTH, CHANNELS_N);
            if (imageToRevise == NULL)
                throw std::exception("Out of memory");
        }
        else
        {
            imageToRevise = *image;
            cvInitImageHeader(imageToRevise, size, DEPTH, CHANNELS_N);
        }

        imageToRevise->imageData = (char *)handle.Data[0];

        *image = imageToRevise;
    }

    inline cv::Mat MatFromProvider(const FrameHandle &handle)
    {
        cv::Mat ret(handle.Width, handle.Height, CV_8UC3,
            (char *)handle.Data[0]);
        return ret;
    }
}