#pragma once

#include <QThread>
#include <math.h>
#ifdef __cplusplus
extern "C"{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
}
#endif //__cplusplus
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <QList>

class EncodeObject:public QThread
{
	Q_OBJECT
public:
	EncodeObject(QObject *parent=0);
	~EncodeObject(void);
	void run();
	void EncodeIplimage();
public:
	char *filename;
};

