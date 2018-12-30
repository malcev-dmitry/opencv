#include "stdafx.h"
#include <highgui.h>
#include <cv.h>

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
void search(IplImage* image);
const char* cascade_name = "haarcascades/haarcascade_frontalface_alt2.xml";
int main(int argc, char** argv)
{
	CvCapture* capture = 0;
	IplImage *frame, *frame_copy = 0;
	int optlen = strlen("--cascade=");
	const char* input_name;
	input_name = "videos\\you_video.mp4";
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	storage = cvCreateMemStorage(0);
	if(!input_name || (isdigit(input_name[0]) && input_name[1] == '\0'))
		capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0');
	else
		capture = cvCaptureFromAVI(input_name);
	if(capture)
	{
		for(;;)
		{
			if(!cvGrabFrame(capture)) break;
				frame = cvRetrieveFrame(capture);
			if(!frame) break;
				if(!frame_copy)
					frame_copy = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, frame->nChannels);
			if(frame->origin == IPL_ORIGIN_TL)
				cvCopy(frame, frame_copy, 0);
			else
				cvFlip(frame, frame_copy, 0);
			search(frame_copy);
			if(cvWaitKey(10) >= 0) break;
		}
		cvReleaseImage(&frame_copy);
		cvReleaseCapture(&capture);
	}
	cvDestroyWindow("window");
	return(0);
}
void search(IplImage* img)
{
	int i;
	int radius;
	double scale = 1.3;
	IplImage* gray = cvCreateImage(cvSize(img->width,img->height), 8, 1);
	IplImage* small_img = cvCreateImage(cvSize(cvRound(img->width/scale), cvRound(img->height/scale)), 8, 1);
	cvCvtColor(img, gray, CV_BGR2GRAY);
	cvResize(gray, small_img, CV_INTER_LINEAR);
	cvEqualizeHist(small_img, small_img);
	cvClearMemStorage(storage);
	if(cascade)
	{
		double t = (double)cvGetTickCount();
		CvSeq* faces = cvHaarDetectObjects(small_img, cascade, storage,
		1.1, 2, 0, cvSize(70, 70));
		for(i = 0; i < (faces ? faces->total : 0); i++)
		{
			CvRect* r = (CvRect*)cvGetSeqElem(faces, i);
			CvPoint center; CvPoint center2;
			radius = cvRound((r->width + r->height) * 0.25 * scale);
			center.x = cvRound((r->x + r->width * 0.5) * scale) - radius;
			center.y = cvRound((r->y + r->height * 0.5) * scale) - radius;
			center2.x = center.x + 2 * radius;
			center2.y = center.y + 2 * radius;
			cvRectangle(img, center, center2, CV_RGB(0, 255, 0), 2, 0, 0);
		}
		cvShowImage("Window",img);
	}
}