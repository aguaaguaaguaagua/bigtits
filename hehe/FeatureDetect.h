
void FlanDescriptorExtract(const Mat&,const string &);
void SurfFeatureDetect(const Mat&, const Mat&,vector<KeyPoint>&);
/////////////////////////////////////////////////
void SurfFeatureDetect(const Mat& src,vector<KeyPoint>& keyPoints)
{
	Mat dst;
	int cn;
	if((cn=src.channels())!=1)
		{
			
			cvtColor(src,dst,CV_RGB2GRAY);
		}
	else
	dst=src;
	int minHessian = 400;

    SurfFeatureDetector detector( minHessian );
	
	detector.detect(dst,keyPoints);
	Mat keyPointsPic;
	drawKeypoints(dst,keyPoints,keyPointsPic,Scalar(255,0,0),DrawMatchesFlags::DEFAULT);
	namedWindow("KeyPoints Pic");
	imshow("KeyPoints Pic",keyPointsPic);
}

void FlanDescriptorExtract(const Mat& src1,const Mat& src2,const string &exType, const string &detectType)
{
	//it;s kinda fucked up cause the two types just don't match
	vector<KeyPoint> keyPoints1,keyPoints2;
	Ptr<DescriptorExtractor> myExtractor=DescriptorExtractor::create(exType);
	SurfFeatureDetect(src1, keyPoints1);
	SurfFeatureDetect(src2, keyPoints2);
	Mat descriptors1,descriptors2;
	myExtractor->compute(src1,keyPoints1,descriptors1);
	myExtractor->compute(src2,keyPoints2,descriptors2);
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors1, descriptors2, matches );
	double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
   for( int i = 0; i < descriptors1.rows; i++ )
  { 
	double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
   std::vector< DMatch > goodMatches;
    for( int i = 0; i < descriptors1.rows; i++ )
	{
		if(matches[i].distance<min_dist*2)
			goodMatches.push_back(matches[i]);
	}
	Mat matchPic,matchPic1;
	drawMatches(src1,keyPoints1,src2,keyPoints2,goodMatches,matchPic,
		Scalar::all(-1),Scalar::all(-1),vector<char>(),
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	resize(matchPic,matchPic1,Size(1024,960));
	namedWindow("match pic",CV_WINDOW_AUTOSIZE);
	imshow("match pic",matchPic1);
}