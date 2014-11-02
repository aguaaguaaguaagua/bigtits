
void histBackproj(Mat&);

void histBackproj(Mat& src,Mat &dst)
{

  
  
  Mat hsv; Mat hue;  
  cvtColor( src, hsv, CV_BGR2HSV );

  /// ���� Hue ͨ��
  hue.create( hsv.size(), hsv.depth() );
  int ch[] = { 0, 0 };
  mixChannels( &hsv, 1, &hue, 1, ch, 1 );

  int bins=50;
  MatND hist;
  int histSize = MAX( bins, 2 );
  float hue_range[] = { 0, 180 };
  const float* ranges = { hue_range };

  /// ����ֱ��ͼ����һ��
  calcHist( &hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false );
  normalize( hist, hist, 0, 255, NORM_MINMAX, -1, Mat() );

  /// ���㷴��ͶӰ
  MatND backproj;
  calcBackProject( &hue, 1, 0, hist, backproj, &ranges, 1, true );
  threshold(backproj,backproj,100,255,THRESH_BINARY_INV);
  /// ��ʾ����ͶӰ
  namedWindow("BackProj",0);
  imshow( "BackProj", backproj );
  backproj.copyTo(dst);


}