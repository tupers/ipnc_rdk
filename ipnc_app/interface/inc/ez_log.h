#ifndef EZ_LOG_H_
#define EZ_LOG_H_
#include <remote_debug_msg_drv.h>


#define EZ_DEBUG_REMOTE

#ifdef EZ_DEBUG_REMOTE
#define Log_printf(fmt, args...)	Remote_printf(fmt, ##args)
#else
#define Log_printf(fmt, args...)	printf(fmt, ##args)
#endif


#endif
