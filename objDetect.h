
#ifndef _OBJDETECT_H
#define _OBLDETECT_H
#include "global.h"
extern Mat src,frame;  

extern VideoWriter sd;


#define PAINT_ALL_VECTOR
//#define PAINT_GOOD_VECTOR
#define PAINT_BAD_VECTOR



#define LOW_B 10
#define UNASSIGNED 905
#define THRESH_ANG CV_PI/4
const Mat offsetForMask;


enum detectorType {COLORED,SHAPED,BROKEN,SPREAD};
/////////////////////////////////////////////////////////////////////////////
//base detector
namespace fucker
{


class detector
{
public:
	
	
	//explicitly refuse default constructor!!!
	detector(int _step,Mat _input,int _maskSize):
	step(_step),
	input(_input),
	maskSize(_maskSize),
	mask(Mat()),
	Dots() 
	{

	};

	virtual ~detector(){};
	//沿线寻找特殊像素
	virtual void alongTheLine(const vector<Vec4i>&) = 0;//abstract base class
	//直方图计算
	void Hist( Mat &);
	//画出检测出来的多边形
	void drawContour();
	
protected:
	//input buffer
	Mat input;
	//template mask
	Mat mask;
	//异常点
	vector<Point> Dots;
	//检测时的步长
	int step;
	int maskSize;
	int imageType;

	//for some extra usage
	Mat tem;
};


//colored detector
class alongTheLineDetector: public detector
{
public:
	
	alongTheLineDetector(int _step,Mat _input,int _maskSize,int _kernelSize):
	detector(_step,  _input, _maskSize)
	{

	};
		
	~alongTheLineDetector()
	{
	};
	void alongTheLine(const vector<Vec4i>& );


};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////gradient detection///////////////////////////////////
////this struct stores data of infomation on gradient at each point///////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct Statis
{
public:
	Statis():unusualNum(0),totalNum(0),unusualPercent(0),
			 Diff(0),indicator(0),if_unusual(false){};
	int unusualNum;// by the angel
	int totalNum;
	double unusualPercent;
	//thresh difference
	vector<double> Diff;
	vector<bool> indicator;
	//
	bool if_unusual;
};

struct gradInfo
{
	vector<double> gradRawMag;
	vector<double> gradRawAng;
	vector<Vec4i> location;
	Point anchor;
	double lineAng;
	int maskSize;
	
	
	Statis statis; 
};



class alongTheLineDetector_1: public detector
{
public:
	
	alongTheLineDetector_1(int _step,Mat _input,int _maskSize, int _kernelSize);
	
	~alongTheLineDetector_1()
	{

	};
	void getGradOfROI(const vector<Vec4i>&);
	void alongTheLine(const vector<Vec4i>&);
	void calcGrad(const Point&);
	void findSpecPoints(vector<gradInfo>&);
	void drawGrad();
	//statistically analyze the data
	void calcPercent(Statis &);
	void getKofLine(const Point&,const Point&);
	void getGoodPosition();
private:
	//stores a set of grads in one mask

	vector<gradInfo> pointGradInfo;

	//special points

	vector<gradInfo> specPoint;
	//
	
	
	double lineSlope;
	//this matrix is for coordinate transformation

	Mat coorOffset;

	//for loop calculation

	MatIterator_<uchar> itMask,endMask;
	MatIterator_<float> itMag,endMag;
	MatIterator_<float> itAng,endAng;
	MatIterator_<int>   itCoordinate,endCoordinate;
	//for vector calculation, also can be used in some other domain
	vector<gradInfo>::iterator infoIter;
	vector<double>::iterator MagIter;
	vector<double>::iterator AngIter;
	vector<Point>::iterator locIter;

	int kernelSize;
	//total square
	int maskArea;
	//overall statis
	double avgPercent;
	vector<gradInfo *> unusualPoints;
	//for store infomation
	Mat Mag,Ang;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////this detector is used to cut an ROI into multiple areas and saliency it///////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class alongTheLineDetector_2: public detector
{
public:
	alongTheLineDetector_2(int _step,Mat _input,int _maskSize, int _kernelSize);

};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////whole detector factory////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class detectorFactory
{
public:
	detectorFactory(){};
	virtual ~detectorFactory(){};
	virtual detector* createDetector(int step, Mat input, int MaskSize, int kernelSize)=0;
};

//derived shape detector
class detectorFactoryColored : public detectorFactory
{
public:
	detectorFactoryColored():detectorFactory(){};
	~detectorFactoryColored(){};
	detector *createDetector(int step, Mat input, int MaskSize, int kernelSize)
		//we dont use mask and kernel here so put them negative
	{
		cout<<"colored detector"<<endl;
		return new alongTheLineDetector(step,input,MaskSize,kernelSize);
	}
};

//derived shape detector
class detectorFactoryShaped : public detectorFactory
{
public:
	detectorFactoryShaped():detectorFactory(){};
	~detectorFactoryShaped(){};
	detector *createDetector(int step, Mat input, int MaskSize, int kernelSize)
	{
		//debug
		cout<<"call shaped detector"<<endl;

		return new alongTheLineDetector_1(step,input,MaskSize,kernelSize);
	}
};
//derived sub-blocks detector
class detectorFactoryBlocked: public detectorFactory
{
	public:
	detectorFactoryBlocked():detectorFactory(){};
	~detectorFactoryBlocked(){};
	detector *createDetector(int step, Mat input, int MaskSize, int kernelSize)
	{
	//	return new alongTheLineDetector_2(step,input,MaskSize,kernelSize);
	}
};
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
	}//end of the namespace
void mySwap(int&,int&);
#endif