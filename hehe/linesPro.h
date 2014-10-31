/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Mat src,frame;  

//д���е��ң��в��õĵط�����˵���ҵ�һ��д����.....


#ifdef InfoCollect

 ofstream foutBg("F:/FFOutput/bg1.txt");			   //������Ϣ�ռ�	
 ofstream foutLine("F:/FFOutput/line1.txt");         //������Ϣ
#endif

class lineProcess
{
public:
	
		//constructors
		lineProcess(){};
		lineProcess(Mat& _templ):templ(_templ){};                       
		lineProcess(vector<Vec4i> _lines,Mat &pic):lines(_lines),showPic(pic){};
		//functions
		//���ߺ���
		void drawLines()                   
		{
			for(size_t i=0;i<lines.size();i++)
			{
				line(showPic,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(255,0,0),2);
			}
		}
		//��������Ӵ�����
		void lineDilate(Mat &src,Mat &dst);				
		//��ɫ�ռ���������� 
		double colorDiff(const Mat& src,Mat& templ);	
		//����ͼ���Ա
		OutputPic showPic;		
		//�ռ���ʱ����
		Vec3b collect_temp;	
		//����ͳ�Ƶĵ�����ɫ����
		vector<Vec3b> linesColor;	
		//����ͳ�Ƶ��޹ر�����ɫ����
		vector<Vec3b> bgColor;				
	private:
			//�ṹ��
		Mat templ;						
		vector<Vec4i> lines;
	protected:
		
	};

lineProcess linesPro;	//ȫ�ֶ���
	
//////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function headers
///////////////////////////////////////////////////////////////////////////////////////////////////////
 //ֱ��ͼ���⻯������ͨ����ϲ����
void equalizeHistBGR(const Mat&, Mat&);   
//ֱ��ͼ���⻯������ͨ���������ͨ������
void equalizeHistSplit(const Mat&,Mat&,Mat&,Mat&);
//��껬��ʱָʾ������Ϣ����Ϊ�ռ���Ϣ��
void PixelInfo(Mat&,int flags,char*);

void on_mouse(int, int, int, int, void*);

//void drawHist( Mat& B,Mat& G,Mat& R,string winname);

bool readFile(const string& filename);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////                        function define
/////////////////////////////////////////////////////////////////////////////////////////////////////////



/*��ֱ��ͼ
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

 /// ��ֱ��ͼ��һ������Χ [ 0, histImage.rows ]
	 normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	 normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	 normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

 /// ��ֱ��ͼ�����ϻ���ֱ��ͼ
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

void equalizeHistBGR(const Mat& src, Mat& dst)
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
		temp<<"B:"<<i<<",G:"<<j<<",R:"<<k<<"Ready";
		temp>>text;
		putText(_temp,text,now,CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
		temp.clear();
		imshow("shit",_temp);
		
	}
	//˫���Ҽ����浱ǰ��ɫ��Ϣ�������ռ���,Ҫ�ȵ��������
	else if(_event==CV_EVENT_RBUTTONDBLCLK)        
	{
		
		putText(_temp,"bgcolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutBg<<linesPro.collect_temp<<endl;
#endif
		imshow("shit",_temp);
	}
	 //˫��������浱ǰ��ɫ��Ϣ��������ռ���
	else if(_event==CV_EVENT_LBUTTONDBLCLK)       
	{
		
		putText(_temp,"linescolor saved",Point(x,y),CV_FONT_HERSHEY_SIMPLEX,scale,Scalar(0,0,255),2);
#ifdef InfoCollect
		foutLine<<linesPro.collect_temp<<endl;
#endif
		imshow("shit",_temp);
	}
}

//�������ļ����ݵģ�������Щ���㹤���������Ը���ʦ�������˵��ѽ�һ��ǰ����������������˼·����
//�ܶ๤���ģ������Ϳ�����������Ϊ���⺢�Ӳ���ô��������Ĺ���= =  �ǲ��Ǻܻ���,�������ᱻѵ��...��
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