#ifndef onvifface_H
#define onvifface_H

int ocheckusername(struct soap *);
char* oget_ipaddress(void);
int oget_encoding(int streamId);
int oget_bitrate(int streamId);
int oget_framerate(int streamId);
int oget_ratecontrol(int streamId);
int oget_ipratio(int streamId); //Govlength
int oget_resolutionwidth(int streamId);
int oget_resolutionheight(int streamId);
int oget_dhcpstatus(void);
char *oget_timezone(int);
int oset_encoding(int res, int encode);
int oset_bitrate(int);
int oset_framerate(int,int,int);
int oset_ratecontrol(int);
int oset_ipratio(int); //Govlength
int oset_resolution(int,int,int);
int oset_dhcpstatus(int);
int oset_timezone(char *TZ);

int onvif_fault(struct soap *,int flag, char *value1,char *value2, char *reason);  
int checkhostname(char *hostname);

#endif
