// hehe.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "global.h"
#include "linesPro.h"
#include "FeatureDetect.h"

int x,y;
  //ȫ��ͼ��
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
#ifdef InfoCollect                            //��Ϣ�ռ�ģʽ

	
	while(1)
	{

		cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
		framecount+=150;
		
		cam>>frame;
		if(framecount>20100){cout<<"video ends"<<endl;foutBg.close();foutLine.close();break;}
		PixelInfo(frame,"shit");              //frame�����������û���ã�ֻ�Ǳ�ʾ׼���ռ���Ϣ��֡
		

		waitKey(0);
		
		
		

	}

#endif



	waitKey(0);
	return 0;}



	
	




