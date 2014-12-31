/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        class
/////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _LINESPRO_H
#define _LINESPRO_H

#ifdef InfoCollect

 ofstream foutBg("F:/FFOutput/bg1.txt");			   //背景信息收集	
 ofstream foutLine("F:/FFOutput/line1.txt");         //电线信息
#endif

#include "ROI.h"

class lineProcess
{
public:
	
		//constructors
		lineProcess():showPic(Mat()),collect_temp(),linesColor(),bgColor(),
					  paraLines()
		{};
		                       
		
		//functions
		//画线函数
		static void drawLines(vector<Vec4i>& lines,Mat& picBoutToDrawshow,Scalar &color=Scalar(0,0,255))                   
		{
			for(size_t i=0;i<lines.size();i++)
			{
				line(picBoutToDrawshow,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),color,2);
			}
		}
		//在色度上滤波,通过统计出来的数据
		void colorFilter1(const Mat&,Mat &);
		void colorFilter2(const Mat&,Mat &);
		//用合适的阈值进行canny边缘检测
		void edgeDetection(const Mat& src,Mat &edges);
		//用概率霍夫变换函数检测直线
		void lineDetection(Mat&);
		//阈值计算，还没用到
		double threshCalc(vector<Vec3b>);
		//膨胀运算加粗线条
		void lineDilate(Mat &src,Mat &dst,int dilationElem);
		//腐蚀运算
		void lineErode(Mat &src,Mat &dst,int ErodeElem);
		//检测平行线，达到一定数量则是电线
		void parallelEx(vector<Vec4i>&);
		
		
		//输入图像成员
		OutputPic showPic;		
		//收集临时分量
		Vec3b collect_temp;	
		//经过统计的电线颜色集合
		vector<Vec3b> linesColor;	
		//经过统计的无关背景颜色集合
		vector<Vec3b> bgColor;			
	private:
					
		//存放平行线
		vector<Vec4i> paraLines;

	protected:
		
	};

lineProcess linesPro;	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function headers
///////////////////////////////////////////////////////////////////////////////////////////////////////
 //直方图均衡化，若三通道则合并输出
void equalizeHistBGR(Mat&, Mat&);   
//直方图均衡化，若三通道则分三个通道输入
void equalizeHistSplit(const Mat&,Mat&,Mat&,Mat&);
//鼠标滑过时指示像素信息，作为收集信息用
void PixelInfo(Mat&,int flags,char*);
void on_mouse(int, int, int, int, void*);
//void drawHist( Mat& B,Mat& G,Mat& R,string winname);
bool readFile(const string& filename);
//计算角度和直线距离
int angle(Vec2i a,Vec2i b);
int Distance(const Vec4i& a,const Vec4i& b);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function define
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void equalizeHistBGR( Mat& src, Mat& dst)
{
	if(src.channels()==3)
	{
		vector<Mat> mv;
		split(src,mv);
		int i=0;
		while(i<3)
		{equalizeHist(mv[i],mv[i]);i++;}
		merge(mv,dst);
	}
	else
		equalizeHist(src,dst);
}

void equalizeHistSplit(const Mat& src,Mat& d1,Mat& d2,Mat& d3)
{

	assert(src.channels()==3);
	vector<Mat> mv;
	split(src,mv);
	int i=0;
	while(i<3)
	{equalizeHist(mv[i],mv[i]);i++;}
	d1=mv[0];d2=mv[1];d3=mv[2];
	
}

void PixelInfo(Mat& source,char *winname)
{
	
	
	cvSetMouseCallback(winname,on_mouse);
}

