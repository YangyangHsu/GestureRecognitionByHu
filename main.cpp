
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include<iostream>

using namespace cv; 
using namespace std;



Mat ball;										//包含球的图像
int ballRadius = 30;							//球的半径
Point ballCenter;								//球的中心


//计算Hu矩
void HU(Mat &handContour, double& distHus)
{
	Moments mymoments1;
	double hu[7];
	mymoments1 = moments(handContour,0); 
	HuMoments(mymoments1, hu);
	distHus = 100*hu[0];

}

//获得手的轮廓
bool FindhandContour(const Mat &frame, Mat &handContour)  
{
 //HSV
	if(frame.empty())
	{
		return false;
	}
    Mat hsv;
	Mat imb;
	Mat imb2;
	Mat imy;
	Mat img = frame;
	
	cvtColor(img,hsv,CV_BGR2HSV);
	vector<Mat> a;
	split(hsv,a);
	Mat h=a[0];
	Mat s=a[1];
	Mat v=a[2];

	inRange(h,0,70,a[0]);
	inRange(s,0,200,a[1]);
	inRange(v,130,380,a[2]);

	bitwise_and(a[0],a[1],imb);bitwise_and(a[2],imb,imb);
	 //膨胀和腐蚀，膨胀可以填补凹洞（将裂缝桥接），腐蚀可以消除细的凸起（“斑点”噪声）  
	dilate(imb, imb, Mat(6, 6, CV_8UC1), Point(-1, -1));    
    erode(imb, imb, Mat(5, 5, CV_8UC1), Point(-1, -1));
	//轮廓
	vector<vector<Point> > contours; 
	vector<Vec4i> hierarchy;
	
	
	findContours(imb, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);  
  
    int index=0;  
    double area, maxArea(0);  
    for (int i=0; i < contours.size(); i++)  
    {  
        area = contourArea(Mat(contours[i]));  
        if (area > maxArea)  
        {  
            maxArea = area;  
            index = i;  
        }             
    }  
		
	handContour = Mat::zeros(img.rows,img.cols, CV_8UC1);
    drawContours(handContour, contours,index, Scalar(255, 255, 255), -1, 8, hierarchy ); 

	return true;
 }

//球在静止状态中
void Show(int lastPosx,int lastPosy)
{
	ball = imread("black.jpg");
	ballCenter = Point(lastPosx,lastPosy);  
    circle(ball, ballCenter, ballRadius, Scalar(255, 0, 0), -1, 8, 0);  
	if (!ball.empty()) 
	{
		imshow("Ball",ball);
	}	
}

//球向左移动
void ShowLeft(int lastPosx,int lastPosy)
{
	ball = imread("black.jpg");
	ballCenter = Point(lastPosx - 10,lastPosy);  
    circle(ball, ballCenter, ballRadius, Scalar(255, 0, 0), -1, 8, 0);  

	if (!ball.empty())
	{
		imshow("Ball",ball);
	}

}

//球向右移动
void ShowRight(int lastPosx,int lastPosy)
{	
	ball = imread("black.jpg");
	ballCenter = Point(lastPosx + 10,lastPosy);  
    circle(ball, ballCenter, ballRadius, Scalar(255, 0, 0), -1, 8, 0);  

	if (!ball.empty()) 
	{
		imshow("Ball",ball);
	}
	
}

//球向上移动
void ShowUp(int lastPosx,int lastPosy)
{

	ball = imread("black.jpg");
	ballCenter = Point(lastPosx,lastPosy - 10);  
    circle(ball, ballCenter, ballRadius, Scalar(255, 0, 0), -1, 8, 0);  
	
	if (!ball.empty())
	{
		imshow("Ball",ball);
	}
	
}

//球向下移动
void ShowDown(int lastPosx,int lastPosy)
{
	
	ball = imread("black.jpg");
	ballCenter = Point(lastPosx,lastPosy + 10);  
    circle(ball, ballCenter, ballRadius, Scalar(255, 0, 0), -1, 8, 0);  
	
	if (!ball.empty()) 
	{
		imshow("Ball",ball);
	}	
}

//防止误操作，两帧
bool waitTime(int move1, int move2)
{
	bool sameFlag;
	if (move1 == move2)
		sameFlag = 1;
	else sameFlag = 0;
	return sameFlag;
}

int main()  
{  
	int move1 = -1;          //前两帧的动作状态
	int move2 = -1;          //前一帧的动作状态
	int oriPosx = 500;       //球初始位置
	int oriPosy = 400;

	int lastPosx;            //球前一帧位置
	int lastPosy;

	bool sameFlag = 0;       //判断前两帧是否一致，一致为1，不一致为0

	VideoCapture cap(0);  
    if(!cap.isOpened())  
    {  
        return -1;  
    }  

    Mat frame;               //摄像头数据
    Mat handContour;         //手的轮廓
	lastPosx = oriPosx;
	lastPosy = oriPosy;
	bool stop = false;       //控制摄像头读取 
	double distHus;          //Hu 的数值

    while(!stop)  
    {  
	    cap>>frame;  
	    
	
		FindhandContour(frame,handContour);
		HU(handContour,distHus);
	

		if (lastPosx<30)
			lastPosx = 30;
		if (lastPosx>994)
			lastPosx = 994;
		if (lastPosy<30)
			lastPosy = 30;
		if (lastPosy>930)
			lastPosy= 930;


		sameFlag = waitTime(move1, move2);  
		cout<<"distHus"<<distHus<<endl;

		if(distHus >=0.07 && distHus <0.08)  // 6
		{
			cout<<"6"<<endl;
			putText(frame,"6", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0,255) );  
			if (sameFlag == 1)
			{
			ShowDown(lastPosx, lastPosy);
			lastPosy = lastPosy + 10;
			}
			move1 = move2;
			move2 = 6;
		}
		else if (distHus >=0.06 && distHus < 0.07)  // 0
		{
			cout<<"0"<<endl;
			putText(frame,"0", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255) );  
			if (sameFlag == 1)
			{
			Show(lastPosx, lastPosy);
			}
			move1 = move2;
			move2 = 0;
		}
		else if ( distHus >=0.1 && distHus <= 0.2) // 1
		{
			cout<<"1"<<endl;
			putText(frame,"1", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0,255) );  
			if (sameFlag == 1)
			{
			ShowLeft(lastPosx, lastPosy);
			}
			lastPosx = lastPosx - 10;
			move1 = move2;
			move2 = 1;
		}
		else if (distHus >=0.09 && distHus < 0.1) // 2
		{
			cout<<"2"<<endl;
			putText(frame,"2", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0,255) );  
			if (sameFlag == 1)
			{
			ShowRight(lastPosx,lastPosy);
			lastPosx = lastPosx + 10;
			}
			move1 = move2;
			move2 = 2;
		}			   
		else if( distHus >=0.08 && distHus <0.09) // 3
		{
			cout<<"3"<<endl;
			putText(frame,"3", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0,255) );  
			if (sameFlag == 1)
			{
			ShowUp(lastPosx,lastPosy);
			lastPosy = lastPosy - 10;
			}
			move1 = move2;
			move2 = 3;
		}
		else  //都不是
		{ 
			cout<<"No"<<endl;
			putText(frame,"NO", Point(0,100),CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255) );  
		}


		if (!frame.empty()) 
		{
			imshow("Frame", frame);
		}
	
	    if(waitKey(30) >=0)  
			stop = true;  

	}  //while(!stop)  

	return 0;  
}  




