#include <signal.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <sys/time.h>
#include <stdio.h>
#include "screen/screen.h"
#include "v4l2/v4l2.h" 
#include <linux/fb.h>
#include <linux/videodev2.h>

#define DEV_VIDEO      "/dev/video0"
#define FBDEVICE		"/dev/fb0"
#define IMAGEWIDTH      640
#define IMAGEHEIGHT     480

V4L2 v4l2_;
#ifdef SHOW_ON HDMI_NOT_IMSHOW
SCREEN screen_;
unsigned int screen_pos_x,screen_pos_y;
unsigned int * pfb;
draw_box box_tmp{Point(150,150),Point(400,400),0};
#endif
cv::Mat rgb;
bool quit;
 pthread_mutex_t mutex_;
void *v4l2_thread(void *threadarg);
void my_handler(int s);


int main(int argc, char *argv[])
{
#ifdef SHOW_ON HDMI_NOT_IMSHOW
	screen_pos_x = 0;
	screen_pos_y = 0;
	screen_.init((char *)FBDEVICE,IMAGEWIDTH,IMAGEHEIGHT);
    pfb = (unsigned int *)malloc(screen_.finfo.smem_len);
 #endif
    quit = false;
    pthread_mutex_init(&mutex_, NULL);
    struct sigaction sigIntHandler;
   sigIntHandler.sa_handler = my_handler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   sigaction(SIGINT, &sigIntHandler, NULL);

    rgb.create(IMAGEHEIGHT,IMAGEWIDTH,CV_8UC3);

    std::cout<<"open "<<DEV_VIDEO<<std::endl;

    v4l2_.init(DEV_VIDEO,IMAGEWIDTH,IMAGEHEIGHT);
    v4l2_.open_device();
	v4l2_.init_device();
	v4l2_.start_capturing();

	pthread_t threads_v4l2;
	int rc = pthread_create(&threads_v4l2, NULL, v4l2_thread, NULL);

    pthread_join(threads_v4l2,NULL);
    v4l2_.stop_capturing();
    v4l2_.uninit_device();

#ifdef SHOW_ON HDMI_NOT_IMSHOW
    free(pfb);
    pfb=NULL;
    screen_.uninit();
#endif
    return 0;
}

void *v4l2_thread(void *threadarg)
{
#ifdef SHOW_ON HDMI_NOT_IMSHOW
    screen_.v_draw.push_back(box_tmp);
#endif
	while (1)
	{
        //pfb is argb for screnn show and rgb is cv::Mat format enjoy it!
#ifndef SHOW_ON HDMI_NOT_IMSHOW
        //show on opencv windows
        v4l2_.read_frame(rgb);
        imshow("rlt",rgb);
        waitKey(10);
#else
        //show on hdmi
        v4l2_.read_frame_argb(pfb,rgb,screen_.vinfo.xres_virtual,screen_pos_x,screen_pos_y);
        memcpy(screen_.pfb,pfb,screen_.finfo.smem_len);
        sleep(0.01); 
#endif
        if (quit)
            pthread_exit(NULL);
    }
}


void my_handler(int s)
{
            quit = true;
            cout<<"Caught signal "<<s<<" quit="<<quit<<endl;
}
 