#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <time.h>

#define N 3

/////  Convert Color BGR to HSV  //////
/*
IplImage* BGR2HSV(IplImage *frame, IplImage *imgHSV)
{
	uchar* data_bgr = (uchar *)frame->imageData, *data_hsv = (uchar *)imgHSV->imageData;  
    int step_bgr = frame->widthStep/sizeof(uchar), step_hsv = imgHSV->widthStep/sizeof(uchar); 
    int chanels_bgr = frame->nChannels, chanels_hsv = imgHSV->nChannels; 
    unsigned char B,G,R;
    float vmin, diff;
    float H1,S1,V1;
    int a,b;
        
    for(a=0;a<frame->height;a++)
        for(b=0;b<frame->width;b++)
        {
			B=data_bgr[a*step_bgr+b*chanels_bgr+0];
			G=data_bgr[a*step_bgr+b*chanels_bgr+1];
			R=data_bgr[a*step_bgr+b*chanels_bgr+2];

            V1 = vmin = R;
            if( V1 < G ) V1 = G;
            if( V1 < B ) V1 = B;
            if( vmin > G ) vmin = G;
            if( vmin > B ) vmin = B;
            
            diff = V1 - vmin;
            S1 = diff/(float)(fabs(V1) + FLT_EPSILON);
            diff = (float)(60./(diff + FLT_EPSILON));
            if( V1 == R )
                H1 = (G - B)*diff;
            else if( V1 == G )
                H1 = (B - R)*diff + 120;
            else
                H1 = (R - G)*diff + 240;

            if( H1 < 0 ) H1 += 360;
            
            H1 = H1 * 0.502577328;
            S1 = S1 * 255;

            data_hsv[a*step_hsv+b*chanels_hsv+0]=H1;
			data_hsv[a*step_hsv+b*chanels_hsv+1]=S1;
			data_hsv[a*step_hsv+b*chanels_hsv+2]=V1;
				
        }    
         
  return imgHSV;	
}
*/

/////  Image Binaryzation  //////
IplImage* Threshold(IplImage* imgThreshed,IplImage* imgHSV,unsigned char min[],unsigned char max[])
{
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;  
    int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char) ;  
    int chanels_hsv = imgHSV->nChannels;
    int a,b;  
    unsigned char H,S,V;
    
    for(a=0;a<imgHSV->height;a++)
        for(b=0;b<imgHSV->width;b++)
        {
			H = data_hsv[a*step_hsv+b*chanels_hsv+0];
			S = data_hsv[a*step_hsv+b*chanels_hsv+1];
			V = data_hsv[a*step_hsv+b*chanels_hsv+2];
			
			if( (H >= min[0] && H <= max[0] && S >= 170 && S<=255 && V>=75 && V<=255)
			||  (H >= min[1] && H <= max[1] && S >= 170 && S<=255 && V>=75 && V<=255)
			||  (H >= min[2] && H <= max[2] && S >= 170 && S<=255 && V>=75 && V<=255)  )
			{
				data_ts[a*step_ts+b]=255;
				
			}else{	
		
				data_ts[a*step_ts+b]=0;

			    }				
	     }  	
	
  return imgThreshed;	
}

