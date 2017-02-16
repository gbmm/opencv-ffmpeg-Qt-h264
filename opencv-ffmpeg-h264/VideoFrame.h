#pragma once

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <QThread>
#include <QList>
#include <QMutex>

class VideoFrame:public QThread
{
	Q_OBJECT
public:
	VideoFrame(QObject*parent=0);
	~VideoFrame(void);
	void run();
	void StartPreview();
};

