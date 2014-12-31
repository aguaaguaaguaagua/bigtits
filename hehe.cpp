// hehe.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include "global.h"
#include "objDetect.h"

#include "lineExtend.h"
#include "linesPro.h"
#include "FeatureDetect.h"
#include "salientDetect.h"
#include "MeanshiftSegmentProcessingForMyPic.h"
#include "time.h"


int x,y;
Mat src,frame;

VideoCapture cam("F:/FFOutput/test.wmv");
VideoWriter sd("F:/FFOutput/tested1.avi",CV_FOURCC('M','J','P','G'),15.0,Size((int) cam.get(CV_CAP_PROP_FRAME_WIDTH),
              (int)cam.get(CV_CAP_PROP_FRAME_HEIGHT)));
  //全局图像
Mat img;
void imgPro(VideoCapture& cam,int framecount);
void standardImgPro(Mat &);
int _tmain(int argc, _TCHAR* argv[])
{
	
	namedWindow("pic1",0);
	namedWindow("pic2",0);
	if(!cam.isOpened()){cout<<"file fail being read"<<endl;return -1;}
	if(!sd.isOpened()){cout<<"file fail being write"<<endl;return -1;}       

	int framecount=16501;
	cam.set(CV_CAP_PROP_POS_FRAMES,framecount);


#ifdef mainTest	
	while(1)
	{
		if(framecount>16501){cout<<"ends"<<endl;return -1;}
		//imgPro(cam,framecount);
		//linesPro.lineDetection(img);
		


		/////--------------------------------------------------//
		//frame=imread("C:/Users/Administrator/Documents/Tencent Files/844730830/FileRecv/Saliency_MC_test2/Saliency_MC_test2/MC_test1/shit2.jpg");
		//msProcessing msPro(frame,20,7,10);
		//msPro.mainProcess();
		
		int start=clock();
		
		Mat test=imread("C:/Users/Administrator/Desktop/Temp/gradTest.jpg");

		

		Mat edges;
		lineProcess linePro;

		

		Canny(test,edges,50,100,3);
		
		
		vector<Vec4i> lines;
		HoughLinesP(edges,lines,1,CV_PI/180,50,200,10);

		show(edges);

		cvtColor(test,test,CV_BGR2GRAY);
		threshold(test,test,10,255,THRESH_BINARY);

		//derived factory
		fucker :: detectorFactory *detectorFac = new fucker::detectorFactoryShaped;
		//relevant detector
		fucker :: detector* curDetector = detectorFac ->createDetector(70,test,51,7);
		//call it
		curDetector ->alongTheLine(lines);
		int end=clock()-start;
		cout<<end<<endl;

		
		
		
		//------------------------------------------------------------------------//
		
		//standardImgPro(frame);
		
		framecount++;
		cout<<"frame:"<<framecount<<endl;

	}
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
		PixelInfo(frame,"pic1");              //frame在这个函数里没有用，只是表示准备收集信息的帧
		

		waitKey(0);
		
		
		

	}

#endif

#ifdef FeatureDetectTest

	//Mat src2=imread("C:/Users/Administrator/Documents/Tencent Files/844730830/FileRecv/src3.png",0);
	
	//FlanAndSurfMatcherExtract(src1,src2,"SURF");

#endif
	

	return 0;}


void imgPro(VideoCapture& cam,int framecount)
{
	cam.set(CV_CAP_PROP_POS_FRAMES,framecount);
	cam>>frame;
	imshow("pic1",frame);
	waitKey(0);
	frame.copyTo(img);
	
	//像素滤波，通过统计的数据过滤掉背景
	linesPro.colorFilter1(img,img);
	Mat edges;
	Canny(img,edges,50,100,3);
	//膨胀边缘，加到原图上时线更明显
	linesPro.lineDilate(edges,edges,0);
	cvtColor(edges,edges,CV_GRAY2BGR);
	//锐化：原图像+边缘----还试过直方图均衡化和反向投影，效果不好
	img+=edges;
	//像素滤波，把色度较低的点去掉
	linesPro.colorFilter2(img,img);
	return;
}
void standardImgPro(Mat &input)
{

	GaussianBlur(input,input,Size(3,3),0);
	Mat edges;
	Canny(input,edges,50,100,3);
	int dilation_size=1;
	Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
									   Point( dilation_size, dilation_size ) );
	int iter=1;
	dilate(edges,edges,element,Point(-1,-1),iter);
	int erode_size=1;
	Mat element1 = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2* erode_size + 1, 2* erode_size+1 ),
    
									   Point( erode_size, erode_size ) );


	erode(edges,edges,element1,Point(-1,-1),iter);
	vector<Vec4i> lines;
	HoughLinesP(edges,lines,1,CV_PI/180,50,300,5);
	
	linesPro.drawLines(lines,input);
	imshow("pic1",input);
	waitKey(0);
}



