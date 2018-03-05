
SRCDIR += links_m3vpss/camera     links_m3vpss/system \
		  links_m3vpss/dei        links_m3vpss/display \
		  links_m3vpss/nsf        links_m3vpss/swMs \
		  links_m3vpss/grpx       links_m3vpss/mjpeg links_m3vpss/vstab \
		  links_m3vpss/grpx       links_m3vpss/scaler \
		  links_m3vpss/fd         links_m3vpss/alg/face_detect \
		  links_m3vpss/swosd      links_m3vpss/alg/common \
		  links_m3vpss/alg/sw_osd links_m3vpss/alg/sw_osd/fonts \
		  links_m3vpss/vnf        links_m3vpss/capture \
          links_m3vpss/avsync 	  links_m3vpss/sclr \
          links_m3vpss/glbce      links_m3vpss/isp \
          links_m3vpss/wdr		  links_m3vpss/alg/dmva/src	\
          links_m3vpss/RotateFrame

MPSCLR_SRCDIR = links_m3vpss/mpSclr 

SRCS_SWOSD      = ascii_TI_Logo_80_32.c    ascii_TI_Logo_160_64.c ascii_UV_462_12.c       \
                  ascii_UV_770_20.c        ascii_UV_1078_28.c     ascii_UV_1232_30.c      \
                  ascii_UV_1540_40_WFONT.c ascii_Y_462_12.c       ascii_Y_770_20.c        \
                  ascii_Y_1078_28.c        ascii_Y_1232_30.c      ascii_Y_1540_40_WFONT.c \
                  ti_swosd.c 			   ti_imx_swosd.c		

SRCS_m3vpss += cameraLink_tsk.c cameraLink_drv.c \
               captureLink_tsk.c captureLink_drv.c \
			   alg_itk_Link_tsk.c\
			   mjpegLink_tsk.c mjpegLink_common.c mjpegLink_enc.c \
			   deiLink_tsk.c deiLink_drv.c \
			   displayLink_tsk.c displayLink_drv.c \
			   nsfLink_tsk.c nsfLink_drv.c \
			   swMsLink_tsk.c swMsLink_drv.c swMsLink_layouts.c \
			   system_m3vpss.c \
			   system_dctrl_common.c \
			   systemLink_tsk_m3vpss.c \
			   sclrLink_tsk.c sclrLink_drv.c \
			   system_dctrl_modeInfo.c \
			   fdLink_tsk.c \
			   ti_facedetect.c \
			   swosdLink_tsk.c \
			   vnfLink_tsk.c vnfLink_drv.c vnfLink_vnf.c \
			   glbceLink_tsk.c glbceLink_drv.c glbceLink_glbce.c glbceLink_movAvg.c \
			   wdrLink_tsk.c wdrLink_drv.c \
			   ispLink_tsk.c ispLink_drv.c \
			   system_m3vpss_edma.c \
			   $(SRCS_SWOSD) \
			   system_dctrl.c \
			   avsync_m3vpss.c \
			   ti_draw.c 	\
			   RotateFrameLink.c

MPSCLR_SRCS_m3vpss = mpSclrLink_drv.c mpSclrLink_tsk.c 

SRCDIR += $(MPSCLR_SRCDIR)
SRCS_m3vpss += $(MPSCLR_SRCS_m3vpss)