/////  Find Blobs  /////
IplImage* Blob(IplImage* frame,IplImage* imgHSV,IplImage* imgThreshed,unsigned char min[],unsigned char max[])
{   
	int i=0,j=0,m=0,k,l;
	int total, avg; 
	double area,tmp_area;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0; 
	double maxArea1[N]={0},maxArea2[N]={0},maxArea3[N]={0},maxArea4[N]={0};
    
    CvSeq *contour,*tmp_cont;
    CvSeq *contours1[N],*contours2[N],*contours3[N],*contours4[N];
	CvPoint *pt;
	CvScalar data;
	
	unsigned char *data_hsv = (unsigned char *)imgHSV->imageData;  
    int step_hsv = imgHSV->widthStep/sizeof(unsigned char) ;  
    int chanels_hsv = imgHSV->nChannels; 
    unsigned char H;
	
	CvMemStorage *storage = cvCreateMemStorage(0); 
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	
	for(;contour;contour = contour->h_next)
	{
		area=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area<100 || area>50000 )
        {
			cvSeqRemove(contour,0);
		    continue;
	    }   
		
		total =0;      
	
        for(l = 0;l<contour->total;++l)	
        {
			pt = (CvPoint *)cvGetSeqElem(contour,l);
			H = data_hsv[step_hsv*pt->y+chanels_hsv*pt->x+0];
			total = H + total;
		}
		
		avg = total / (contour->total);
			
        
		if((avg>=min[0])&&(avg<=max[0]))  //red color
		{
			cnt1++;
			
            for(i = N-1; i >= 0; --i)
            {
              if(area > maxArea1[i])
              {
                maxArea1[i] = area;
                contours1[i] = contour;
                for(m = (i-1); m >= 0; --m)
                {
                   if(maxArea1[m] < maxArea1[m+1])
                   {
                     tmp_area = maxArea1[m+1];
                     tmp_cont = contours1[m+1];
                     maxArea1[m+1] = maxArea1[m];
                     contours1[m+1] = contours1[m];
                     maxArea1[m] = tmp_area;
                     contours1[m] = tmp_cont;
                    }
                }
              break;
              }
            }	   
			
		}
		else if((avg>=min[1])&&(avg<=max[1])) 	 // yellow color
		{
			cnt2++;
			
            for(i = N-1; i >= 0; --i)
            {
              if(area > maxArea2[i])
              {
                maxArea2[i] = area;
                contours2[i] = contour;
                for(m = (i-1); m >= 0; --m)
                {
                   if(maxArea2[m] < maxArea2[m+1])
                   {
                     tmp_area = maxArea2[m+1];
                     tmp_cont = contours2[m+1];
                     maxArea2[m+1] = maxArea2[m];
                     contours2[m+1] = contours2[m];
                     maxArea2[m] = tmp_area;
                     contours2[m] = tmp_cont;
                    }
                }
              break;
              }
            }	   
				
		}
		else if((avg>=min[2])&&(avg<=max[2])) 	//Blue Color
		{
			cnt3++;
			
			for(i = N-1; i >= 0; --i)
            {
              if(area > maxArea3[i])
              {
                maxArea3[i] = area;
                contours3[i] = contour;
                for(m = (i-1); m >= 0; --m)
                {
                   if(maxArea3[m] < maxArea3[m+1])
                   {
                     tmp_area = maxArea3[m+1];
                     tmp_cont = contours3[m+1];
                     maxArea3[m+1] = maxArea3[m];
                     contours3[m+1] = contours3[m];
                     maxArea3[m] = tmp_area;
                     contours3[m] = tmp_cont;
                    }
                }
              break;
              }
            }
			
		}

	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
		printf("Red: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
	}	
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		printf("Yellow: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
	}	
	
	if(cnt3 != 0)
	{
		CvRect rect = ((CvContour*)contours3[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
		printf("Blue: (%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height);
	}	
	
			
/*     
     if(cnt1 >= N) k = N;
	 else k = cnt1;
	 
	 if(k != 0)
	 {
		 for(j=0;j < k; j=j+1)
		 {
		   CvRect rect = cvBoundingRect(contours1[j],0);
		   cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 0), 2, 8, 0);
		   printf("Red %d : ", j+1);
		   printf("(%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height); 
		 }
     }
	 
	 if(cnt2 >= N) k = N;
	 else k = cnt2;
	 
	 if(k != 0)
	 {
		 for(j=0;j < k; j=j+1)
		 {
		   CvRect rect = cvBoundingRect(contours2[j],0);
		   cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		   printf("Yellow %d : ", j+1);
		   printf("(%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height); 
		 }
     }
     
	 if(cnt3 >= N) k = N;
	 else k = cnt3;
	 
	 if(k != 0)
	 {
		 for(j=0;j < k; j=j+1)
		 {
		   CvRect rect = cvBoundingRect(contours3[j],0);
		   cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(100, 100, 0), 2, 8, 0);
		   printf("Green %d : ", j+1);
		   printf("(%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height); 
		 }
     }
	 
	 if(cnt4 >= N) k = N;
	 else k = cnt4;
	 
	 if(k != 0)
	 {
		 for(j=0;j < k; j=j+1)
		 {
		   CvRect rect = cvBoundingRect(contours4[j],0);
		   cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 255), 2, 8, 0);
		   printf("Blue %d : ", j+1);
		   printf("(%d , %d) (%d , %d)\n", rect.x,rect.y,rect.x + rect.width,rect.y + rect.height); 
		 }
     }
*/
	cvReleaseMemStorage(&storage);  
	return frame;
}

int main( )
{
	time_t start,end;
	IplImage *frame, *imgHSV, *imgThreshed;
	
	unsigned char min[] = {160,22,85};
	unsigned char max[] = {179,38,115};
	
	imgHSV = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,3); 
    imgThreshed = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,1);
    
    cvNamedWindow("Original",1);
    cvMoveWindow("Original",100,100);      
    cvNamedWindow("Result",1);
    cvMoveWindow("Result",100,380);
       	
	CvCapture *capture = cvCaptureFromCAM(0);        
    cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,240);
    
    if(!capture){
          printf("Capture failure\n");
          return -1;
    }
    
    time(&start);
    int counter=0;
    
    while(1){
		
		frame = cvQueryFrame(capture);  
        if(!frame) break; 
    
	    cvCvtColor(frame, imgHSV, CV_BGR2HSV);   

//	    imgHSV = BGR2HSV(frame,imgHSV);	// low speed

        imgThreshed = Threshold(imgThreshed,imgHSV,min,max); 
        
        cvShowImage("Result", imgThreshed);    
        				
	    frame = Blob(frame,imgHSV,imgThreshed,min,max);
	    
	    time(&end);
        ++counter;
        double sec=difftime(end,start);
        double fps=counter/sec;
        printf("FPS = %.2f\n\n",fps);
        
	    cvShowImage("Original",frame);
	
        if ( (cvWaitKey(10) & 255) == 27 ) break;
	} 
	
	cvReleaseImage(&imgHSV);
    cvReleaseImage(&imgThreshed);  	
          
	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	    
  	return 0;
}
