
#include <QtCore/QCoreApplication>
#include <stdio.h>
#include <tchar.h>
#include "VideoFrame.h"
#include "EncodeObject.h"
#include <QMutex>

QList<IplImage *> listImage;
QMutex mutex;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	cvNamedWindow("video");
	VideoFrame videoFrameObject;
	//videoFrameObject.StartPreview();

	EncodeObject encodeObject;
	videoFrameObject.start();
	encodeObject.start();

	return a.exec();
}
