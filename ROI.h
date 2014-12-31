
#ifndef _OBJECTDETECT2_H
#define _OBJECTDETECT2_H

#include "global.h"
//ROI setup and detection shit
class Extractor
{
public:
	
	Extractor(Mat _inputFrame):frame(_inputFrame),xHull(),xROI(){};
	~Extractor(){xHull.clear();xROI.clear();};
	//���ֱ��͹����һ��ֱ��һ��͹��
	void lineContour(const vector<Vec4i>& );
	//��͹������Ĳ�������Ϊ����Ȥ�����򣬲�����һ��������
	void getGOIrgb();
	void getGOIgray();
	//����͹���������ֱ��ͼ
	void histInROI();
	//���÷ָ��㷨��������ָ�

public:

	//input buffer
	Mat frame;

private:
	


	//�������ֱ�ߵ�͹��
	vector<Point> xHull;
	//����Ȥ������͹���ڲ�������
	vector<Vec3b> xROI;
	Mat xmROI;
	
};


////////////////////////////////////////////////////////////////////////////////


#endif