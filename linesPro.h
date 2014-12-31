/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        class
/////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _LINESPRO_H
#define _LINESPRO_H

#ifdef InfoCollect

 ofstream foutBg("F:/FFOutput/bg1.txt");			   //������Ϣ�ռ�	
 ofstream foutLine("F:/FFOutput/line1.txt");         //������Ϣ
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
		//���ߺ���
		static void drawLines(vector<Vec4i>& lines,Mat& picBoutToDrawshow,Scalar &color=Scalar(0,0,255))                   
		{
			for(size_t i=0;i<lines.size();i++)
			{
				line(picBoutToDrawshow,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),color,2);
			}
		}
		//��ɫ�����˲�,ͨ��ͳ�Ƴ���������
		void colorFilter1(const Mat&,Mat &);
		void colorFilter2(const Mat&,Mat &);
		//�ú��ʵ���ֵ����canny��Ե���
		void edgeDetection(const Mat& src,Mat &edges);
		//�ø��ʻ���任�������ֱ��
		void lineDetection(Mat&);
		//��ֵ���㣬��û�õ�
		double threshCalc(vector<Vec3b>);
		//��������Ӵ�����
		void lineDilate(Mat &src,Mat &dst,int dilationElem);
		//��ʴ����
		void lineErode(Mat &src,Mat &dst,int ErodeElem);
		//���ƽ���ߣ��ﵽһ���������ǵ���
		void parallelEx(vector<Vec4i>&);
		
		
		//����ͼ���Ա
		OutputPic showPic;		
		//�ռ���ʱ����
		Vec3b collect_temp;	
		//����ͳ�Ƶĵ�����ɫ����
		vector<Vec3b> linesColor;	
		//����ͳ�Ƶ��޹ر�����ɫ����
		vector<Vec3b> bgColor;			
	private:
					
		//���ƽ����
		vector<Vec4i> paraLines;

	protected:
		
	};

lineProcess linesPro;	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function headers
///////////////////////////////////////////////////////////////////////////////////////////////////////
 //ֱ��ͼ���⻯������ͨ����ϲ����
void equalizeHistBGR(Mat&, Mat&);   
//ֱ��ͼ���⻯������ͨ���������ͨ������
void equalizeHistSplit(const Mat&,Mat&,Mat&,Mat&);
//��껬��ʱָʾ������Ϣ����Ϊ�ռ���Ϣ��
void PixelInfo(Mat&,int flags,char*);
void on_mouse(int, int, int, int, void*);
//void drawHist( Mat& B,Mat& G,Mat& R,string winname);
bool readFile(const string& filename);
//����ǶȺ�ֱ�߾���
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
		
		 //�������
		Point now(x,y);	      
		//��ȡĿǰ���ص���ɫ��������ͨ��8bit
		 int i=frame.at<Vec3b>(y,x)[0];
		 int j=frame.at<Vec3b>(y,x)[1];
		 int k=frame.at<Vec3b>(y,x)[2];
		 //��������ʱ��ѵ�ǰ��ɫ��Ϣ���浽ȫ�������ʱ����
		linesPro.collect_temp=frame.at<Vec3b>(y,x);   
		stringstream temp;		                          
		string text;
		temp<<"x:"<<x<<",y:"<<y<<"coordinates";
		temp>>text;
		putText(_temp,text,now,CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
		temp.clear();
		imshow("pic1",_temp);
		
	}
	//˫���Ҽ����浱ǰ��ɫ��Ϣ�������ռ���,Ҫ�ȵ��������
	else if(_event==CV_EVENT_RBUTTONDBLCLK)        
	{
		
		putText(_temp,"bgcolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutBg<<linesPro.collect_temp<<endl;
#endif
		imshow("pic1",_temp);
	}
	 //˫��������浱ǰ��ɫ��Ϣ��������ռ���
	else if(_event==CV_EVENT_LBUTTONDBLCLK)       
	{
		
		putText(_temp,"linescolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutLine<<linesPro.collect_temp<<endl;
#endif
		imshow("pic1",_temp);
	}
}

//�������ļ����ݵ�,Ŀǰû����
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
	//Canny��Ե�����ֵ��
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
			//��ɫ�˲����˳�һЩ����
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

	//����ƽ�����Թ���ʱ��������ע�͵�
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
	//ֱ�߷��������
	if(lines.size()==0)
		return;
	vector<Vec4i> dir;
	int flag=0,j=0,type=0;
	//����������������ƽ�е��������ִ������룬������������ʾĳ��ƽ���߳��ֵĴ��������ĸ�Ϊ��ͬ���������
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
				//��flag��1����ʾΪͬ����ƽ���߳���,�������ǵķ�������type������
				flag++;
				dir[i][3]=dir[j][3];
				break;
			}
		}
		//���Ϊ�·����ƽ���ߣ�������ִ���������1������������������1
		if(flag==0){dir[i][2]++;type++;dir[i][3]=type;}
		
		//��flag�����Խ����µ�ͳ��
		flag=0;

	}
	
	//ͨ�������������ò�ͬ�Ƕ�ƽ���ߵ�����
	//ͨ���Ƚ��˳�����
	Mat result=Mat::zeros(Size(1,100),CV_8U);
	for(int i=0;i<lines.size();i++)
		result.at<uchar>(i,0)=(uchar)dir[i][2];
	//���ۼƵ�ƽ��������ѡ�����ָ�������2��
	cv::sort(result,result,CV_SORT_EVERY_COLUMN|CV_SORT_DESCENDING );
	
	
	fucker::alongTheLineDetector *myDetector1=new fucker::alongTheLineDetector(3,frame,0,0);

	Mat tem;
	frame.copyTo(tem);
		for(int i=0;i<lines.size();i++)
		{
			
			//��Ϊ����Ƶ������3��ʱ
			if(dir[i][2]==result.at<uchar>(0,0)||dir[i][2]==result.at<uchar>(1,0)||dir[i][2]==result.at<uchar>(2,0))
			{
				//vector<Vec4i> tempPoint;
				//��������ͬ��ֱ��ȫ������
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
		
//���߼��- -
	

		
	
	
	//frame.copyTo(showPic);
	//drawLines(paraLines);
	
}
//����Ƿ�ƽ�У���׼����������
int angle(Vec2i a,Vec2i b)
{
	int i=a.mul(a)[0]+a.mul(a)[1],j=b.mul(b)[0]+b.mul(b)[1];
	double result=(a.dot(b))/(sqrt(double(i))*sqrt(double(j)));
	return result>0.986? 1 : 0;
}
//������
int Distance(const Vec4i& a,const Vec4i& b)
{
	int distThresh=60,a1=a[0],a2=a[1],a3=a[2],a4=a[3],b1=b[0],b2=b[1];
	double k=double((a2-a4))/(a1-a3);
	double A=k,C=-k*a[0]+a[1],B=-1;
	double dist=abs(A*b1+B*b2+C)/sqrt(pow(A,2)+pow(B,2));
	return dist<distThresh? 1:0;
}
#endif