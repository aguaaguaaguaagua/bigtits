#include "stdafx.h"
#include "ROI.h"

void Extractor::lineContour( const vector<Vec4i>& lineD)
{

	if(lineD.empty())return;
	xHull.clear();
	vector<Point> vertex;
	
	for(size_t i=0;i<lineD.size();i++)
	{
		vertex.push_back(Point(lineD[i][0],lineD[i][1]));
		vertex.push_back(Point(lineD[i][2],lineD[i][3]));
	}
	//计算凸包
	convexHull(vertex,xHull,false);
	
	//在原始图像上画凸包, it's a debug part
	/***
	/*
	/*
	int hullSize=xHull.size();
	for(int i=0;i<hullSize;i++)
	{
		line(frame,xHull[i%hullSize],xHull[(i+1)%hullSize],Scalar(255,0,0),2,8);
	}
	*/
	

	
}

void Extractor::getGOIrgb()
{
	if(xHull.empty()) return;

	if(frame.channels()!=3)
		cvtColor(frame,frame,CV_GRAY2BGR);
	Point pt;
	int loc;
	int cn=frame.channels();
	
	Vec3b *p;
	
	
	for(int i=0;i<frame.rows;i++)
	{
		
		p=frame.ptr<Vec3b>(i);
		for(int j=0;j<frame.cols;j++)
		{
			pt=Point(j,i);
			loc=pointPolygonTest(xHull,pt,false);
			//如果像素点在凸包区域里，存入矩阵
			if(loc<0)
			
				p[j]=Vec3b(0,0,0);
			
		}
	}

	show(frame);
	//assert(!xROI.empty());
	//xmROI=Mat(xROI,1);
	
	
}
void Extractor::histInROI()
{
	
	Mat binary,edges;
	
	Canny(frame,edges,50,100);
	imshow("shit",edges);
	waitKey(0);
	adaptiveThreshold(frame,binary,255,THRESH_BINARY,ADAPTIVE_THRESH_GAUSSIAN_C,7,0);
	imshow("shit",binary);
	waitKey(0);
}
void Extractor::getGOIgray()
{
	if(xHull.empty()) return;
	assert(frame.channels()==1);
	Point pt;
	int loc;
	
	uchar *p;
	for(int i=0;i<frame.rows;i++)
	{
		
		p=frame.ptr<uchar>(i);
		for(int j=0;j<frame.cols;j++)
		{
			pt=Point(j,i);
			loc=pointPolygonTest(xHull,pt,false);
			//如果像素点在凸包区域里，存入矩阵
			if(loc<0)
			 p[j]=0;
			
			
		}
	}
	//
	//show(frame);
}

//////////////////////////////////////////////////////////////////////////////////