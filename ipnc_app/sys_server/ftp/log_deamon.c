#inclulde "log_deamon.h"

unsigned int gLogDeamonQuit = 0;

static unsigned int getCurHour()
{
	time_t t;
	struct tm *tp;

	t = time(NULL);
	tp = localtime(&t);

	return tp->tm_hour;
}

void exec_cron()
{
	system("/opt/ipnc/logcron.bash");	
}

void *LogDeamonThrMain(void *pPrm)
{
	unsigned int old_hour;
	unsigned int new_hour;
	
	old_hour = getCurHour();

	gLogDeamonQuit = 0;
	
//	exec_cron();
	
	while(gLogDeamonQuit == 0)
	{
		new_hour = getCurHour();
		if((new_hour -old_hour) > DEMON_RUN_INTER)
		{
			exec_cron();
			
			old_hour = new_hour;
		}
	}
}
