#ifndef _GLOBAL_H
#define __GLOBAL_H
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "assert.h"
#include "math.h"
#include <opencv2\nonfree\features2d.hpp>
#include "opencv2\opencv.hpp"
#include "tdef.h"



using namespace std;
using namespace cv;

//����������ط�


//#define InfoCollect       //�ռ�������Ϣ��ʱ����
//#define HistAnalyze       //��ֱ��ͼ������ʱ����
#define mainTest
//#define FeatureDetectTest
#define BIG_O_ASS_LARGE_NUMBER 10000
typedef  Mat OutputPic;
//this is for construct as mat with specific datatype buffer
//template<typename _Tp> void arrayToIntMat(Mat &out,_Tp *in,int ,int);
//void intArrayToMat(Mat &out,int *in); //coordinates mask


#define show(pic)   {imshow("pic1",pic);waitKey(0);}
#define show1(pic)  {imshow("pic2",pic);waitKey(0);}

#endif