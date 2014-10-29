// hehe.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "global.h"
#include "linesPro.h"
#include "FeatureDetect.h"

int thresh=20,max_thresh=200,
	length=70,max_length=700,
	gap=6,max_gap=60;
Mat frame,edge,frame_blur,frame_gray;
void HoughLinesDet(int, void*);
vector<Vec4i> lines;
int _tmain(int argc, _TCHAR* argv[])
{
	
	namedWindow("shit",CV_WINDOW_NORMAL);
	namedWindow("fuck",CV_WINDOW_NORMAL);
	VideoCapture cam("F:/FFOutput/test.wmv");
	cam.set(CV_CAP_PROP_POS_FRAMES,15000);
	if(!cam.isOpened()){cout<<"file fail being read"<<endl;return -1;}
	
	int framecount=0;
	while(1)
	{
		
		
		cam>>frame;
		framecount++;
		
		
		if(frame.empty()){cout<<"video ends"<<endl;break;}
		
		blur(frame,frame_blur,Size(3,3));
		cvtColor(frame_blur,frame_gray,CV_BGR2GRAY);
		Canny(frame_blur,edge,50,130,3);
		imshow("fuck",edge);
		createTrackbar("threshold","shit",&thresh,max_thresh,HoughLinesDet);
		createTrackbar("length","shit",&length,max_length,HoughLinesDet);
		createTrackbar("gap","shit",&gap,max_gap,HoughLinesDet);
		
		waitKey(0);
		
		
		
	}

	waitKey(0);
	return 0;}
void HoughLinesDet(int, void*)
{
	Mat show(frame);
	HoughLinesP(edge,lines,1,CV_PI/180,thresh,length,gap);
	lineProcess LP(lines,show);
	LP.drawLines();
	imshow("shit",LP.showPic);
		
}
	
	




