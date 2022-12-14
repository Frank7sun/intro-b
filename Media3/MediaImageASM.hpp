#ifndef MEDIACOM_ASM_H // 二重インクルードを防ぐ
#define MEDIACOM_ASM_H

#include "MediaImage.hpp"
#include "asmfitting.h"
#include "asmlibrary.h"

#ifdef _DEBUG
#pragma comment(lib, "asmlibraryD.lib")
#else
#pragma comment(lib, "asmlibrary.lib")
#endif



//以下、オリジナルのプログラムに定義されていた関数など

//vjfacedetect
/**
 Load adaboost cascade file for detect face.
 @param cascade_name Filename the cascade detector located in
 @return false on failure, true otherwise
*/
bool init_detect_cascade(const char* cascade_name = "haarcascade_frontalface_alt2.xml");


/**
 Release the memory of adaboost cascade face detector
*/
void destory_detect_cascade();

/**
 Detect only one face from image, and this human face is located as close as to the center of image
 @param shape return face detected box which stores the Top-Left and Bottom-Right points, so its \a NPoints() = 2 here
 @param image the image resource
 @return false on no face exists in image, true otherwise
*/
bool detect_one_face(asm_shape& shape, const IplImage* image);


/**
 Detect all human face from image.
 @param shapes return face detected box which stores the Top-Left and Bottom-Right points, so its \a NPoints() = 2 here
 @param n_shapes the numbers of faces to return
 @param image the image resource
 @return false on no face exists in image, true otherwise
*/
bool detect_all_faces(asm_shape** shapes, int& n_shapes, const IplImage* image);

/**
 Release the shape resource allocated by detect_all_faces().
 @param shapes the ptr of asm_shape []
*/
void free_shape_memeory(asm_shape** shapes);


//video_camera.h
/**
 Open an AVI file.
 @param filename the video file located in
 @return -1 on failure, frame count of the video otherwise
*/
int open_video(const char* filename);

/**
 Get one certain frame of video.
 @param frame_no  which frame
 @return Internal IplImage ptr
*/
IplImage* read_from_video(int frame_no);

/**
 Close avi and release memory.
 */
void close_video();


/**
 Capture from live camera.
 @param index camara index
 @return false on failure, true otherwise
 */
bool open_camera(int index);

/**
 Get one certain frame of live camera.
 @return Internal IplImage ptr
*/
IplImage* read_from_camera();

/**
 Close camara and release memory.
 */
void close_camera();

#endif /* MEDIACOM_OPENCV_H */#pragma once
