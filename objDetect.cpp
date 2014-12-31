#include "stdafx.h"
#include "objDetect.h"
#include "ROI.h"
//#define MEAN_VECTOR
#define SEPERATE_VECTOR



using namespace fucker;




void mySwap(int &i,int &j)
{
	int temp;

	temp=i;
	i=j;
	j=temp;
}
/*
void distantSpreadLineDetector::alongTheLine(vector<Vec4i>& srcLines)
{	
	
	
	if(srcLines.size()==0)
		return;
	Dots.clear();
	Mat diff;
	Mat padding=Mat::zeros(Size(frame.cols+templSize,frame.rows+templSize),CV_8UC3);
	//沿线计算的步长
	step=5;
	
	//霍夫变换输出的直线两端点都是上端点在前面
	for(size_t i=0;i<srcLines.size();i++)
	{
		
		int x1=srcLines[i][0],y1=srcLines[i][1],x2=srcLines[i][2],y2=srcLines[i][3];
		double k=(double)(y1-y2)/(x1-x2);
		if(k<0)
		{
			for(int x=x1,j=0;x<x2;x+=step,j++)
			{
			int y=cvRound(k*(x-x1)+y1);
			//获取y坐标并取整到整数坐标
			//roi along the line got
			frame(Rect(x-templSize,y-templSize,x+templSize,y+templSize)).copyTo(templ);
			templ.copyTo(curTempl);

			//circle tags which position is under process
			circle(tem,Point(x,y),2,Scalar(255,0,0),2,8);
			sd<<tem;
			imshow("pic1",tem);
			
			waitKey(1);
			if(x==x1)
			{curTempl.copyTo(preTempl);continue;}
			else
			{

				absdiff(preTempl,curTempl,diff);

				int dotP=Mat(preColor).dot(Mat(curColor));
				//得到沿线像素的偏差和对应的坐标，存入
				double diff=dotP/(mol1*mol2);
				//当cos小于一定值的时候为入侵物体
				if(diff<0.85)
				{

					Dots.push_back(Point(x,y));
					rectangle(tem,Point(x-9,y-9),Point(x+9,y+9),Scalar(0,255,0),2);
					sd<<tem;
					cout<<diff<<endl;
				}
			}
			preColor=curColor;
			}
		}
		else if(k>0)

	}


}
*/
////////////////////////////////////////////////////////////////////////////////////////



