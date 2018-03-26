cmake_minimum_required(VERSION 2.8.3)
project(airsim_bridge)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  message_generation
  std_msgs
  image_transport
  cv_bridge
  dynamic_reconfigure
)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

add_message_files(
  FILES
  ImageMosaicer.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
  sensor_msgs
  airsim_bridge
)


## Access to the precompiled libraries from Airsim
SET (AIRSIM_ADDRESS ${CMAKE_CURRENT_SOURCE_DIR}/external/AirLib)

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR} )


## System dependencies are found with CMake's conventions
#find_package(Boost REQUIRED COMPONENTS system)

# Custom target to include headers and launch files in QtCreator
add_custom_target(dtarget_airsim_bridge SOURCES ${HDRS} ${SRCS} ${LNCH})

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)


include_directories(
  ${catkin_INCLUDE_DIRS}
  ${AIRSIM_ADDRESS}/include
  ${AIRSIM_ADDRESS}/include/api
  ${AIRSIM_ADDRESS}/include/controllers
  ${AIRSIM_ADDRESS}/deps/rpclib/include/rpc
  ${AIRSIM_ADDRESS}/deps/eigen3
  ${AIRSIM_ADDRESS}/include
  ${OPENCV_INCLUDE_DIRS}
)

link_directories(
    ${AIRSIM_ADDRESS}/lib/x64/Debug
    )

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES choose_data
  CATKIN_DEPENDS message_runtime
#  DEPENDS system_lib

)

## Declare a cpp executable
add_executable(airsim_bridge_node src/airsim_bridge_node.cpp)

# set(CUDA_USE_STATIC_CUDA_RUNTIME OFF) #behzad added, need this always


## Specify libraries to link a library or executable target against
 target_link_libraries(airsim_bridge_node
   ${catkin_LIBRARIES}
    AirLib
    MavLinkCom
    rpc
    pthread
    ${OpenCV_LIBS}
 )
 

