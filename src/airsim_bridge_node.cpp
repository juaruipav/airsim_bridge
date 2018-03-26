#include <chrono>
#include <iostream>

#include <ros/ros.h>
#include <iostream>
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

#include "common/common_utils/StrictMode.hpp"

#include "airsim_bridge/ImageMosaicer.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "vehicles/car/api/CarRpcLibClient.hpp"
#include "common/common_utils/FileSystem.hpp"

using namespace ros;
using namespace std;
using namespace msr::airlib;

//Short typedefs
typedef ImageCaptureBase::ImageRequest ImageRequest;
typedef ImageCaptureBase::ImageResponse ImageResponse;
typedef ImageCaptureBase::ImageType ImageType;

//Parameters from the .launch file
string  airsim_ip;
int     airsim_port;
int     image_freq=1;
string  image_rgb_topic="";
int     num_images_rcv = 0;

// Global variables
MultirotorRpcLibClient * client;
NodeHandle             * n;
ros::Publisher           pub_rgb_camera;

int main(int argc, char **argv)
{
 
   ros::init(argc, argv, "airsim_bridge");
   n = new ros::NodeHandle("~");

   n->getParam("airsim_ip", airsim_ip);
   n->getParam("airsim_port", airsim_port);
   n->getParam("image_freq", image_freq);
   n->getParam("image_rgb_topic", image_rgb_topic);


  if(image_rgb_topic.length() > 0)
    pub_rgb_camera = n->advertise<sensor_msgs::Image>(image_rgb_topic.c_str(),100);
  else{
    ROS_ERROR("Topic not set corretly.. exiting");
    return -1;
  }

  if(image_freq == 0)
      image_freq = 1;

   client = new msr::airlib::MultirotorRpcLibClient(airsim_ip, airsim_port);

   ROS_INFO("Airsim_brdige started! Connecting to:");
   ROS_INFO("IP: %s", airsim_ip.c_str());
   ROS_INFO("Port: %d", airsim_port);

   client->confirmConnection();

  // Once connection is established, spin, get images and pub them if subscribers
  ros::Rate r(image_freq);
  while (ros::ok())
  {
      //Creating the request, only interesed (by the moment) in RGB camera
      std::vector<ImageRequest> request = { ImageRequest(0, ImageType::Scene) };

      //Getting the response
      const std::vector<ImageResponse>& response = client->simGetImages(request);

      //For each response, we extract the RGB cv::Mat. Process multiples cameras in the future
      if (response.size() > 0) {
           for (const ImageResponse& image_info : response) {
//                std::cout << "Image uint8 size: " << image_info.image_data_uint8.size() << std::endl;

                cv::Mat rgb_image = cv::imdecode(image_info.image_data_uint8, cv::IMREAD_COLOR);

                // Publishing image as a sensor_msgs::CompressedImage. In the future add SensorImage.
                cv_bridge::CvImage imageCV;
                std_msgs::Header header; // empty header
                header.stamp = ros::Time::now(); // time
                imageCV = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, rgb_image);
                sensor_msgs::Image imageCompressed;
                imageCV.toImageMsg(imageCompressed);

                // If there is anybody listening, publish via ROS
                if(pub_rgb_camera.getNumSubscribers() > 0){
                    pub_rgb_camera.publish(imageCompressed);
                }

                num_images_rcv++;
                ROS_INFO(" # of images received from Airsim are: %d ",num_images_rcv);
            }
        }

     r.sleep();
     ros::spinOnce();
  }

  return 0;

}


