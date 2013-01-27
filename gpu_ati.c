#include <stdio.h>
#include <string.h>

float ati_gpu_max_temperature;

int ati_gpu_init()
{
	ati_gpu_max_temperature = 120; //FIXME: is there a way to get the "alarm" temperature?
	return 1;
}

float ati_gpu_temperature()
{
	float ret = -1, temp = 0;
	int sensor = 0;

	FILE*p = popen("aticonfig --odgt", "r");
	if(!p) return 0.0;
	char buf[128];
	while(fgets(buf, 128, p)) {
		if(2 == sscanf(buf, " Sensor %d: Temperature - %f C", &sensor, &temp))
		{
			ret = temp;
		}
	}

	return ret;
}
