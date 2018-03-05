#ifndef onvif_H
#define onvif_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipnc_ver.h>
#include <soapH.h>
#include <soapStub.h>
#include <httpda.h>

#include <sys_env_type.h>
#include <sysctrl.h>
#include <system_default.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <web_translate.h>
#include <signal.h>
#define KEY (1492)
#define NUM_BROADCAST (4)        //number of times hello is broadcasted

/* security */
int silvan_wsse_authenticate(struct soap *);
extern int wsse_ret;
extern char wsse_username[20];


extern int server_udp;
extern int server_hello;
extern char udp_sync;

void midprint_soap(struct soap *soap);
void reboot_ipcam_onvif(void);
struct SOAP_ENV__Fault onvif_send_fault(char *sender,char *first,char *second);
void ByeHandler(int);
int udp_hello(int *);
int udp_probe(int *);

void explodeitem(char *in, char dl, char list[][100]);
char *strparser(char *str);
char *anyparser(char *str);
long epoch_convert_switch(int value, char convert, int time);
long periodtol(char *ptr);
int xsperiodtol(const char *);
unsigned long iptol(char *ip);
int isValidIp4 (char *str);
int isValidHostname (char *str);
int onvif_receiver_fault(struct soap*, char *, char *);
int oget_numberofstream(void);


pthread_t hello_thread;
pthread_t set_ip_thread;
void *set_ip_thr(unsigned long s_addr);
void *hello_thr_scope(void *);
void *reboot_thread();
extern int IsDHCPchange;
extern int IsIPchange;
extern int set_DHCP_value;



// event related
#define MAX_SUBSCRIPTION 10
pthread_t notify_thread_sub[MAX_SUBSCRIPTION];
char notify_thread[MAX_SUBSCRIPTION];
void *notify_thr(int );
void *eventtracker_thr(int );
extern int sub_cnt;
extern int sub_cnt_prev;
extern int eventstatus;


int soap_default_dom_element(struct soap *,struct soap_dom_element *);
const char * wtochar(struct soap * ,const wchar_t * );
int soap_default_notify_msg(struct soap *,struct _tt__Message *);

//For Event Handling
#include <pthread.h>
typedef struct notify_thr_arg{
	int t_cnt;
	time_t t_out_thr;
	char consumer_addr[150];
	//char * topic_str;
	char topic_str[150];
	int status;
	pthread_t thread_id;
}notify_struct __attribute__((packed));	//to prevent structure padding issues

extern notify_struct notify_args[MAX_SUBSCRIPTION];
int function_event(notify_struct*);

typedef struct{
	__u8 alarm_enable;
	__u8 motion_enable;
	__u8 custmotion_enable;
	__u8 custmotion_value;
	__u8 motion_level;
	char motion_block[4];
}motion_param __attribute__((packed));

int motion_conf(motion_param*);

struct enum_values
{
	enum status_boolean *status;
};
typedef struct enum_values enumvalues;

enum status_boolean
{
	status_false = 0,
	status_true = 1
};


#define wsse_user_token_profile (1)

#define SERVICE_DEVICE 0
#define SERVICE_IMAGING 1
#define SERVICE_MEDIA 2
#define SERVICE_ONVIF 4
#define INFO_LENGTH 100
#define SMALL_INFO_LENGTH 20
#define IP_LENGTH 20
#define LARGE_INFO_LENGTH 1024
#define LINK_LOCAL_ADDR "169.254.166.174"
#define MACH_ADDR_LENGTH 6
#define DEFAULT_SCOPE "onvif://www.onvif.org"
#define MAJOR_VER 2
#define MINOR_VER 20
#define HOSTNAMELENGTH 80
#define NUM_LOG_PER_PAGE	20
#define SIZE_ALLOC      100
#define ALL  0
#define ANALYTICS 1
#define DEVICE 2
#define EVENTS 3
#define IMAGING 4
#define MEDIA 5
#define PANTILTZOOM 6
#define TRUE 1
#define FALSE 0
#define NUM_IN_CONEC 0
#define NUM_RELAY_CONEC 0
#define MAJOR 1
#define MINOR 0
#define DISCOVERABLE 0
#define NON_DISCOVERABLE 1
#define HTTP 0
#define HTTPS 1
#define RTSP 2
#define IPV4_NTP 0
#define IPV6_NTP 1
#define DNS_NTP 2
#define DATE_MANUAL 0
#define DATE_NTP 1
#define DEFAULT_VALUE 1
#define DEFAULT_SIZE 1
#define NUM_PROFILE 4
#define ANALYTICS_ENABLE 1
#define PTZ_ENABLE 1
#define METADATA_ENABLE 1
#define EXIST 1
#define NOT_EXIST 0
#define SUCCESS 0
#define DEFAULT_ENCODER_PROF {0,"", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "", 0, 0, 0}
#define DEFAULT_SOURCE_PROF {0,"", 0, "", "", 0, 0, 0, 0}
#define ETH_NAME "eth0"

#define COMMAND_LENGTH 40
#define MID_INFO_LENGTH 40
#define SMALL_INFO_LENGTH 20
#define IP_LENGTH 20
#define INFO_LENGTH 100
#define LARGE_INFO_LENGTH 1024
#define TERMINATE_THREAD 0
#define LINK_LOCAL_ADDR "169.254.166.174"
#define MACH_ADDR_LENGTH 6
#define DEFAULT_SCOPE "onvif://www.onvif.org"
#define ETH_NAME_LOCAL "eth0:avahi"

#define YEAR_IN_SEC (31556926)
#define MONTH_IN_SEC (2629744)
#define DAY_IN_SEC (86400)
#define HOURS_IN_SEC (3600)
#define MIN_IN_SEC (60)
#define MAX_EVENTS (3)
#define KEY_NOTIFY_SEM (1729)
#define MOTION_DETECTED_MASK (1)
#define NO_MOTION_MASK (2)


#endif

