
SRCDIR += links_common/dup links_common/null links_common/system \
		links_common/merge links_common/nullSrc \
		links_common/ipcOutM3 links_common/ipcInM3 \
		links_common/ipcBitsIn links_common/ipcBitsOut \
		links_common/ipcFramesIn links_common/ipcFramesOut \
		links_common/select links_common/va \
        links_common/mux
		
SRCS_VALINK = vaLink_tsk.c vaLink_alg.c		

SRCS_COMMON += dupLink_tsk.c nullLink_tsk.c system_linkApi.c \
		system_linkApi_local.c system_common.c \
		system_ipc.c system_ipc_listMP.c system_ipc_msgq.c system_ipc_notify.c \
		mergeLink_tsk.c nullSrcLink_tsk.c \
		ipcInM3Link_tsk.c ipcOutM3Link_tsk.c system_tiler.c \
		ipcBitsInLink_tsk.c ipcBitsOutLink_tsk.c \
		ipcFramesInLink_tsk.c ipcFramesOutLink_tsk.c \
		selectLink_tsk.c $(SRCS_VALINK) \
		muxLink_tsk.c
