/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Mat src,frame;  

//写的有点乱，有不好的地方给俺说，我第一次写程序.....


#ifdef InfoCollect

 ofstream foutBg("F:/FFOutput/bg1.txt");			   //背景信息收集	
 ofstream foutLine("F:/FFOutput/line1.txt");         //电线信息
#endif

class lineProcess
{
public:
	
		//constructors
		lineProcess(){};
		lineProcess(Mat& _templ):templ(_templ){};                       
		lineProcess(vector<Vec4i> _lines,Mat &pic):lines(_lines),showPic(pic){};
		//functions
		//画线函数
		void drawLines()                   
		{
			for(size_t i=0;i<lines.size();i++)
			{
				line(showPic,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(255,0,0),1);
			}
		}
		//在色度上滤波,通过统计出来的数据
		void colorFilter1(const Mat&,Mat &);
		void colorFilter2(const Mat&,Mat &);
		//用合适的阈值进行canny边缘检测
		void edgeDetection(const Mat& src,Mat &edges);
		//用概率霍夫变换函数检测直线
		void lineDetection(Mat&, Mat&);
		//阈值计算，还没用到
		double threshCalc(vector<Vec3b>);
		//膨胀运算加粗线条
		void lineDilate(Mat &src,Mat &dst,int dilationElem);
		//腐蚀运算
		void lineErode(Mat &src,Mat &dst,int ErodeElem);
		//颜色空间分量差别计算 
		double colorDiff(const Mat& src,Mat& templ);	
		//输入图像成员
		OutputPic showPic;		
		//收集临时分量
		Vec3b collect_temp;	
		//经过统计的电线颜色集合
		vector<Vec3b> linesColor;	
		//经过统计的无关背景颜色集合
		vector<Vec3b> bgColor;			
	private:
		
			//结构子
		Mat templ;						
		vector<Vec4i> lines;
		//...先放着，万一有用呢
		vector<Mat> MatColorFiltered;
		//边缘阈值，通过统计数据得到，貌似他妈的有点不好用,默认设个150吧
		double thresh;
	protected:
		
	};

lineProcess linesPro;	//全局对象
	
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



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function define
/////////////////////////////////////////////////////////////////////////////////////////////////////////



/*画直方图
void drawHist( Mat& B,Mat& G,Mat& R,string winname)			
{
	

	const int histSize = 256;
	float Range[]={0,255};
	const float *histRange[]={Range};
	const int channel = 0;  
	Mat b_hist,g_hist,r_hist;
	bool uniform=true,accumulate=false;

	calcHist( &B, 1, &channel, Mat(), b_hist, 1, &histSize, histRange, uniform, accumulate );
	calcHist( &G, 1, &channel, Mat(), g_hist, 1, &histSize, histRange, uniform, accumulate );
	calcHist( &R, 1, &channel, Mat(), r_hist, 1, &histSize, histRange, uniform, accumulate );
	int hist_w = 400; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	Mat histImage( hist_w, hist_h, CV_8UC3, Scalar( 0,0,0) );

 /// 将直方图归一化到范围 [ 0, histImage.rows ]
	 normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	 normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	 normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

 /// 在直方图画布上画出直方图
	for( int i = 1; i < histSize; i++ )
     {
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                      Scalar( 0, 0, 255), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                      Scalar( 0, 255, 0), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                      Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                      Scalar( 255, 0, 0), 2, 8, 0  );
      }

 namedWindow("hist image");
 imshow("hist image",histImage);
}
*/

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
		temp<<"B:"<<i<<",G:"<<j<<",R:"<<k<<"Ready";
		temp>>text;
		putText(_temp,text,now,CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
		temp.clear();
		imshow("shit",_temp);
		
	}
	//双击右键保存当前颜色信息到背景收集表,要先点左键才行
	else if(_event==CV_EVENT_RBUTTONDBLCLK)        
	{
		
		putText(_temp,"bgcolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutBg<<linesPro.collect_temp<<endl;
#endif
		imshow("shit",_temp);
	}
	 //双击左键保存当前颜色信息到输电线收集表
	else if(_event==CV_EVENT_LBUTTONDBLCLK)       
	{
		
		putText(_temp,"linescolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutLine<<linesPro.collect_temp<<endl;
#endif
		imshow("shit",_temp);
	}
}

//用来读文件数据的
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
	
	Canny(temp,edges,155,253,3);

}

//---------------class functions---------------------//
void lineProcess::colorFilter1(const Mat& frame,Mat &temp)
{
	
	frame.copyTo(temp);
	for(int i=0;i<temp.rows;i++)
		for(int j=0;j<temp.cols;j++)
			//颜色滤波，滤除一些背景
		{
			if(j>(temp.cols>>2)+(temp.cols>>1))
			{
			if((temp.at<Vec3b>(i,j)[0]<100)||(temp.at<Vec3b>(i,j)[1]<145)||(temp.at<Vec3b>(i,j)[2]<145))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			else if((temp.at<Vec3b>(i,j)[0]>250)&&(temp.at<Vec3b>(i,j)[1]>250)&&(temp.at<Vec3b>(i,j)[2]>250))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			else if((temp.at<Vec3b>(i,j)[0]>temp.at<Vec3b>(i,j)[1])||(temp.at<Vec3b>(i,j)[0]+1>temp.at<Vec3b>(i,j)[2]))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			}
			else
			{
			if((temp.at<Vec3b>(i,j)[0]<110)||(temp.at<Vec3b>(i,j)[1]<170)||(temp.at<Vec3b>(i,j)[2]<170))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			else if((temp.at<Vec3b>(i,j)[0]>250)&&(temp.at<Vec3b>(i,j)[1]>250)&&(temp.at<Vec3b>(i,j)[2]>250))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			else if((temp.at<Vec3b>(i,j)[0]+1>temp.at<Vec3b>(i,j)[1])||(temp.at<Vec3b>(i,j)[0]+1>temp.at<Vec3b>(i,j)[2]))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);

			}
		}
}
void lineProcess::colorFilter2(const Mat& frame,Mat &temp)
{
	frame.copyTo(temp);
	for(int i=0;i<temp.rows;i++)
		for(int j=0;j<temp.cols;j++)
			
			{if((temp.at<Vec3b>(i,j)[0]<105))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);
			else if((temp.at<Vec3b>(i,j)[0]>240)&&(temp.at<Vec3b>(i,j)[1]>240)&&(temp.at<Vec3b>(i,j)[2]>240))
				temp.at<Vec3b>(i,j)=Vec<uchar,3>(0,0,0);

			
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
void lineProcess::lineDetection(Mat& src, Mat& dst)
{
	vector<Vec4i> linesTemp;
	int threshVote=40;
	int minLength=250;
	int maxGap=10;
	double rho=1,theta=CV_PI/180;
	
	HoughLinesP(src,linesTemp,rho,theta,threshVote,minLength,maxGap);
	lines=linesTemp;
	showPic=dst;
	drawLines();
	imshow("fuck",showPic);
}