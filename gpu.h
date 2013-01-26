#ifndef __GPU_H__
#define __GPU_H__

int ati_gpu_init(void);
float ati_gpu_temperature(void);
float ati_gpu_max_temperature;

int nvidia_gpu_init(Display *dpy, int screen);
float nvidia_gpu_temperature(Display *dpy, int screen);
extern float nvidia_gpu_max_temperature;

#endif /* __GPU_H__ */