void detector::drawContour()
{
	
	int n=Dots.size();
	if(n==0)return;
	for(int i=0;i<n;i++)
	{
		circle(tem,Dots[i],40,Scalar(255,255,255),2,8);
		
		imshow("pic1",tem);
		waitKey(1);
		sd<<tem;
	}

}
/////////////////////////////////////////////////////////////////////////////////
//along the line detection , non windowed, colored
void alongTheLineDetector::alongTheLine(const vector<Vec4i>& srcLines)
{
	if(srcLines.size()==0)
		return;
	Dots.clear();
	Vec3b preColor,curColor;
	//沿线计算的步长
	//slope
	double k;

	for(size_t i=0;i<srcLines.size();i++)
	{
		int x1=srcLines[i][0],y1=srcLines[i][1],x2=srcLines[i][2],y2=srcLines[i][3];
		
		//防止直线垂直于x轴时导致斜率过大出错
		if(x1!=x2)
		 k=(double)(y1-y2)/(x1-x2);
		else k=BIG_O_ASS_LARGE_NUMBER;
		//直线更趋于平行于屏幕长边时
		if(abs(k)<1&&k!=BIG_O_ASS_LARGE_NUMBER)
		{

			for(int x=x1,j=0;x<x2;x+=step,j++)
			{
			int y=cvRound(k*(x-x1)+y1);
			//获取y坐标并取整到整数坐标
			curColor=input.at<Vec3b>(y,x);
			//paint that bitch!
			circle(tem,Point(x,y),2,Scalar(255,0,0),2,8);
			sd<<tem;
			//imshow("pic1",tem);
			//waitKey(1);
			if(x==x1)
			{preColor=curColor;continue;}
			else
			{

				double mol1=norm(preColor,NORM_L2),mol2=norm(curColor,NORM_L2);
				int dotP=Mat(preColor).dot(Mat(curColor));
				//得到沿线像素的偏差和对应的坐标，存入
				double diff=dotP/(mol1*mol2);
				//当cos小于一定值的时候为入侵物体
				if(diff<0.85)
				{
					
					Dots.push_back(Point(x,y));
					//paint 
					rectangle(tem,Point(x-9,y-9),Point(x+9,y+9),Scalar(0,255,0),2);
					circle(tem,Point(x,y),40,Scalar(255,255,255),2,8);
					sd<<tem;
					//imshow("pic1",tem);
					//waitKey(1);
					cout<<diff<<endl;
				}
			}
			preColor=curColor;
			}
		}
		//直线更平行于宽边时
		else if(abs(k)>1)
		 {
			//交换x,y轴计算
			
			//若y1在y2的下面，将其换到上面，这样更好处理
			if(y1>y2)
				mySwap(y1,y2);
			k=1/k;	
			for(int y=y1,j=0;y<y2;y+=step,j++)
			{
			int x=cvRound(k*(y-y1)+x1);
			//获取y坐标并取整到整数坐标
			curColor=input.at<Vec3b>(y,x);
			//paint that bitch!
			circle(tem,Point(x,y),2,Scalar(255,0,0),2,8);
			sd<<tem;
			//imshow("pic1",tem);
			//waitKey(1);
			if(y==y1)
			{preColor=curColor;continue;}
			else
			{

				double mol1=norm(preColor,NORM_L2),mol2=norm(curColor,NORM_L2);
				int dotP=Mat(preColor).dot(Mat(curColor));
				//得到沿线像素的偏差和对应的坐标，存入
				double diff=dotP/(mol1*mol2);
				//当cos小于一定值的时候为入侵物体
				if(diff<0.85)
				{
					Dots.push_back(Point(x,y));
					//paint
					rectangle(tem,Point(x-9,y-9),Point(x+9,y+9),Scalar(0,255,0),2);
					circle(tem,Point(x,y),40,Scalar(255,255,255),2,8);
					sd<<tem;
					//imshow("pic1",tem);
					//waitKey(1);
					cout<<diff<<endl;
				}
			}
			preColor=curColor;
			}
		 }
		drawContour();

	}
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
alongTheLineDetector_1::alongTheLineDetector_1(int _step,Mat _input,int _maskSize, int _kernelSize):
detector(_step,  _input, _maskSize),
		kernelSize(_kernelSize),
		pointGradInfo(0),
		specPoint(0),
		lineSlope(UNASSIGNED),
		avgPercent(0),
		unusualPoints(0)
{
		assert(input.channels()==1&&input.depth()==CV_8U);
		
		//construct a mask offset
		coorOffset.create(Size(maskSize,maskSize),CV_32SC1);
		int *pOffset;
		for(int i=0;i<maskSize;i++)
		{
			pOffset=coorOffset.ptr<int>(i);
			for(int j=0;j<maskSize;j++)
				* (pOffset+j)=i* maskSize+j;
		}

		maskArea=maskSize*maskSize;
}


 void alongTheLineDetector_1::getKofLine(const Point& pt_1,const Point& pt_2 )
{
	//保证角度在0到2*Pi之内
	vector<double> magnitude,slope;
	cartToPolar(pt_1.x-pt_2.x,pt_1.y-pt_2.y,magnitude,slope,false);
	lineSlope=*slope.begin();
}




//////////////////////////////////////////////////////////////////////////////////

void alongTheLineDetector_1::alongTheLine(const vector<Vec4i>& srcLines)
{
	//for all the lines in a ROI,execute once at each frame to save resources
	getGradOfROI(srcLines);
	

	if(srcLines.size()==0)
		return;
	//for the border where could get wrong results
	int wing=(maskSize-1)/2;

	double k;

	for(size_t i=0;i<srcLines.size();i++)
	{
		int x1=srcLines[i][0],y1=srcLines[i][1],
			x2=srcLines[i][2],y2=srcLines[i][3];
		
		if(x1!=x2)
			k=(double)(y1-y2)/(x1-x2);
		else 
			k=BIG_O_ASS_LARGE_NUMBER;
		//store the k of the line
		getKofLine(Point(x1,y1),Point(x2,y2));

		if(abs(k)<1&&k!=BIG_O_ASS_LARGE_NUMBER)
		{
			//main loop
			for(int x=x1+wing,j=0;x<x2-wing;x+=step,j++)
			{
			int y=cvRound(k*(x-x1)+y1);

			//process here
			calcGrad(Point(x,y));
			}		
		}
		else if(abs(k)>1)
		 {
			if(y1>y2)
				mySwap(y1,y2);

			k=1/k;	
			for(int y=y1+wing,j=0;y<y2-wing;y+=step,j++)
			{
			int x=cvRound(k*(y-y1)+x1);
			calcGrad(Point(x,y));

			}
		 }
	

	}//end lines for here
	drawGrad();
}



////////////calculate the all the info about gradient at a point/////////////

void alongTheLineDetector_1::calcGrad(const Point& curLoc)
{
	
	int dataType;
	//we only take edge img into consideration
	int wing=(maskSize-1)/2;

	if( curLoc.x>maskSize&&
		curLoc.y>maskSize&&
		curLoc.x<input.cols-maskSize&&
		curLoc.y<input.rows-maskSize)
		mask=Mat::Mat(input,Rect(Point(curLoc.x-wing,curLoc.y-wing),Point(curLoc.x+wing+1,curLoc.y+wing+1)));
	else 
	{
		cout<<"skip those close to the border"<<endl;
		return;
	}
	//debug the algorithm
	

	//Mat showMaskArea(input);
	//cvtColor(showMaskArea,showMaskArea,CV_GRAY2BGR);
	//rectangle( showMaskArea,Point(curLoc.x-wing,curLoc.y-wing),Point(curLoc.x+wing+1,curLoc.y+wing+1),Scalar(255,0,0) );
	//show(showMaskArea);



	gradInfo thisPointInfo;


	
	
	
	int offset=0;
	

	int offsetX=0,offsetY=0;

	//get the start position
	itMask=mask.begin<uchar>();
	itMag=Mag.begin<float>();
	itAng=Ang.begin<float>();
	//temp var
	thisPointInfo.anchor = curLoc;
	Statis temp_statis;
	
	int desiredLength=10;
	
	for(int shit=0;shit<mask.rows;shit++)
	{

		for(int fuck=0;fuck<mask.cols;fuck++)
		{

			if(itMask!=endMask)
			{

				//debug
				//Point debug_now=curLoc + Point(offset%maskSize-wing,offset/maskSize-wing);
				//showMaskArea.begin<Vec3b>()[debug_now.x+showMaskArea.cols*debug_now.y]=Vec3b(0,0,255);
				//show(showMaskArea);

				if(*itMask > LOW_B)
				{
					//main calculation
					//store the slope of the line
					if(lineSlope==UNASSIGNED)
					{
						cout<<"error getting line slope"<<endl;
						exit(1);
					}
					thisPointInfo.lineAng=lineSlope;

					//store these motherfucka
					double tempMag= *(itMag+ (curLoc.x-wing+offset%maskSize) + (curLoc.y-wing+offset/maskSize)*input.cols  );
					double tempAng= *(itAng+ (curLoc.x-wing+offset%maskSize) + (curLoc.y-wing+offset/maskSize)*input.cols  );

					thisPointInfo.gradRawMag.push_back(tempMag);
					thisPointInfo.gradRawAng.push_back(tempAng);

					//calculate the differences between the vector and the threshold
					double diff[4];
					diff[0]=abs((tempAng-lineSlope) - CV_PI/2);
					diff[1]=abs((tempAng-lineSlope) + CV_PI/2);
					diff[2]=abs((tempAng-lineSlope) + 3 * CV_PI/2);
					diff[3]=abs((tempAng-lineSlope) - 3 * CV_PI/2);
					double lowestDiff=min(min(diff[0],diff[1]),min(diff[2],diff[3]));
					temp_statis.Diff.push_back(lowestDiff);

			

					//calc the end point
					Point startPoint=curLoc+Point(offset%maskSize-wing,offset/maskSize-wing);

					//debug the algorithm
					//showMaskArea.begin<Vec3b>()[showMaskArea.cols*startPoint.y+startPoint.x]=Vec3b(0,255,0);
					//imshow("pic1",showMaskArea);
					//show(showMaskArea);
					
					//this is for debug method of printing results
					offsetX = desiredLength*cos(tempAng);
					offsetY = desiredLength*sin(tempAng);
					Point endPoint =startPoint + Point(offsetX, offsetY);
					Vec4i temp_pair_pt;
					temp_pair_pt[0]=startPoint.x,temp_pair_pt[1]=startPoint.y;
					temp_pair_pt[2]=endPoint.x,temp_pair_pt[3]=endPoint.y;
					thisPointInfo.location.push_back(temp_pair_pt);

					//get anchor index in realistic vector
					//if(offset==(wing*maskSize+wing))
				    ///thisPointInfo.indexAnchor=index;

			
					//
					thisPointInfo.maskSize=maskSize;
		

				}

			}
			offset++;
			itMask++;


		}



	}


	//////////////////////////////////////////////////////////////////////////
	//we've store some data in there, so we'd like to calc the statistic part
	//statis collect
	

	temp_statis.totalNum=thisPointInfo.gradRawAng.size();
	calcPercent(temp_statis);
	//overall percent, not have being divided
	avgPercent+=temp_statis.unusualPercent;


	thisPointInfo.statis=temp_statis;
	pointGradInfo.push_back(thisPointInfo);
}


/////////////////////////////paint that B////////////////////////////////////
/*
*can optim!!
*/
void alongTheLineDetector_1::drawGrad()
{

	getGoodPosition();

	Mat showGrad(input);
			cvtColor(showGrad,showGrad,CV_GRAY2BGR);
			MatIterator_<Vec3b> itImg=showGrad.begin<Vec3b>();

	gradInfo tempInfo;

	

	
	//paint by different demands
	for(int i=0;i< pointGradInfo.size();i++)
	{
		//for each point in one same window
		for(int eachInOneWindow=0;eachInOneWindow < pointGradInfo[i].gradRawAng.size();eachInOneWindow++)
		{

			

			Point startPt=Point(pointGradInfo[i].location[eachInOneWindow][0],pointGradInfo[i].location[eachInOneWindow][1]);
			Point endPt  =Point(pointGradInfo[i].location[eachInOneWindow][2],pointGradInfo[i].location[eachInOneWindow][3]);
			
			itImg[input.cols * startPt.y+startPt.x]=Vec3i(255,0,0);

			
#ifdef PAINT_ALL_VECTOR
			
			//line(showGrad,startPt,endPt,Scalar(255,0,0));
			
			//show(showGrad);
			
#endif

#ifdef PAINT_GOOD_VECTOR
			if(pointGradInfo[i].statis.indicator[eachInOneWindow])
			{
				line(showGrad,startPt,endPt,Scalar(255,255,0));
				
				
			}
			else
				continue;
#endif
			
		}


	}
	//point out where the bad points are
	for(int i=0;i<unusualPoints.size();i++)
	{

		int sizeUnusualBlock=unusualPoints.size();
		//always paint unusual points
		
			for(int eachInOneWindow=0;eachInOneWindow < unusualPoints[i]->gradRawAng.size();eachInOneWindow++)
			{
				Point startPt=Point(unusualPoints[i]->location[eachInOneWindow][0],unusualPoints[i]->location[eachInOneWindow][1]);
				Point endPt  =Point(unusualPoints[i]->location[eachInOneWindow][2],unusualPoints[i]->location[eachInOneWindow][3]);
				itImg[input.cols * startPt.y+startPt.x]=Vec3i(0,255,0);

			//always paint the good VECTORS
				
				//if(unusualPoints[i]->statis.indicator[eachInOneWindow])
				//	line(showGrad,startPt,endPt,Scalar(0,255,0));
				//else 
				//	continue;
			    
			}
	}
	
	show(showGrad);

}

void alongTheLineDetector_1::calcPercent(Statis &statis)
{
	int numUnusual=0;
	for(int i=0;i<statis.Diff.size();i++)
	{
		if(statis.Diff[i]<THRESH_ANG)
		{
			statis.indicator.push_back(true);
			
		}
		else
		{
			statis.indicator.push_back(false);
			numUnusual++;
		}
		
	}
	
	statis.unusualNum=numUnusual;
	if((statis.unusualPercent=(double)numUnusual/statis.totalNum)>1)
	{
		cout<<"percentage above unique, sth's gonna be wrong"<<endl;
	}
	
}


void alongTheLineDetector_1::getGoodPosition()
{
	avgPercent=avgPercent/(pointGradInfo.size());
	vector<gradInfo>::iterator iter;
	for(iter=pointGradInfo.begin();iter!=pointGradInfo.end();iter++)
	{
		if((*iter).statis.unusualPercent<avgPercent)
			(*iter).statis.if_unusual=false;
		else
		{
			(*iter).statis.if_unusual=true;
			gradInfo* tempInfo(NULL);
			tempInfo=&(*iter);
			unusualPoints.push_back(tempInfo);
		}
	}

}


//get the Gradient matrix Of ROI pic buffer
void alongTheLineDetector_1::getGradOfROI(const vector<Vec4i>& lines)
{
	Extractor *extractor= new Extractor(input);
	extractor->lineContour(lines);
	extractor->getGOIgray();

	//get an new input only containing the ROI
	extractor->frame.copyTo(input);

	
	//temp variable
	Mat grad_x,grad_y;
	Sobel(extractor->frame,grad_x,CV_32F,1,0,kernelSize);
	
	
	Sobel(extractor->frame,grad_y,CV_32F,0,1,kernelSize);
	//calc the gradient
	cartToPolar(grad_x,grad_y,Mag,Ang);
	
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
alongTheLineDetector_2::alongTheLineDetector_2(int _step,Mat _input,int _maskSize, int _kernelSize):
	detector(_step,  _input, _maskSize)
{

}