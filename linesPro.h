class lineProcess
{
public:
	
		   
		lineProcess(vector<Vec4i> _lines,Mat &pic):lines(_lines),showPic(pic){};
		void drawLines()
		{
			for(size_t i=0;i<lines.size();i++)
			{
				line(showPic,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(255,0,0),2);
			}
		}



		OutputPic showPic;
	private:
		vector<Vec4i> lines;
		
	};
			



