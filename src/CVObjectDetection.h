/**
 * @file
 * @brief Header file for CVObjectDetection class
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @ref License
 */

/* LICENSE
 *
 * Copyright (c) 2008-2019 OpenShot Studios, LLC
 * <http://www.openshotstudios.com/>. This file is part of
 * OpenShot Library (libopenshot), an open-source project dedicated to
 * delivering high quality video editing and animation solutions to the
 * world. For more information visit <http://www.openshot.org/>.
 *
 * OpenShot Library (libopenshot) is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * OpenShot Library (libopenshot) is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenShot Library. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define int64 opencv_broken_int
#define uint64 opencv_broken_uint
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#undef uint64
#undef int64
#include "Json.h"
#include "ProcessingController.h"
#include "Clip.h"
#include "protobuf_messages/objdetectdata.pb.h"

#include "sort_filter/sort.hpp"

namespace openshot
{
    // Stores the detected object bounding boxes and its properties.
    struct CVDetectionData{
        CVDetectionData(){}
        CVDetectionData(std::vector<int> _classIds, std::vector<float> _confidences, std::vector<cv::Rect_<float>> _boxes, size_t _frameId){
            classIds = _classIds;
            confidences = _confidences;
            boxes = _boxes;
            frameId = _frameId;
        }
        size_t frameId;
        std::vector<int> classIds;
        std::vector<float> confidences;
        std::vector<cv::Rect_<float>> boxes;
    };

    /**
     * @brief This class runs trought a clip to detect objects and returns the bounding boxes and its properties.
     *
     * Object detection is performed using YoloV3 model with OpenCV DNN module
     */
    class CVObjectDetection{

        private:

        cv::dnn::Net net;
        std::vector<std::string> classNames;
        float confThreshold, nmsThreshold;

        std::string classesFile;
        std::string modelConfiguration;
        std::string modelWeights;
        std::string processingDevice;
        std::string protobuf_data_path;

        SortTracker sort;

        uint progress;

        size_t start;
        size_t end;

        bool error = false;

        /// Will handle a Thread safely comutication between ClipProcessingJobs and the processing effect classes
        ProcessingController *processingController;

        void setProcessingDevice();

        // Detect onbects on a single frame
        void DetectObjects(const cv::Mat &frame, size_t frame_number);

        bool iou(cv::Rect pred_box, cv::Rect sort_box);

        // Remove the bounding boxes with low confidence using non-maxima suppression
        void postprocess(const cv::Size &frameDims, const std::vector<cv::Mat>& out, size_t frame_number);

        // Get the names of the output layers
        std::vector<cv::String> getOutputsNames(const cv::dnn::Net& net);

        public:

        std::map<size_t, CVDetectionData> detectionsData;

        CVObjectDetection(std::string processInfoJson, ProcessingController &processingController);

        // Iterate over a clip object and run inference for each video frame
        void detectObjectsClip(openshot::Clip &video, size_t start=0, size_t end=0, bool process_interval=false);

        CVDetectionData GetDetectionData(size_t frameId);

        /// Protobuf Save and Load methods
        // Save protobuf file
        bool SaveObjDetectedData();
        // Add frame object detection data into protobuf message.
        void AddFrameDataToProto(pb_objdetect::Frame* pbFrameData, CVDetectionData& dData);

        // Get and Set JSON methods
        void SetJson(const std::string value); ///< Load JSON string into this object
        void SetJsonValue(const Json::Value root); ///< Load Json::Value into this object

        // Load protobuf file (ONLY FOR MAKE TEST)
        bool _LoadObjDetectdData();
    };

}
