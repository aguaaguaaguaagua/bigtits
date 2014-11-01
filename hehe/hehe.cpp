// hehe.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "global.h"
#include "linesPro.h"
#include "FeatureDetect.h"

int x,y;
  //全局图像
Mat frame_blur,frame_gray,frame_equal;

int _tmain(int argc, _TCHAR* argv[])
{
	

	
	VideoCapture cam("F:/FFOutput/test.wmv");
	namedWindow("shit",0);
	if(!cam.isOpened()){cout<<"file fail being read"<<endl;return -1;}
	       

	int framecount=11900;
	cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
	cam>>frame;
	Mat temp;
	frame.copyTo(temp);
	for(int i=0;i<temp.rows;i++)
		for(int j=0;j<temp.cols;j++)
			{if((temp.at<Vec3b>(i,j)[0]<150)||(temp.at<Vec3b>(i,j)[1]<175)||(temp.at<Vec3b>(i,j)[2]<175))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			if((temp.at<Vec3b>(i,j)[0]>250)&&(temp.at<Vec3b>(i,j)[1]>250)&&(temp.at<Vec3b>(i,j)[2]>250))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			if(temp.at<Vec3b>(i,j)[0]>temp.at<Vec3b>(i,j)[1])
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
		}
			
	imshow("shit",temp);
#ifdef HistAnalyze

	assert(readFile("F:/FFOutput/line1.txt")==1);
#endif
#ifdef InfoCollect                            //信息收集模式

	
	while(1)
	{

		cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
		framecount+=150;
		
		cam>>frame;
		if(framecount>20100){cout<<"video ends"<<endl;foutBg.close();foutLine.close();break;}
		PixelInfo(frame,"shit");              //frame在这个函数里没有用，只是表示准备收集信息的帧
		

		waitKey(0);
		
		
		

	}

#endif



	waitKey(0);
	return 0;}



	
	




