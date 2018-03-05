#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/types.h>
#include <Frame_rate_ctrl.h>
#include <sys_env_type.h>

const unsigned int enc_framerate_day_1[10][4][4] =
{
	{
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{5},
	},
	{
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{60,30,15},
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{60,30,15},
	},
	{
#ifdef IPNC_DM385
		{60,30,15},
#else
		{60,30,15},
		{24},
		{15},
		{12},
#endif
	},
	{
		{50,30,15},
	},
};

const unsigned int enc_framerate_day_2[10][4][4] =
{
	{
		{0,0,0},
	},
	{
		{0,0,0},
	},
	{
		{0},
	},
	{
		{30,15},
	},
	{
		{25,15},
	},
	{
		{30,15},
		{30,15},
	},
	{
		{25,15},
	},
	{
		{30,15},
	},
	{
#ifdef IPNC_DM385
		{30,15},
#else
		{30,15},
		{12},
		{8},
		{6},
#endif
	},
	{
		{25,15},
	},
};
const unsigned int enc_framerate_day_3[10][4][4] =
{
	{
		{0,0,0},
	},
	{
		{0,0,0},
	},
	{
		{0},
	},
	{
		{0,0},
	},
	{
		{0,0},
	},
	{
		{0,0},
		{0,0},
	},
	{
		{0,0},
	},
	{
		{0,0},
	},
	{
#ifdef IPNC_DM385
		{5},
#else
		{5},
		{5},
		{5},
		{5},
#endif
	},
	{
		{5},
	},
};

const unsigned int enc_framerate_night_1[10][4][4] =
{
	{
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{5},
	},
	{
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{60,30,15},
		{60,30,15},
	},
	{
		{50,30,15},
	},
	{
		{60,30,15},
	},
	{
#ifdef IPNC_DM385
		{60,30,15},
#else
		{60,30,15},
		{24},
		{15},
		{12},
#endif
	},
	{
		{50,30,15},
	},
};

const unsigned int enc_framerate_night_2[10][4][4] =
{
	{
		{0,0,0},
	},
	{
		{0,0,0},
	},
	{
		{0},
	},
	{
		{30,15},
	},
	{
		{25,15},
	},
	{
		{30,15},
		{30,15},
	},
	{
		{25,15},
	},
	{
		{30,15},
	},
	{
#ifdef IPNC_DM385
		{30,15},
#else
		{30,15},
		{12},
		{8},
		{6},
#endif
	},
	{
		{25,15},
	},
};
const unsigned int enc_framerate_night_3[10][4][4] =
{
	{
		{0,0,0},
	},
	{
		{0,0,0},
	},
	{
		{0},
	},
	{
		{0,0},
	},
	{
		{0,0},
	},
	{
		{0,0},
		{0,0},
	},
	{
		{0,0},
	},
	{
		{0,0},
	},
	{
#ifdef IPNC_DM385
		{5},
#else
		{5},
		{5},
		{5},
		{5},
#endif
	},
	{
		{5},
	},
};

int frame_rate_control(int nDayNight, int nVideocodecmode, int nVideocodecres, int streamNO, int UI_option)
{
	int value=-1;


	if(streamNO==1)
		value = nDayNight? enc_framerate_day_1[nVideocodecmode][nVideocodecres][UI_option]:enc_framerate_night_1[nVideocodecmode][nVideocodecres][UI_option];
	else if(streamNO==2)
		value = nDayNight? enc_framerate_day_2[nVideocodecmode][nVideocodecres][UI_option]:enc_framerate_night_2[nVideocodecmode][nVideocodecres][UI_option];
	else if(streamNO==3)
		value = nDayNight? enc_framerate_day_3[nVideocodecmode][nVideocodecres][UI_option]:enc_framerate_night_3[nVideocodecmode][nVideocodecres][UI_option];

	return value;
}
