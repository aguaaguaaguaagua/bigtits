#include "stdafx.h"
#include "MeanshiftSegmentProcessingForMyPic.h"

msImageProcessor mss;
extern Mat frame;

void DoMeanShiftSegmentationBasedProcessing(
	const vector<UINT>&						inputImg,
	const int&								width,
	const int&								height,
	const string&							filename,
	const vector<double>&					salmap,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<UINT>&							segimg,
	vector<UINT>&							segobj);
void DoMeanShiftSegmentation(
	const vector<UINT>&						inputImg,
	const int&								width,
	const int&								height,
	vector<UINT>&							segimg,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<int>&							labels,
	int&									numlabels);
//////////////////////////////////////////////////////////////////////////////////
///////////-----------我是分割线-------------////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void msProcessing::GetPicBuffer(Mat &in)
{
	//check input
	int cn=in.channels();
	if(in.channels()==3)
		m_imgType=COLOR;
	else if(in.channels()==1)
		m_imgType=GRAYSCALE;
	else {cout<<"请输入正确格式的图像"<<endl;return;}
	
	int width=in.cols,height=in.rows;
	int size=width*height;

	uchar *p;
	for(int i=0;i<height;i++)
	{
		p=in.ptr<uchar>(i);
		for(int j=0;j<width*cn;j++)
			m_imgBuffer.push_back(p[j]);
	}
	if(m_imgBuffer.size()!=size*(in.channels()))
	{cout<<"图像读入错误"<<endl;return;}
}

void msProcessing::SendPicBuffer(Mat &out)
{
	uchar *p;
	int cn=out.channels();
	int width=out.cols,height=out.rows;
	
		for(int i;i<height;i++)
			{
				p=out.ptr<uchar>(i);
					for(int j=0;j<width*cn;j++)
					{
						p[j]=m_imgBuffer[i*width+j];
					}
			}
}
void msProcessing::SendPicBuffer(Mat &out,BYTE * buff)
{
	uchar *p;
	int width=out.cols,height=out.rows;
	int cn=out.channels();
		for(int i=0;i<height;i++)
			{
				p=out.ptr<uchar>(i);
					for(int j=0;j<width*cn;j++)
					{
						p[j]=buff[i*width*cn+j];
					}
			}
}
void msProcessing::SendPicBuffer(Mat &out,int * lab)
{
	//it is for the singel channel testing image
	
	uchar *p;
	int width=out.cols,height=out.rows;
	int cn=out.channels();
		for(int i=0;i<height;i++)
			{
				p=out.ptr<uchar>(i);
					for(int j=0;j<width*cn;j++)
					{
						p[j]=(uchar)lab[i*width*cn+j*cn];
					}
			}
}

void msProcessing::DoMeanShiftSegmentationBasedProcessing(
	const int&								width,
	const int&								height,
	const string&							filename,
	const vector<double>&					salmap,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<UINT>&							segimg,
	vector<UINT>&							segobj)
{
	
	int sz = width*height;
	
	vector<int> labels(0);
	int numlabels(0);
	DoMeanShiftSegmentation(width, height, segimg, sigmaS, sigmaR, minRegion, labels, numlabels);
	
	segobj.resize(sz, 0);
	
}
//自定义的
bool greater_user(const countsList & m1, const countsList & m2) {
        return m1.counts > m2.counts;
}

