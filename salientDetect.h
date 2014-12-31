#ifndef __SALIENT_DETECT__
#define __SALIENT_DETECT__



class salientObjectBase
{
public:
	salientObjectBase(int _stairs):stairs(_stairs),histOfBGR(),saliencyMap(),if_ok(true){};
	~salientObjectBase(){};
	//12
	void RGBhist(const Mat&,int);
	void showTheSaliency();
	void _24bitSingelChannelToLabPerPixel(uchar *sR,uchar *sB,uchar *sG,float *sL,float *sA,float *sb);
	void BGR2Lab(Mat &,Mat &);
	virtual void saliency24bRegion(Mat &)=0;
protected:
	int stairs;
	MatND histOfBGR;

	//Map having been normalized on dist
	Mat saliencyMap;
	bool if_ok;//if the format is ok?Why i put this variable here
};
//////////////////////////////////////////////////////////////////////////////////////////////////
//functions in base class
class salientObjectBasedOnFT: public salientObjectBase
{
public:
	salientObjectBasedOnFT():salientObjectBase(0),meanL(0),meanA(0),meanB(0){};
	~salientObjectBasedOnFT();
	void  saliency24bRegion(Mat &);
private:
	float meanL,meanA,meanB;
	
};
///////////////////////////////////////////////////////////////////////////////////////////////
void salientObjectBase::_24bitSingelChannelToLabPerPixel(uchar *sB,uchar *sG,uchar *sR,float *sL,float *sA,float *sb)
{

		double R = *sR/255.0;
		double G = *sG/255.0;
		double B = *sB/255.0;

		double r, g, b;

		if(R <= 0.04045)	r = R/12.92;
		else				r = pow((R+0.055)/1.055,2.4);
		if(G <= 0.04045)	g = G/12.92;
		else				g = pow((G+0.055)/1.055,2.4);
		if(B <= 0.04045)	b = B/12.92;
		else				b = pow((B+0.055)/1.055,2.4);

		double X = r*0.4124564 + g*0.3575761 + b*0.1804375;
		double Y = r*0.2126729 + g*0.7151522 + b*0.0721750;
		double Z = r*0.0193339 + g*0.1191920 + b*0.9503041;
		//------------------------
		// XYZ to LAB conversion
		//------------------------
		double epsilon = 0.008856;	//actual CIE standard
		double kappa   = 903.3;		//actual CIE standard

		double Xr = 0.950456;	//reference white
		double Yr = 1.0;		//reference white
		double Zr = 1.088754;	//reference white

		double xr = X/Xr;
		double yr = Y/Yr;
		double zr = Z/Zr;

		double fx, fy, fz;
		if(xr > epsilon)	fx = pow(xr, 1.0/3.0);
		else				fx = (kappa*xr + 16.0)/116.0;
		if(yr > epsilon)	fy = pow(yr, 1.0/3.0);
		else				fy = (kappa*yr + 16.0)/116.0;
		if(zr > epsilon)	fz = pow(zr, 1.0/3.0);
		else				fz = (kappa*zr + 16.0)/116.0;

		*sL = 116.0*fy-16.0;
		*sA = 500.0*(fx-fy);
		*sb = 200.0*(fy-fz);
		float t1=*sL,t2=*sA;
}

void salientObjectBase::BGR2Lab(Mat &src,Mat &dst)
{
	
	if(dst.channels()!=3||dst.depth()!=CV_32F||if_ok==0)
	{cout<<"dst datatype not matches the Lab"<<endl;if_ok=0;return;}
	uchar *p=NULL;
	float *pdst=NULL;
	for(int i=0;i<src.rows;i++)
	{
		p=src.ptr<uchar>(i);
		pdst=dst.ptr<float>(i);
		for(int j=0;j<src.cols;j++)
		{
			
			_24bitSingelChannelToLabPerPixel(p,p+1,p+2,pdst,pdst+1,pdst+2);
			float t1=*pdst,t2=*(pdst+1);
			p+=3;
			pdst+=3;
		}
	}


}
void salientObjectBase::RGBhist(const Mat &src, int width)
{
	int rbins=stairs,gbins=stairs,bbins=stairs;
	int histSize[]={rbins,gbins,bbins};
	float Ranges[2]={0,256};
	const float* ranges[3] = { Ranges, Ranges,Ranges };
	int channels[]={0,1,2};
	Mat histAfterQuantedToStairs;
	calcHist(&src,1,channels,Mat(),histAfterQuantedToStairs,3,histSize,ranges,false,false);
	histAfterQuantedToStairs.copyTo(histOfBGR);
	
}

void salientObjectBase::showTheSaliency()
{
	if(if_ok==0)
	{cout<<"saliency fails being gotten"<<endl;return;}

	imshow("pic1",saliencyMap);
	waitKey(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//functions in FT class

void salientObjectBasedOnFT::saliency24bRegion(Mat &src)
{
	if(src.channels()!=3||if_ok==0)
	{cout<<"输入图像格式不正确,检查是否为3通道RGB图像"<<endl;
	 if_ok=0;
	 return;
	}
	//distance map, shoulda be float data with singel channel 
	Mat distMap=Mat::zeros(src.size(),CV_32FC1);
	
	//convert to lab
	Mat dst(src.size(),CV_32FC3);
	BGR2Lab(src,dst);

	//mean value in lab
	if(dst.empty()||if_ok==0)
		return;

	unsigned char* p=NULL;
	int sz=src.rows*src.cols;
	for(int i=0;i<dst.rows;i++)
	{
		p=dst.ptr<uchar>(i);
		for(int j=0;j<dst.cols;j++)
		{
			meanL+=*p;meanA+=*(p+1);meanB=*(p+2);
			p+=3;
		}
	}
	meanL=meanL/sz,meanA=meanA/sz,meanB=meanB/sz;
	//gaussian blur

	//compute the map
	float *pMap=NULL;
	float *pLab=NULL;
	for(int i=0;i<dst.rows;i++)
	{
		pLab=dst.ptr<float>(i);
		pMap=distMap.ptr<float>(i);
		for(int j=0;j<dst.cols;j++)
		{
			*(pMap+j)=pow((meanL-pLab[j*3]),2)+pow(meanA-pLab[j*3+1],2)+pow(meanB-pLab[j*3+2],2);
		}
	}
	normalize(distMap,saliencyMap,1,0,CV_MINMAX,-1);
	
}
#endif