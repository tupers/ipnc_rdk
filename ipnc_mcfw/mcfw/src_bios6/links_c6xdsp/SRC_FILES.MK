SDSDIR 	  := links_c6xdsp/imgproc/SDS/src
GVSSDIR   := links_c6xdsp/imgproc/detectRect
SDSSRC	  := pcvMisc.c AdasUtility.c pcvDoubleVectorLib.c pcvBasicStruct.c MVCameraPara.c SDS_Detection.c pcvImageProcessing.c pcvMathFixed32.c pcvColorConversion.c SDS_Export.c pcvMathFloat.c pcvDoubleMatrixLib.c
GVSSSRC   := detectRect.c IMG_Common.c MATH_Common.c IMG_ApproxPoly.c
#GVSSSRC   := detectRect.c IMG_Common.c MATH_Common.c 

SRCDIR += links_c6xdsp/system links_c6xdsp/utils/src \
		links_c6xdsp/alg_link links_c6xdsp/alg_link/swosd links_c6xdsp/alg_link/scd \
		links_c6xdsp/rvm  links_c6xdsp/imgproc  
		
SRCDIR += $(GVSSDIR)

SRCS_UTILS = utils_dsp.c

SRCS_ALGLINK    = algLink_tsk.c algLink_priv.c

SRCS_OSDLINK    = osdLink_alg.c  

SRCS_SCDLINK    = scdLink_alg.c 

SRCS_RVM        = rvmLink_tsk.c

SRC_IMG			= $(GVSSSRC) Alg_drvMgr.c imgLink_alg.c imgLink.c 

#SRC_IMG			=  imgLink_alg.c imgLink.c

SRCS_SYSTEMDSP  = system_c6xdsp.c systemLink_tsk_c6xdsp.c system_c6xdsp_edma.c

SRCS_c6xdsp += $(SRCS_SYSTEMDSP) $(SRCS_UTILS) $(SRCS_ALGLINK) $(SRCS_OSDLINK)  $(SRCS_SCDLINK) $(SRC_RVM) $(SRC_IMG)