/////last entry
void msProcessing::DoMeanShiftSegmentation(
	
	const int&								width,
	const int&								height,
	vector<UINT>&							segimg,
	const int&								sigmaS,
	const float&							sigmaR,
	const int&								minRegion,
	vector<int>&							labels,
	int&									numlabels)
{
	int sz = width*height;

	//put the data into the pointer
	bytebuff=new BYTE[sz*3];
	for(size_t i=0;i<sz*3;i++) bytebuff[i]=m_imgBuffer[i];

	
	mss.DefineImage(bytebuff, m_imgType, height, width);		
	mss.Segment(sigmaS, sigmaR, minRegion, HIGH_SPEEDUP);
	mss.GetResults(bytebuff);
	
	//get the points count
	vector<countsList> countsList(0);
	mss.GetModeCounts(countsList);
	//stl sort
	sort(countsList.begin(),countsList.end(),greater_user);
	

	int* p_labels = new int[sz];
	numlabels = mss.GetLabels(p_labels);
	labels.resize(sz);
	for( int n = 0; n < sz; n++ ) labels[n] = p_labels[n];
	//a simple test
	Mat oFrame(frame.size(),CV_8UC3);
	SendPicBuffer(oFrame,bytebuff);
	//Mat testFrame(frame.size(),CV_8UC1);
	//SendPicBuffer(testFrame,p_labels);
	//imshow("pic1",testFrame);
	//waitKey(0);
	//////filter bg
	/*
	Mat gray;
	cvtColor(oFrame,gray,CV_BGR2GRAY);
	Mat edges;
	Canny(gray,edges,50,100,3);
	vector<Vec4i> lines;
	HoughLinesP(edges,lines,1,CV_PI/180,20,80,3);
	vector<Point> tempPt;
	for(int i=0;i<lines.size();i++)
	{
		tempPt.push_back(Point(lines[i][0],lines[i][1]));
		tempPt.push_back(Point(lines[i][2],lines[i][3]));
	}
	for(int i=0;i<tempPt.size();i++)
	{
		int x=tempPt[i].x;
		int y=tempPt[i].y;
		int locLabel=p_labels[y*width+x];
		GetThePointMask(locLabel,p_labels);//by labels
		uchar *colorToBeFiltered=new uchar[3];
		GetTheColorAtSpecificLabel(oFrame,3,colorToBeFiltered,p_labels);
		filterWithMask(oFrame,pointsSelected,colorToBeFiltered);
	}
	//cvtColor(oFrame,oFrame,CV_BGR2GRAY);
	//Mat edges;
	//Canny(oFrame,edges,50,100,3);
	//vector<Vec4i> lines;
	//HoughLinesP(edges,lines,1,CV_PI/180,30,100,3);
	*/
	imshow("pic1",oFrame);
	waitKey(0);

	///////////////////////////////////////
	if(p_labels) delete [] p_labels;
	
	imshow("pic1",oFrame);
	waitKey(0);
	if(bytebuff) delete [] bytebuff;
	return;
}

void msProcessing::mainProcess()
{
	GetPicBuffer(input);
	vector<UINT> segimg, segobj;
	vector<int> labels(0);
	int numLabels=0;
	DoMeanShiftSegmentation(m_width,m_height,segimg,m_sigmaS,m_sigmaR,minRegion,labels,numLabels);
	return;
}

void msProcessing::GetThePointMask(int label,int *labels)
{
	
	int cn=1;
	if(m_imgType==COLOR)
		cn=3;
	//the largest
	
	
	int pointsCount=0;
	for(int i=0;i<m_height;i++)
	{
		for(int j=0;j<m_width;j++)
		{
			if(labels[i*m_width+j]==label)
			{
				pointsSelected.push_back(Point(j,i));
				pointsCount++;
			}
		}
	}
	return;
}
void msProcessing::filterWithMask(Mat &in,vector<Point> &pts,uchar *color)
{
	uchar *p=NULL;
	int x,y,cn=in.channels();
	for(int i=0;i<pts.size();i++)
	{
		x=pts[i].x;
		y=pts[i].y;
		p=(in.ptr<uchar>(y))+x*cn;
		p[0]=color[0],p[1]=color[1],p[2]=color[2];
	}
	return;
}
void msProcessing::GetTheColorAtSpecificLabel(Mat &in,int label,uchar *color,int *labels)
{
	int index=0,cn=in.channels();
	while(labels[index]!=label)
	{
		index++;
	}
	int x=index%(in.cols);
	int y=index/(in.cols);
	uchar *p=NULL;
	p=(in.ptr<uchar>(y))+x*cn;
	for(int i=0;i<3;i++)
		color[i]=p[i];
}
