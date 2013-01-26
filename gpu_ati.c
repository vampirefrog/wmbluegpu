#include <stdio.h>
#include <string.h>

float ati_gpu_max_temperature;

int ati_gpu_init()
{
	ati_gpu_max_temperature = 120; //FIXME: is there a way to get the "alarm" temperature?
}

float ati_gpu_temperature()
{
	float ret, temp;
	int sensor;

	FILE*p = popen("aticonfig --odgt", "r");
	if(!p) return 0.0;
//	FILE*p = popen("echo -e \"Default Adapter - ATI Radeon HD 3870\\n                  Sensor 0: Temperature - 49.00 C\\n\\n\"", "r");
	char buf[128];
	while(fgets(buf, 128, p)) {
		if(2 == sscanf(buf, " Sensor %d: Temperature - %f C", &sensor, &temp))
		{
			ret = temp;
//			printf("sensor %d  temp %f\n", sensor, temp);
		}
	}
	
	return temp;
}
