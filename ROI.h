
#ifndef _OBJECTDETECT2_H
#define _OBJECTDETECT2_H

#include "global.h"
//ROI setup and detection shit
class Extractor
{
public:
	
	Extractor(Mat _inputFrame):frame(_inputFrame),xHull(),xROI(){};
	~Extractor(){xHull.clear();xROI.clear();};
	//求出直线凸包，一束直线一个凸包
	void lineContour(const vector<Vec4i>& );
	//将凸包里面的部分设置为感兴趣的区域，并存入一个矩阵中
	void getGOIrgb();
	void getGOIgray();
	//计算凸包区域里的直方图
	void histInROI();
	//采用分割算法进行异物分割

public:

	//input buffer
	Mat frame;

private:
	


	//求出来的直线的凸包
	vector<Point> xHull;
	//感兴趣的区域，凸包内部的区域
	vector<Vec3b> xROI;
	Mat xmROI;
	
};


////////////////////////////////////////////////////////////////////////////////


#endif