#pragma once

#include <OpenNI.h> //OpenNI2���g�p����(RGB-D)
#include <NiTE.h> //NITE2���g�p����(Skeleton)

#define _X64
//#undef _X64

#ifdef _X64
#pragma comment(lib, "C:\\Program Files\\OpenNI2\\Lib\\OpenNI2.lib")
#pragma comment(lib, "C:\\Program Files\\PrimeSense\\NiTE2\\Lib\\NiTE2.lib")
#else
#pragma comment(lib, "C:\\Program Files (x86)\\OpenNI2\\Lib\\OpenNI2.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\PrimeSense\\NiTE2\\Lib\\NiTE2.lib")

#endif