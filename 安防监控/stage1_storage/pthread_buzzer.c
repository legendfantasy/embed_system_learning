

#include "data_global.h"


//:A9蜂鸣器控制线程.
void *pthread_buzzer(void *arg)
{
	printf("pthread_buzzer\n");

#if 0	
	1.	open(dev_buzzer,  )
	2.	pthread_cond_wait (cond_buzzer,  );
	3.	获取dev_buzzer_mask(控制标志)
	4.	通过ioctl（）控制buzzer
#endif 
}



