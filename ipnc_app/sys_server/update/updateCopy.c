#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIRMWARE_C6DSP			"/opt/ipnc/firmware/ipnc_rdk_fw_c6xdsp.xe674"
#define FIRMWARE_VPSSM3			"/opt/ipnc/firmware/ipnc_rdk_fw_m3vpss.xem3"
#define FIRMWARE_VIDEOM3		"/opt/ipnc/firmware/ipnc_rdk_fw_m3video.xem3"
#define EZCAM_EZFTP_PATH		"/opt/ipnc/Ezftp"
#define EZCAM_APP_FILE			"/opt/ipnc/EzApp"
#define EZCAM_MCFW_PATH			"/opt/ipnc/bin/ipnc_rdk_mcfw.out"
#define EZCAM_APP_SERVER		"/opt/ipnc/system_server"

#define UPDATE_FIRMWARE_C6DSP			"/opt/ipnc/update/ipnc_rdk_fw_c6xdsp.xe674"
#define UPDATE_FIRMWARE_VPSSM3			"/opt/ipnc/update/ipnc_rdk_fw_m3vpss.xem3"
#define UPDATE_FIRMWARE_VIDEOM3			"/opt/ipnc/update/ipnc_rdk_fw_m3video.xem3"
#define UPDATE_EZCAM_EZFTP_PATH			"/opt/ipnc/update/Ezftp"
#define UPDATE_EZCAM_APP_FILE			"/opt/ipnc/update/EzApp"
#define UPDATE_EZCAM_MCFW_PATH			"/opt/ipnc/update/ipnc_rdk_mcfw.out"
#define UPDATE_EZCAM_APP_SERVER			"/opt/ipnc/update/system_server"

#define UPDATA_ROOT					"/opt/ipnc/update/"
int main()
{
	int ret;
	char cmd[100] = {0};
	
	ret = access(UPDATA_ROOT, F_OK);
	if(ret!=0)
	{
		perror("access");

		sprintf(cmd,"mkdir -p %s",UPDATA_ROOT);

 		system(cmd);
 		
		return -1;
	}
				
#if 1
	if(access(UPDATE_FIRMWARE_C6DSP, F_OK) == 0)
	{
		sprintf(cmd,"cp "UPDATE_FIRMWARE_C6DSP" "FIRMWARE_C6DSP);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_FIRMWARE_C6DSP);
		system(cmd); 
	}

	if(access(UPDATE_FIRMWARE_VPSSM3, F_OK) == 0)
	{
		sprintf(cmd,"cp "UPDATE_FIRMWARE_VPSSM3" "FIRMWARE_VPSSM3);
		system(cmd); 	

		sprintf(cmd,"rm "UPDATE_FIRMWARE_VPSSM3);
		system(cmd); 
	}

	if(access(UPDATE_FIRMWARE_VIDEOM3, F_OK) == 0)
	{
		sprintf(cmd,"cp "UPDATE_FIRMWARE_VIDEOM3" "FIRMWARE_VIDEOM3);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_FIRMWARE_VIDEOM3);
		system(cmd); 
	}

	if(access(UPDATE_EZCAM_EZFTP_PATH, F_OK) == 0)
	{	
		sprintf(cmd,"cp "UPDATE_EZCAM_EZFTP_PATH" "EZCAM_EZFTP_PATH);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_EZCAM_EZFTP_PATH);
		system(cmd); 
	}

	if(access(UPDATE_EZCAM_APP_FILE, F_OK) == 0)
	{	
		sprintf(cmd,"cp "UPDATE_EZCAM_APP_FILE" "EZCAM_APP_FILE);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_EZCAM_APP_FILE);
		system(cmd); 
	}

	if(access(UPDATE_EZCAM_MCFW_PATH, F_OK) == 0)
	{	
		sprintf(cmd,"cp "UPDATE_EZCAM_MCFW_PATH" "EZCAM_MCFW_PATH);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_EZCAM_MCFW_PATH);
		system(cmd); 
	}	

	if(access(UPDATE_EZCAM_APP_SERVER, F_OK) == 0)
	{	
		sprintf(cmd,"cp "UPDATE_EZCAM_APP_SERVER" "EZCAM_APP_SERVER);
		system(cmd); 

		sprintf(cmd,"rm "UPDATE_EZCAM_APP_SERVER);
		system(cmd); 
	}	
#endif
	return 0;
}
