// hehe.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "global.h"
#include "linesPro.h"
#include "Hist.h"
#include "FeatureDetect.h"

int x,y;
  //全局图像
Mat frame_blur,frame_gray,frame_equal,color,edges;

int _tmain(int argc, _TCHAR* argv[])
{
	

	
	VideoCapture cam("F:/FFOutput/test.wmv");
	namedWindow("shit",0);
	namedWindow("fuck",0);
	if(!cam.isOpened()){cout<<"file fail being read"<<endl;return -1;}
	       

	int framecount=6500;
#ifdef mainTest	
	cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
	cam>>frame;
	vector<Mat> cn;
	linesPro.colorFilter1(frame,frame);
	Canny(frame,edges,50,100);
	cvtColor(edges,edges,CV_GRAY2BGR);
	frame+=edges;
	linesPro.lineDilate(edges,edges,0);
	imshow("fuck",edges);

	

	

#endif
#ifdef HistAnalyze

	assert(readFile("F:/FFOutput/line1.txt")==1);
#endif
#ifdef InfoCollect                            //信息收集模式

	
	while(1)
	{

		cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
		framecount+=150;
		
		cam>>frame;
		if(frame.empty()){cout<<"video ends"<<endl;foutBg.close();foutLine.close();break;}
		PixelInfo(frame,"shit");              //frame在这个函数里没有用，只是表示准备收集信息的帧
		

		waitKey(0);
		
		
		

	}

#endif



	waitKey(0);
	return 0;}



	
	




