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
	       

	int framecount=0;
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



	
	




