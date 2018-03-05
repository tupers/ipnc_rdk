#ifndef _NETSERVER_MSG_DEF_H_
#define _NETSERVER_MSG_DEF_H_
/* MSG_TYPE_MSG1 is reserved for server */
/**
 * @addtogroup FILE_MSG_DRV
* @{*/
#define NETSERVER_SYS_MSG		MSG_TYPE_MSG2 ///< message ID used in system server to communicate with file manager.
#define NETSERVER_BOA_MSG		MSG_TYPE_MSG3 ///< message ID used in BOA to communicate with file manager.
#define NETSERVER_STREAM_MSG		MSG_TYPE_MSG4 ///< message ID used in stream server to communicate with file manager.
#define NETSERVER_MOTION_MSG		MSG_TYPE_MSG5 ///< message ID used by motion detection to communicate with file manager.
/** @} */
#endif
