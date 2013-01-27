#include <stdio.h>

#include <X11/Xlib.h>

#include "NVCtrl.h"
#include "NVCtrlLib.h"

float nvidia_gpu_max_temperature;

// this function does some checks
int nvidia_gpu_init(Display * dpy, int screen) {

	int event_base, error_base;

    int ret = XNVCTRLQueryExtension(dpy, &event_base, &error_base);
    if (ret != True) {
        fprintf(stderr, "The NV-CONTROL X extension does not exist on '%s'.\n",
                XDisplayName(NULL));
        return 0;
    }

	if(!XNVCTRLIsNvScreen(dpy, screen)) {
		fprintf(stderr, "Screen %d does not support the NV-Control extension\n", screen);
		return 0;
	}

	int max;
	ret = XNVCTRLQueryAttribute(dpy, screen, 0, NV_CTRL_GPU_CORE_THRESHOLD, &max);
	if(ret) {
		nvidia_gpu_max_temperature = max;
	}
	
	return 1;
}

float nvidia_gpu_temperature(Display *dpy, int screen) {
	int core;
	int ret = XNVCTRLQueryAttribute(dpy, screen, 0, NV_CTRL_GPU_CORE_TEMPERATURE, &core);

	if(ret) {
		return core;
	}
	
	return 0.0;
}
