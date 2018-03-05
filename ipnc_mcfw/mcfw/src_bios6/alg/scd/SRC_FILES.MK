
SRCDIR += alg/scd/src

SRCS_SCD += SCD_TI_ialg.c
SRCS_SCD += SCD_TI_dma.c
SRCS_SCD += SCD_TI_ires.c
SRCS_SCD += SCD_bgModelEWRMeanS16.c
SRCS_SCD += SCD_bgModelEWRVarianceS16.c
SRCS_SCD += SCD_config.c
SRCS_SCD += SCD_calcSobelEdgeMap.c
SRCS_SCD += SCD_detect.c
SRCS_SCD += SCD_dmaMemoryMap.c
SRCS_SCD += SCD_segmentFGBlock.c
SRCS_SCD += SCD_process.c
SRCS_SCD += SCD_modifyLumaBlk.c
SRCS_SCD += VLIB_updateEWRMeanS16_i.c
SRCS_SCD += VLIB_updateEWRVarianceS16_i.c

SRCS_c6xdsp +=  $(SRCS_SCD)
#INCDIR_UTILS = $(dvr_rdk_PATH)/mcfw/src_bios6/links_c6xdsp/utils/
INCDIR += $(SRCDIR)

