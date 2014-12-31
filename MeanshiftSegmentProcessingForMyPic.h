#pragma once
#ifndef _MEANSHIFTSEGMENTPROCESSING_H_
#define _MEANSHIFTSEGMENTPROCESSING_H_
#include "global.h"
#include "msImagePro.h"
//i use this shit on the image segment procedure
class msProcessing
{
public:
	msProcessing():if_segment(true),m_imgBuffer(0),m_sz(0),m_SegmentCount(0),m_pointCount(0),
	bytebuff(NULL),minRegion(20),m_sigmaS(7),m_sigmaR(10){};
	msProcessing(Mat _input,int _minRegion,double _sigmaS,double _sigmaR):m_sigmaS(_sigmaS),m_sigmaR(_sigmaR)
	,minRegion(_minRegion),input(_input),m_width(_input.cols),m_height(_input.rows){};
	void GetPicBuffer(Mat &in);//data interface
	void SendPicBuffer(Mat &out);
	void SendPicBuffer(Mat &out,BYTE * buff);
	void SendPicBuffer(Mat &out,int * buff);
void getSegmentCount();
void getSegmentPointCount();
void backgroundPrune();
void getSegmentLabel();
void getContourMask();
BYTE *bytebuff;
imageType m_imgType;
vector<uchar> m_imgBuffer; //the byte buffer of the image
void mainProcess();

void DoMeanShiftSegmentation(
	const int&								width,
	const int&								height,
	vector<UINT>&							segimg,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<int>&							labels,
	int&									numlabels);
void DoMeanShiftSegmentationBasedProcessing(
	const int&								width,
	const int&								height,
	const string&							filename,
	const vector<double>&					salmap,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<UINT>&							segimg,
	vector<UINT>&							segobj);
//filter large bg
void filterWithMask(Mat &input,vector<Point>& Mask,uchar *color);
void GetThePointMask(int label,int *labels);
void GetTheColorAtSpecificLabel(Mat &inputForFindingTheSpecificPixel,int label,uchar *color,int *labels);
void GetTheLabelByPoint(int,int);
private:
	Mat input;
	bool if_segment;
	int m_SegmentCount;
	vector<long> m_pointCount;
	int m_sz,m_width,m_height;//input img size
	Mat mask;//use for contour mask
	////input parameters
	const int minRegion;
	const double m_sigmaS;
	const double m_sigmaR;
	//it is for test
	vector<Point> pointsSelected;
};
/////////////////////////////////////////////////////////////

#endif