void on_mouse(int _event,int x,int y,int flags, void*)
{
	Mat _temp;
	frame.copyTo(_temp);
	cvtColor(_temp,_temp,CV_BGR2HSV);
	double scale=1;
	if(_event==CV_EVENT_LBUTTONDOWN)
	{
		
		 //鼠标坐标
		Point now(x,y);	      
		//获取目前像素的颜色向量，三通道8bit
		 int i=frame.at<Vec3b>(y,x)[0];
		 int j=frame.at<Vec3b>(y,x)[1];
		 int k=frame.at<Vec3b>(y,x)[2];
		 //点击左键的时候把当前颜色信息保存到全局类的临时变量
		linesPro.collect_temp=frame.at<Vec3b>(y,x);   
		stringstream temp;		                          
		string text;
		temp<<"x:"<<x<<",y:"<<y<<"coordinates";
		temp>>text;
		putText(_temp,text,now,CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
		temp.clear();
		imshow("pic1",_temp);
		
	}
	//双击右键保存当前颜色信息到背景收集表,要先点左键才行
	else if(_event==CV_EVENT_RBUTTONDBLCLK)        
	{
		
		putText(_temp,"bgcolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutBg<<linesPro.collect_temp<<endl;
#endif
		imshow("pic1",_temp);
	}
	 //双击左键保存当前颜色信息到输电线收集表
	else if(_event==CV_EVENT_LBUTTONDBLCLK)       
	{
		
		putText(_temp,"linescolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutLine<<linesPro.collect_temp<<endl;
#endif
		imshow("pic1",_temp);
	}
}

//用来读文件数据的,目前没用了
bool readFile(const string& filename)               
{
	ifstream fin(filename);
	if(!fin){cerr<<"file open fail"<<endl;return false;}
	string str;
	
	char temp;
	int i;
	while(getline(fin,str))
	{
		int j=0;
		string tem;
		int r,g,b;
		for(i=0;i<str.length();i++)
		{

			temp=*(str.begin()+i);
			switch(temp)
			{
			case '[':
			case ' ':continue;
			case ',':{
				if(j==0)
					 { 
						 b=atoi(tem.c_str());
						j++;
					 }
					 else if(j==1)
						 g=atoi(tem.c_str());
						tem.clear();
					 }
			case ']':{r=atoi(tem.c_str());tem.clear();break;}
			default:{
				tem.push_back(temp);}
			}
		}
		
		Vec3b vec(b,g,r);
		linesPro.linesColor.push_back(vec);


	}
	
	return 1;
}

void lineProcess::edgeDetection(const Mat& src,Mat &edges)
{
	Mat temp;
	blur(src,temp,Size(3,3));
	//Canny边缘检测阈值，
	int lowThresh=155,highThresh=253;
	Canny(temp,edges,lowThresh,highThresh,3);

}

//---------------class functions---------------------//
void lineProcess::colorFilter1(const Mat& frame,Mat &temp)
{
	
	frame.copyTo(temp);
	uchar *p;
	int cn=temp.channels();
	for(int i=0;i<temp.rows;i++)
	{
		p=temp.ptr<uchar>(i);
		for(int j=0;j<temp.cols;j++)
			//颜色滤波，滤除一些背景
		{
			if(j>(temp.cols>>1))
			{
			if((p[cn*j]<100)||(p[cn*j+1]<155)||(p[cn*j+2]<155))
			{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
			else if((p[cn*j]>250)&&(p[cn*j+1]>250)&&(p[cn*j+2]>250))
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
			else if((p[cn*j]>p[cn*j+1])||(p[cn*j]+1>p[cn*j+2]))
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
			}
			else
			{
			if((p[cn*j]<130)||(p[cn*j+1]<170)||(p[cn*j+2]<170))
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
			else if((p[cn*j]>250)&&(p[cn*j+1]>250)&&(p[cn*j+2]>250))
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
			else if((p[cn*j]>p[cn*j+1])||(p[cn*j]+1>p[cn*j+2]))
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}

			}
		}
	}
}
void lineProcess::colorFilter2(const Mat& frame,Mat &temp)
{
	frame.copyTo(temp);
	int cn=temp.channels();
	uchar *p;
	for(int i=0;i<temp.rows;i++)
	{
		p=temp.ptr<uchar>(i);
		for(int j=0;j<temp.cols;j++)
			
			{
				if(p[j*cn]<230)
				{p[cn*j]=0,p[cn*j+1]=0,p[cn*j+2]=0;}
				
			}
	}	
}
void lineProcess::lineDilate(Mat &src,Mat &dst,int dilationElem)
{

	int type;
	if( dilationElem == 0 ){ type = MORPH_RECT; }
	else if( dilationElem == 1 ){ type = MORPH_CROSS; }
	else if( dilationElem == 2) { type = MORPH_ELLIPSE; }
	

	int dilation_size=1;
	Mat element = getStructuringElement( type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
    
									   Point( dilation_size, dilation_size ) );

	
	
	int iter=1;
	dilate(src,dst,element,Point(-1,-1),iter);
}

void lineProcess::lineErode(Mat &src,Mat &dst,int ErodeElem)
{

	int type;
	if( ErodeElem == 0 ){ type = MORPH_RECT; }
	else if( ErodeElem == 1 ){ type = MORPH_CROSS; }
	else if( ErodeElem == 2) { type = MORPH_ELLIPSE; }
	

	int erode_size=1;
	Mat element = getStructuringElement( type,
                                       Size( 2* erode_size + 1, 2* erode_size+1 ),
    
									   Point( erode_size, erode_size ) );

	
	
	int iter=1;
	erode(src,dst,element,Point(-1,-1),iter);
}
void lineProcess::lineDetection(Mat& src)
{
	vector<Vec4i> linesTemp;
	int threshVote=450;
	int minLength=frame.cols/6;
	int maxGap=10;
	double rho=1,theta=1*CV_PI/180;
	if(src.channels()==3)
	cvtColor(src,src,CV_BGR2GRAY);
	HoughLinesP(src,linesTemp,rho,theta,threshVote,minLength,maxGap);

	//采用平行特性过滤时，将下列注释掉
	/*
	frame.copyTo(showPic);
	drawLines(linesTemp);
	imshow("shit",showPic);
	waitKey(0);
	for(int i=0;i<linesTemp.size();i++)
	{
		myLineEx.aloneTheLine(Point(linesTemp[i][0],linesTemp[i][1]),Point(linesTemp[i][2],linesTemp[i][3]));
		
	}
	imshow("shit",frame);
	waitKey(0);
	*/
	parallelEx(linesTemp);
	
}

void lineProcess::parallelEx(vector<Vec4i>& lines)
{
	//直线方向的向量
	if(lines.size()==0)
		return;
	vector<Vec4i> dir;
	int flag=0,j=0,type=0;
	//计算向量，并近似平行的向量出现次数存入，第三个变量表示某类平行线出现的次数，第四个为不同方向的种类
	for(int i=0;i<lines.size();i++)
	{
		
		dir.push_back(Vec4i(lines[i][2]-lines[i][0],lines[i][3]-lines[i][1],0,type));
		
		if(i==0)
		{dir[i][2]++;continue;}
		for(j=0;j<i;j++)
		{
			int cos=angle(Vec2i(dir[j][0],dir[j][1]),Vec2i(dir[i][0],dir[i][1]));
			int dis=Distance(lines[i],lines[j]);
			if(cos&&dis)
			{
				dir[j][2]++;
				//当flag加1，表示为同方向平行线出现,并将它们的方向索引type变成相等
				flag++;
				dir[i][3]=dir[j][3];
				break;
			}
		}
		//如果为新方向的平行线，则将其出现次数单独加1，并且其种类索引加1
		if(flag==0){dir[i][2]++;type++;dir[i][3]=type;}
		
		//将flag置零以进行新的统计
		flag=0;

	}
	
	//通过上述计算可求得不同角度平行线的数量
	//通过比较滤除杂线
	Mat result=Mat::zeros(Size(1,100),CV_8U);
	for(int i=0;i<lines.size();i++)
		result.at<uchar>(i,0)=(uchar)dir[i][2];
	//对累计的平行线排序，选出出现概率最大的2个
	cv::sort(result,result,CV_SORT_EVERY_COLUMN|CV_SORT_DESCENDING );
	
	
	fucker::alongTheLineDetector *myDetector1=new fucker::alongTheLineDetector(3,frame,0,0);

	Mat tem;
	frame.copyTo(tem);
		for(int i=0;i<lines.size();i++)
		{
			
			//当为出现频数最大的3个时
			if(dir[i][2]==result.at<uchar>(0,0)||dir[i][2]==result.at<uchar>(1,0)||dir[i][2]==result.at<uchar>(2,0))
			{
				//vector<Vec4i> tempPoint;
				//把种类相同的直线全部存入
				for(int k=0;k<lines.size();k++)
				{
					if(dir[i][3]==dir[k][3])
					{
						paraLines.push_back(lines[k]);
						//tempPoint.push_back(lines[k]);
					}
	
				}
				//lineContour(tempPoint);
				
			//myExtractor.lineContour(paraLines);
			//myExtractor.getGOI();
			//imshow("pic1",frame);
			//waitKey(0);
			//myExtractor.histInROI();
			//--------------------------------------
			
			drawLines(paraLines,tem);
			imshow("pic1",tem);
			sd<<tem;
			waitKey(1);
//-------------------------------------------------------
			//sd<<tem;
			myDetector1->alongTheLine(paraLines);
			myDetector1->drawContour();
			paraLines.clear();
			}
			
		
		}
		
//沿线检测- -
	

		
	
	
	//frame.copyTo(showPic);
	//drawLines(paraLines);
	
}
//检测是否平行，标准是向量共线
int angle(Vec2i a,Vec2i b)
{
	int i=a.mul(a)[0]+a.mul(a)[1],j=b.mul(b)[0]+b.mul(b)[1];
	double result=(a.dot(b))/(sqrt(double(i))*sqrt(double(j)));
	return result>0.986? 1 : 0;
}
//检测距离
int Distance(const Vec4i& a,const Vec4i& b)
{
	int distThresh=60,a1=a[0],a2=a[1],a3=a[2],a4=a[3],b1=b[0],b2=b[1];
	double k=double((a2-a4))/(a1-a3);
	double A=k,C=-k*a[0]+a[1],B=-1;
	double dist=abs(A*b1+B*b2+C)/sqrt(pow(A,2)+pow(B,2));
	return dist<distThresh? 1:0;
}
#endif