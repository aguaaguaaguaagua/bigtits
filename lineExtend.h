

class lineEx
{
public:
	lineEx(){};
	//����Ѱ����������
	void aloneTheLine(Point& ,Point&);
	//����ģ������С���ز��
	double diff(Vec3b&,Vec3b&);
private:
	//Ѱ�ҵĲ���
	 int step;
}myLineEx;

////////////////////////////////////////////////////////////////////////////////////
int clip(int *i,int lim);
void lineEx::aloneTheLine(Point& p1,Point& p2)
{
	int dir=0;
	//imgת����Mat����copy����
	
	Mat myMask(Size(3,3),CV_8UC3);
	step=1;
	if(p1.x<p2.x)
		//��������
		dir=-1;
	else dir=1;//��������
	Point temp1,temp2;
	Vec3b preColor,curColor;
	
	
   int x1=p1.x,y1=p1.y,x2=p2.x,y2=p2.y;
   float k=(float)(y1-y2)/(x1-x2);
   int _x=x1,_y=0;
   int yt=cvRound(k*(_x-x1)+y1);
	//б��
	while(_x>=0&&_y>=0&&_x<frame.cols&&_y<frame.rows)
	{
		//��ֱ�߷��̼���y��ȡ��
			if(_x==x1)
			{preColor=curColor;_x+=dir*step;_y=cvRound(k*(_x-x1)+y1);continue;}
			int i=_y,j=_x;
			curColor=frame.at<Vec3b>(_y,_x);
			frame.at<Vec3b>(_y,_x)=Vec3b(0,0,255);
			
				double differ=diff(preColor,curColor);
				
				//
				if(differ<0.85)
				{
					
					cout<<differ<<endl;
					break;
				}
			
			preColor=curColor;
			_x+=dir*step;
			_y=cvRound(k*(_x-x1)+y1);
		}
	//�����һ�������
	temp1.x=clip(&_x,frame.cols);
	temp1.y=clip(&_y,frame.rows);
	_x=x2,_y=0;
	dir=0-dir;
	while(_x>=0&&_y>=0&&_x<frame.cols&&_y<frame.rows)
	{
		//��ֱ�߷��̼���y��ȡ��
			
			if(_x==x1)
			{preColor=curColor;_x+=dir*step;_y=cvRound(k*(_x-x1)+y1);continue;}
			curColor=frame.at<Vec3b>(_y,_x);
			frame.at<Vec3b>(_y,_x)=Vec3b(0,0,255);
			
				double differ=diff(preColor,curColor);
				
				//
				if(differ<0.85)
				{
					
					cout<<differ<<endl;
					break;
				}
			
			preColor=curColor;
			_x+=dir*step;
			_y=cvRound(k*(_x-x1)+y1);
		}
	//����ڶ��������
	temp2.x=clip(&_x,frame.cols);
	temp2.y=clip(&_y,frame.rows);
	p1=temp1;p2=temp2;
	cout<<"before"<<p1<<"_"<<p2<<endl;
	cout<<"after"<<temp1<<"_"<<temp2<<endl;
	}
double lineEx::diff(Vec3b& c1,Vec3b& c2)
{
	double mol1=norm(c1,NORM_L2),mol2=norm(c2,NORM_L2);
	int dotP=Mat(c1).dot(Mat(c2));
	//�õ��������ص�ƫ��Ͷ�Ӧ�����꣬����
	float diff=dotP/(mol1*mol2);
	return diff;
}
int clip(int *i,int lim)
{
	if(*i>=lim)
		*i=lim;
	else if(*i<0)
		*i=0;
	return *i;
}