#include "EncodeObject.h"
#include <QMutex>

extern QList<IplImage*> listImage;
extern QMutex mutex;

EncodeObject::EncodeObject(QObject *parent):QThread(parent)
{

}


EncodeObject::~EncodeObject(void)
{

}

void EncodeObject::run()
{
	EncodeIplimage();
}

void EncodeObject::EncodeIplimage()
{
	 avcodec_register_all();

	filename = "test.h264";
	AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y, got_output;
    FILE *f;
    AVFrame *frame;
    AVPacket pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    printf("Encode video file %s\n", filename);

	AVCodecID codec_id = AV_CODEC_ID_H264;
    /* find the mpeg1 video encoder */
    codec = avcodec_find_encoder((AVCodecID)codec_id);
    if (!codec) 
	{
        fprintf(stderr, "Codec not found\n");
        exit(0);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) 
	{
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 200000;
    /* resolution must be a multiple of two */
    c->width = 640;
    c->height = 480;
    /* frames per second */
	AVRational avtmp = {1,25};//25
    c->time_base = avtmp;//(AVRational){1,25};
    c->gop_size = 5;//每10帧插入一帧关键帧
    c->max_b_frames = 0;
    c->pix_fmt = AV_PIX_FMT_YUV420P;//AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
		av_opt_set(c->priv_data, "tune", "zerolatency", 0);
       // av_opt_set(c->priv_data, "preset", "slow", 0);
	  /********************************************************************************
       * 有两个地方影响libav是不是缓存编码后的视频帧，也就是影响实时性：
       * 1、av_opt_set(c->priv_data, "tune", "zerolatency", 0);这个比较主要。
       * 2、参数中有c->max_b_frames = 1;如果这个帧设为0,就没有B帧了，编码会很快的。
       ********************************************************************************/
    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

	
    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height,
                         c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

	int index=0;
	int frameCount = 0;
    /* encode  video */
    for (i = 0; i > -2; i++) 
	{
		mutex.lock();
		if(listImage.count()<=frameCount){ mutex.unlock();continue;}
		frameCount = 0;
		IplImage *pFrame = listImage[0];
		listImage.pop_front();
		mutex.unlock();
		

		av_init_packet(&pkt);
		pkt.data = NULL;    // packet data will be allocated by the encoder
		pkt.size = 0;

		fflush(stdout);

		
		

		CvScalar pixel;
		for (y = 0; y < c->height; y++) {
			for (x = 0; x < c->width; x++) {
				pixel = cvGet2D(pFrame, y, x);
				int Red = pixel.val[2] ;
				int Green = pixel.val[1];
				int Blue = pixel.val[0];
				int Y = (77 * Red / 256) + (150 * Green / 256) + (29 * Blue / 256) ;
				int Cb = - (44 * Red / 256) - (87 * Green / 256) + (131 * Blue / 256) + 128 ;
				int Cr = (131 * Red / 256) - (110 * Green / 256) - (21 * Blue / 256) + 128; 

				frame->data[0][y * frame->linesize[0] + x] = Y;
				if(x%2==0&&y%2==0)
				{
					frame->data[1][(y/2) * frame->linesize[1] + x/2] = Cb;
					frame->data[2][(y/2) * frame->linesize[2] + x/2] = Cr;
				}
				//frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				//frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
			}
		}
		
		cvReleaseImage(&pFrame);
		//continue;
#if(0)
		
		/* prepare a dummy image */
		/* Y */
		for (y = 0; y < c->height; y++) {
			for (x = 0; x < c->width; x++) {
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}

		/* Cb and Cr */
		for (y = 0; y < c->height/2; y++) {
			for (x = 0; x < c->width/2; x++) {
				frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
			}
		}
#endif

		frame->pts = index;

		/* encode the image */
		ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}

		if (got_output) {
			printf("Write 1frame %d (size=%d)\n", (int)(index++/25), pkt.size);
			fwrite(pkt.data, 1, pkt.size, f);
			av_free_packet(&pkt);
		}
    }//end encode

    /* get the delayed frames */
    for (got_output = 1; got_output; index++) {
        fflush(stdout);

        ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output) {
            printf("Write 2frame %d (size=%d)\n", (int)(index/25), pkt.size);
			//upsocket->SendData(pkt.data,pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_free_packet(&pkt);
        }
    }

    /* add sequence end code to have a real mpeg file */
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
}