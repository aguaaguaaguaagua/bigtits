#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "assert.h"
#include <opencv2\nonfree\features2d.hpp>
#include "opencv2\opencv.hpp"
using namespace std;
using namespace cv;



//#define InfoCollect       //收集像素信息的时候定义
//#define HistAnalyze       //做直方图分析的时候定义
#define mainTest
typedef  Mat OutputPic;