#include "VideoFrame.h"

extern QList<IplImage*> listImage;
extern QMutex mutex;
VideoFrame::VideoFrame(QObject *parent):QThread(parent)
{

}


VideoFrame::~VideoFrame(void)
{

}

void VideoFrame::run()
{
	StartPreview();
}

void VideoFrame::StartPreview()
{
	 
	 CvCapture* pCapture = cvCreateCameraCapture(-1); 
	 while(true)
	 {
		 IplImage *pFrame=cvQueryFrame( pCapture ); 
		 if(pFrame==NULL)continue;
		 IplImage *image = cvCreateImage(cvGetSize(pFrame),pFrame->depth,pFrame->nChannels);
		 cvCopy(pFrame,image);
		 mutex.lock();
		 listImage.append(image);
		 mutex.unlock();
		 cvShowImage("video",pFrame);
		 cvWaitKey(25);
	 }
}