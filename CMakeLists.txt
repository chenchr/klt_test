cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DNDEBUG")

# find_package(OpenCV 3.0 QUIET)
# if(NOT OpenCV_FOUND)
#    find_package(OpenCV 2.4.3 QUIET)
#    if(NOT OpenCV_FOUND)
#       message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
#    endif()
# endif()
find_package(OpenCV 2.4.3 REQUIRED)

add_library(klt SHARED
convolve.c 
error.c 
pnmio.c 
pyramid.c 
selectGoodFeatures.c 
storeFeatures.c 
trackFeatures.c 
klt.c 
klt_util.c 
writeFeatures.c
)
target_link_libraries(klt m)

add_executable(example1 example1.c)
target_link_libraries(example1 klt)

add_executable(euroc euroc.cpp)
target_link_libraries(euroc 
klt 
/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so
/usr/lib/x86_64-linux-gnu/libopencv_highgui.so
/usr/lib/x86_64-linux-gnu/libopencv_core.so
${OpenCV_LIBS}
)

add_executable(euroc_cv euroc_opencv.cpp)
target_link_libraries(euroc_cv 
klt 
${OpenCV_LIBS}
/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so
/usr/lib/x86_64-linux-gnu/libopencv_highgui.so
/usr/lib/x86_64-linux-gnu/libopencv_core.so
/usr/lib/x86_64-linux-gnu/libopencv_video.so
)

add_executable(test test.cpp)
target_link_libraries(test 
opencv_videostab
opencv_video
opencv_ts
opencv_superres
opencv_stitching
opencv_photo
opencv_objdetect
opencv_nonfree
opencv_ml
opencv_legacy
opencv_imgproc
opencv_highgui
opencv_gpu
opencv_flann
opencv_features2d
opencv_core
opencv_contrib
opencv_calib3d)
message(${OpenCV_LIBS})