#ifndef SDS_SERIAL_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SDS_SERIAL_H


#define SDS_BAUDRATE        B115200
#define SDS_RS485_DEV		"/dev/ttyO1"

int Serial_Open(char *dev_name);
int Serial_Close(int fd);
int Serial_Read(int fd, void *pData, int size);
int Serial_Write(int fd, void *pData, int size);
int fd_printf(int fd, char * format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
