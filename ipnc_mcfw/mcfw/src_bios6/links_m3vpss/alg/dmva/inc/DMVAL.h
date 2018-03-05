/*
 *******************************************************************************
 *
 * Digital Media Video Analytics Library
 *
 * "Digital Media Video Analytics Library" is software module developed for TI's
 * DSP and DMVAx processor to demonstrate key video anlytics algorithms. 
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *******************************************************************************
*/
  
/**
  ******************************************************************************
  *  @file     DMVAL.h
  *
  *  @brief    Digital Media Video Analytics Library Interface
  *
  *  @author   Moore, Darnell
  *            Sharma, Vinay 
  *
  *  @version  0.1 - : Initial Version [Darnell, Vinay]
  *
  *  @version  0.2 - August 08,2012 [Deepak]
  *                  A. Added control to tune the event detection of overlapped 
  *                     events
  *                  B. Added control to get focus information from user
  *  @version 0.3  - September 09, 2012 [Deepak + Pramod]
  *                  Taking care of various review comments provided by team. like
  *                  A. converting all the enums into Int32
  *                  B. downsizing the DMVALhandle structure
  *                  C. Adding a new API DMVAL_getMemReq
  *                  D. Detletion of the API DMVAL_delete
  *                  E. Removing the interface parameter which is not supprted
  *                     at present.
  *                  F. Introduction of new API DMVAL DMVAL_resetModule
  *  @version 0.4  - November 29, 2012 [Kumar]
  *                  Added EDMA3.0 channel ID and Queue ID to create time params
  *
  *****************************************************************************
  */

#ifndef _DMVAL_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DMVAL_H_

#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>

/**
 Maximum number of object tracked in any frame processing
*/
#define DMVAL_MAX_OBJECTS    64
/**
 Maximum number of points in a polygon definition
*/
#define DMVAL_MAX_POLY_PTS    16
/**
 Maximum number of ROI in IMD
*/
#define DMVAL_MAX_IMD_ROI    4
/** 
  Defines maximum number of mem tabs asked in DMVAL_create call
*/
#define DMVAL_MAX_MEM_TABS    (8)

/**
 Maximum number of polygon
*/
#define DMVAL_MAX_POLYGONS    DMVAL_MAX_IMD_ROI + 1
/**
 Maximum number of input YUV buffers locked by DMVAL 
*/
#define DMVAL_MAX_BUFFER_LOCK (4)
/**
 Maximum number of different meta data that can provided 
 for given image frame.
*/


#define DMVAL_MAX_METADATA (4)


/**
 *  @enum       DMVALsensitivity
 *  @brief      Defines various level of enumeration types of sensitivity
 */

typedef enum {
  DMVAL_SENSITIVITY_LEVEL_1    = 1,
  DMVAL_SENSITIVITY_LEVEL_2    = 2,
  DMVAL_SENSITIVITY_LEVEL_3    = 3,
  DMVAL_SENSITIVITY_LEVEL_4    = 4,
  DMVAL_SENSITIVITY_LEVEL_5    = 5,
  DMVAL_SENSITIVITY_LEVEL_6    = 6,
  DMVAL_SENSITIVITY_LEVEL_7    = 7,
  DMVAL_SENSITIVITY_LEVEL_8    = 8,
} DMVALsensitivity;

/**
 *  @enum       DMVALstatus
 *  @brief      Defines status of DMVAL API calling
 */

typedef enum
{
  DMVAL_NO_ERROR = 0,
                 /**< No Error has happened in DMVAL API calling
                  */
  DMVAL_ERR_INSTANCE_CREATE_FAILED,
                 /**< Indicates DMVAL instance creation has failed.
                  */
  DMVAL_ERR_INPUT_INVALID,
                 /**< Indicates invalid input has been provided
                  */
  DMVAL_ERR_INPUT_INVALID_FRAME,
                 /**< Indicates invalid inforamtion about input frame
                      has been provided through DMVAL API
                  */
  DMVAL_ERR_INPUT_INVALID_TIMESTAMP,
                 /**< Indicates invalid information about time stamp 
                      of input frame has been provided
                  */
  DMVAL_ERR_INPUT_NEGATIVE,
                 /**< Indicates negative value recieved for a parameter
                      which was expected to be positive
                  */
  DMVAL_ERR_INPUT_EXCEEDED_RANGE,
                 /**< Value of a input parameter recieved is greater than 
                      expected allowed range
                  */
  DMVAL_ERR_MEMORY_INSUFFICIENT,
                 /**< Memory insufficiency has occured inside algorithm
                  */
  DMVAL_ERR_MEMORY_POINTER_NULL,
                 /**< Null memory pointer access has been encountered inside
                      algorithm
                  */
  DMVAL_WARNING_INITIALIZING,
                 /**< In current process call DMVAL is getting
                      initialized. When this warning is thrown then 
                      DMVAL output is not reliable, basically in this
                      duration DMVAL is just getting initialized
                  */
  DMVAL_WARNING_PARAMETER_UNDERSPECIFIED,
                 /**< Some key parameters are not specified or 
                      partially specified.
                  */
  DMVAL_WARNING_DISABLED
                 /**< This warning indicates that DMVAL process 
                      has been called without initalizing it.
                   */

} DMVALstatus;

/**
 *  @enum       DMVALdetectorResult
 *  @brief      Defines various enumerated types 
 *              for events detected by DMVAL algorithm
 */
typedef enum
{
  DMVAL_DETECTOR_NONE      = 0,
  DMVAL_DETECTOR_TAMPER    = 32,
  DMVAL_DETECTOR_IMD       = 64,
  DMVAL_DETECTOR_TRIPZONE  = 128,
  DMVAL_DETECTOR_COUNTER   = 256,
  DMVAL_DETECTOR_SMETA     = 512,
  DMVAL_DISABLED           = 1024,
  DMVAL_INITIALIZING       = 2048
} DMVALdetectorResult;

/**
 *  @enum       DMVALtamperType
 *  @brief      Defines various enumerated types 
 *              for tampering result 
 */

typedef enum
{
  DMVAL_TAMPER_NONE       = 0,
                 /**< No camera tampering has been detected
                  */
  DMVAL_TAMPER_SCENECHANGE= 1,
                 /**< Scene cahnge for camera view has been 
                      detected
                  */
  DMVAL_TAMPER_BRIGHT     = 2,
                 /**< Camera brightning has been detected
                  */
  DMVAL_TAMPER_DARK       = 4,
                 /**< Camera darkening has been detected
                   */
  DMVAL_TAMPER_MOVEMENT   = 8,
                 /**< Camera movement is detected
                  */
  DMVAL_TAMPER_BLUR       = 16
                /**< Blurring of the camera has been detected
                 */
} DMVALtamperType;

/**
 *  @enum       DMVALimgType
 *  @brief      Defines various enumerated types 
 *              for different format for image data 
 */
typedef enum
{
  DMVAL_IMG_LUMA                      = 0,  
  DMVAL_IMG_YUV422_INTERLEAVED        = 1,
  DMVAL_IMG_YUV420_PLANARINTERLEAVED  = 2,
  DMVAL_IMG_RGB_PLANAR                = 3,
  DMVAL_IMG_RGB_INTERLEAVED           = 4,
  DMVAL_IMG_BGR_PLANAR                = 5,
  DMVAL_IMG_BGR_INTERLEAVED           = 6
} DMVALimgType;

/**
 *  @enum       DMVALdepth
 *  @brief      Defines various enumerated types 
 *              for different depth of each pixel for
                imput image data
 */
typedef enum {
  DMVAL_PIXEL_U01 = 1,
  DMVAL_PIXEL_S01,
  DMVAL_PIXEL_U08,
  DMVAL_PIXEL_S08,
  DMVAL_PIXEL_U16,
  DMVAL_PIXEL_S16,
  DMVAL_PIXEL_U32,
  DMVAL_PIXEL_S32
} DMVALdepth;

/**
 *  @enum       DMVALdetectMode
 *  @brief      Defines various enumerated types 
 *              for different types of event detected
                by DMVAL.
 */

typedef enum
{
  DMVAL_DETECTMODE_DISABLE   = 0,
  DMVAL_DETECTMODE_TAMPER    = 1,
  DMVAL_DETECTMODE_IMD       = 2,
  DMVAL_DETECTMODE_SMETA     = 4,
  DMVAL_DETECTMODE_TRIP      = 8,
  DMVAL_DETECTMODE_COUNT     = 16
} DMVALdetectMode;

/**
 *  @enum       DMVALdirection
 *  @brief      Defines various enumerated types 
 *              for different types of object movements
 */
typedef enum
{
  DMVAL_DIRECTION_UNSPECIFIED  = 0,
  DMVAL_DIRECTION_ZONEA2B      = 1,
  DMVAL_DIRECTION_ZONEB2A      = 2
} DMVALdirection;
/**
 *  @enum       DMVALparameter
 *  @brief      Defines various enumerated types 
 *              for different parameter setting possible
 *              from user. these enums are used in 
 *              DMVAL_setparams API to communicate specifc 
 *              parameter which is required to be set in current
 *              call.
 */
typedef enum
{
  DMVAL_PARAM_SENSITIVITY,
                 /**< Sensitivity for algorithms. This parameter is applicable
                      to all algorithm except camera tamper detection. Allowed 
                      range of this parameter is defined in the enum definition
                      @DMVALsensitivity
                  */
  DMVAL_PARAM_INVIEW_PEOPLE, 
                 /**< Used to set to indicate whether people are present in 
                      scene or not.
                   */
  DMVAL_PARAM_INVIEW_VEHICLES,
                 /**< Used to set to indicate whether vehicles are present in 
                      scene or not.
                   */
  DMVAL_PARAM_MINPERSONSIZE,
                   /**< Used to set minimum person size, first field as width
                        and second field as height need to be passed in 
                        DMVAL_setParameter. Values are expected in pixels.
                        
                        Allowed range for this parameter for width is 
                        [16, 0.25*imageWidth] for trip zone algorith.
                        Allowed range for this parameter for height is 
                        [16, 0.33*imageHeight] for trip zone algorith.

                        Allowed range for this parameter for width is 
                        [16, 0.25*imageWidth] for object counting algorith.
                        Allowed range for this parameter for height is 
                        [16, 0.25*imageHeight] for object counting algorith.

                        IMPORTANT ::
                        For Object counting single value of the object size
                        is Supported. So only min size is valid. Also only person
                        counting is supprted, vehicle counting is not 
                        supported.
                   */

  DMVAL_PARAM_MAXPERSONSIZE,
                   /**< Used to set maximum person size, first field as width
                        and second field as height need to be passed in 
                        DMVAL_setParameter. Values are expected in pixels.
                        
                        Allowed range for this parameter for width is 
                        [16, 0.25*imageWidth] for trip zone algorith.
                        Allowed range for this parameter for height is 
                        [16, 0.33*imageHeight] for trip zone algorith.

                        Allowed range for this parameter for width is 
                        [16, 0.25*imageWidth] for object counting algorith.
                        Allowed range for this parameter for height is 
                        [16, 0.25*imageHeight] for object counting algorith.

                        IMPORTANT ::
                        For Object counting single value of the object size
                        is Supported. So only min size is valid. Also only person
                        counting is supprted, vehicle counting is not 
                        supported. This parameter for Object counting algorithm is
                        ignored

                   */
  DMVAL_PARAM_MINVEHICLESIZE,
                   /**< Used to set minimum vehicle size, first field as width
                        and second field as height need to be passed in 
                        DMVAL_setParameter. Values are expected in pixels.

                        allowed range for this parameter is [6, 3/4 * ImageWidth].
                        vehicle width, and [12, 85*ImageHieght>>7] for vehicle 
                        height.
                   */
  DMVAL_PARAM_MAXVEHICLESIZE,
                   /**< Used to set maximum vehicle size, first field as width
                        and second field as height need to be passed in 
                        DMVAL_setParameter. Values are expected in pixels.

                        allowed range for this parameter is [6, 3/4 * ImageWidth].
                        vehicle width, and [12, 85*ImageHieght>>7] for vehicle 
                        height. Same time it shoule be larger than 
                        DMVAL_PARAM_MINVEHICLESIZE

                   */
  DMVAL_PARAM_TZDIRECTION,
                  /**< Direction in trip zone algorithm which is required 
                       to be detected. Allowed range for this is per the
                       enum @DMVALdirection
                  */
  DMVAL_PARAM_OCDIRECTION,
                  /**< Direction in object counting algorithm which is required 
                       to be detected. Allowed range for this is per the
                       enum @DMVALdirection
                  */
  DMVAL_PARAM_OCORIENTATION,
                 /**< Event detected when objects move across top and bottom 
                      halves of the image or event detected when objects move 
                      across left and right halves of the image. Allowed range 
                      for this parameter is [0,1]. where 0 indicates top to
                      bottom, and 1 indicates left to right.
                 */
  DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO,
                /**< Used to control the ovelapped area threshold for merging 
                     or not merging of two overlapped objects. Allowed range 
                     for this parameter is as from [0, 100]. So if the % of 
                     ovelapped area of two detected object is larger than the 
                     this parameter than these tow overlapped objects are
                     probable candiadte to get merged into one object while
                     reporting to application.
                  */
  DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL,
                /**< This parameter is used in conjugation with the 
                     parameter "DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO". 
                     if % of ovelapped area of the detected object is larger
                     than DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO, and the 
                     time interval between these tow detected object is lesser
                     than this parameter than these two objects will be merged 
                     into a single object while reporting to application.

                     This parameter is expected in miliseconds, and allowed 
                     value for this is any non positive number.

                  */
  DMVAL_PARAM_DETECTIONRATE,
                /**< Controls how frequently object detection are done 
                     by DMVAL. For frame in which object detection is not
                     done there objects are tracked based on previous
                     history of detection.

                     Allowed range for this parameter is [1, 30]. Recomended
                     value for this parameter is '8'.
                 */
  DMVAL_PARAM_TAMPER_BCK_RESET_INTERVAL,
                 /**< This parameter is accepted in terms of seconds and it 
                       is used to control the duration after which camera 
                       tamper state is reseted and stops reporting the 
                       tamper event even is camera is blocked/moved/shaking/
                       defocused etc.. Recomended and default value for this 
                       parameter is 20 seconds. However any positive number 
                       is accepted from interface.
                   */
  DMVAL_PARAM_TAMPER_BLOCKUP_TH,
                /**< used to control the rate at which transition from
                    non blocked state of tamper to blocked state of 
                    camera tamper.

                    Allowed for this is [0,32]. Recomended and default 
                    value for this is 16. This is an optional parameter , 
                    if user need any specific control then only this 
                    parameter is required to be set.
                   */
  DMVAL_PARAM_TAMPER_BLOCKDOWN_TH,
                /**< used to control the rate at which transition from
                     blocked state of tamper to non blocked state of 
                    camera tamper.

                    Allowed for this is [0,32]. Recomended and default 
                    value for this is 16. This is an optional parameter , 
                    if user need any specific control then only this 
                    parameter is required to be set.

                   */
  DMVAL_PARAM_TAMPER_GAP_AFTER_CAMERA_MOTION,
               /**< used to control the frame gaps after motion 
                    detection to select bacground image in camera 
                    tamper algorithm.

                    This parameter is expected in seconds. This also
                    is optional parameter to be set from user. Default
                    and recommended value for this is 1 sec. 
                 */
  DMVAL_PARAM_TAMPER_BACKGROUND_INTERVAL,
               /**< used to control how frequently back ground images
                    are updated in tamper algorithm. this value is in
                    number of frames. 

                    This parameter is expected in seconds. This also
                    is optional parameter to be set from user. Default
                    and recommended value for this is 10 sec. 

                 */
  DMVAL_PARAM_TAMPERSENSITIVITY,
               /**< Used to control the sensitivity for camera tamper
                    algorithm. Higher the value of this parameter means 
                    higher number of event detected. Lower this value 
                    means lesser events detected.

                    Allowed range for this parameter is as per the 
                    enum @DMVAL_sensitivity
                 */
  DMVAL_PARAM_DBG_VERBOSE_LEVEL,
               /**< Used to control verbose level.
                    
                    Allowed range for this is [0,3]
                 */
  DMVAL_PARAM_IGNOREZONE,
               /**< Used to control bcground image ignore mask
                    in IMD and SMETA algorithm.

                    This is an optional parameter.
               */
  DMVAL_PARAM_SMD_TRACKER
               /**< Used to control to enable SMD tracker.
                    
                    This is an optional parameter for user.

                    Allowed range for this [0,1].
                */
} DMVALparameter;
/**
 *  @enum       DMVALroiType
 *  @brief      Defines various enumerated types 
 *              for different kind of ROI.
 */
typedef enum
{
  DMVAL_NO_ROI   = 0,
  DMVAL_IMD      = 1,
  DMVAL_TZ_1A    = 2,
  DMVAL_TZ_1B    = 4,
  DMVAL_OC_1A    = 8,
  DMVAL_OC_1B    = 16,
  DMVAL_META     = 32
} DMVALroiType;

/**
 *  @enum       DMVALfocusValueValidity
 *  @brief      Defines various enumerated types 
 *              to indicate how focus value 
 *              is given by application to DMVAL. these are  
 *              possible values for the field 
 *              @DMVALimage::focusValue. Any other value
 *              outside the below enumerated values for 
 *              the field @DMVALimage::focusValue is assumed
 *              as the direct value for focus for current image. 
 */

typedef enum
{
 DMVAL_FOCUS_VALUE_NOT_AVAILABLE = -1,
                 /**< No focus value is available to application.
                      DMVAL itself has to calculate focus from
                      the image data.
                  */

 DMVAL_FOCUS_VALUE_USE_METADATA = 0
                 /**< focus value has to be given through 
                      @DMVALImage::metaData[DMVAL_METADATA_FOCUS]
                      in predfined format.
                  */

}DMVALfocusValueValidity;
/**
 *  @enum       DMVALmetaDataType
 *  @brief      Defines various meta data types. these values are used 
 *              to index @DMVALImage::metaData
 */
typedef enum{
  DMVAL_METADATA_HORIZONTAL_FOCUS = 0
                 /**< this meta data is in use when 
                      @DMVALImage::focusValue = @DMVALfocusValueValidity::
                      DMVAL_FOCUS_VALUE_USE_METADATA. Format for this meta data
                      is not defined now.
                  */

}DMVALmetaDataType;


/**
  @ struct dmvalTimeZone
  @ brief  The structure elements specifies information 
           about the local time zone. 
  @ param tz_minuteswest : This is the number of minutes west of UTC
  @ param tz_dsttime     : If nonzero, Daylight Saving Time applies 
                           during some part of the year
  */
typedef struct 
{
  XDAS_Int32  tz_minuteswest;  
  XDAS_Int32  tz_dsttime;    
}dmvalTimeZone;

/**
  @ struct dmvalTimeValue
  @ brief  This structure defines clock information of time stamp
  @ param tv_sec  : Seconds portion of clock. 
  @ param tv_usec : Micro seconds portion clock

  */
typedef struct 
{
  XDAS_UInt32        tv_sec;         /* seconds */
  XDAS_UInt32        tv_usec;        /* and microseconds */
}dmvalTimeValue;


/**
  @ struct HPTimeStamp
  @ brief  This structure defines a high hrecision time stamp
  @ param clock    : clock information of a time stamp 
  @ param timeZone : zone of time stamp
  */

typedef struct  
{
  dmvalTimeValue  clock; 
  dmvalTimeZone   timeZone;
} HPTimeStamp;  

/**
  @struct DMVALguid
  @brief  This structure contains all the parameters which uniquely
          identify a a detected object. Basically it is signature 
          of detected object.
*/
typedef struct
{
  XDAS_Int32  data1; 
                /**< Micro second part of the time stamp
                     of the image in which this obejct was 
                     detected. 
                 */
  XDAS_UInt16 data2;
               /**< Object id of the detected object
                 */
  XDAS_UInt16 data3;
               /**< tz_minuteswest part of time stamp of the 
                    frame in which this object was detected.
                 */
  XDAS_UInt32 data4;
                /**< Second part of the time stamp
                     of the image in which this obejct was 
                     detected. 
                 */

} DMVALguid;
/**
  
  @struct DMVALpoint
  @brief  This structure defines a point interms of
          X & Y cordinate.
*/
typedef struct
{
  XDAS_Int32 x;
  XDAS_Int32 y;
} DMVALpoint;
/**
  @struct DMVALvector2D
  @brief  This structure defines a vector interms of
          X & Y components.
*/
typedef struct
{
  XDAS_Int16 x;
  XDAS_Int16 y;
} DMVALvector2D;
/**
  @struct DMVALbox
  @brief  This structure various parameter to 
          define a rectangular box.
*/
typedef struct
{
  XDAS_Int16  ymin;
  XDAS_Int16  xmin;
  XDAS_Int16  width;
                 /**< Width of the box
                   */
  XDAS_Int16  height;
                 /**< Height of the box
                  */
  XDAS_Int32  boxArea;
                 /**< Area of the rectangular box containing 
                      the object.
                   */
  XDAS_Int32  objArea;
                 /**< Area of acctual object.
                   */

} DMVALbox;

/**
  @struct DMVALpolygon
  @brief  This structure various parameter to 
          define a polygon.
  @param  numPoints : number of points in polygon. There can be 
                      maximum of 16 points per polygon.
  @param  valid     : Indiactes current polygon is active or not.
  @param  polygonId : unique identification number for polygon
  @param  pt        : array of x&y cordinates in polygon
  @param  type      : specifies the type of polygon. 
                      @DMVALroiType associates the polygon 
                      with a particular video analytics application
  @param  dir       : not used. future use.
*/
typedef struct
{
  XDAS_Int32     size;
  XDAS_Int8      numPoints;
  XDAS_Int32     valid;
  XDAS_Int32     polygonID;
  DMVALpoint     pt[DMVAL_MAX_POLY_PTS + 1];
  XDAS_Int32     type;
  XDAS_Int32     dir;
} DMVALpolygon;


  
/**
  
  @struct DMVALimage
  @brief  This structure contains all the parameters which will define 
          input image data given to DMVAL

  @param  timeStamps : captured time stamp of the image data
  @param  type       : type of image data, defined by enum
                       @DMVALimgType
  @param  pixeldDepath : each pixel length defined by enum
                         @DMVALdepth
  @param  width        : width of image in pixels
  @param  height       : height of image in pixels
  @param  imageStride  : stride of image in pixels. currently
                         not supported
  @param  horzOffset   : horizontal offset of active data.
                         currently not supported.
  @param  vertOffset   : vertical offset of active data.
                         currenly not supprted.  Not supprted currently.
  @param  imageData    : pointer of the memory where image
                         data is present
  @param  inputId      : input id asocciated with current image data.
                         This id is allocated by application and same id is
                         returned by DMVAL if usages of image data is 
                         fnished by DMVAL.
  @param  focusValue   : Measure of focus of image data
  @param  metaData     : array of meta data pointer for current image
 */
typedef struct
{ 
  XDAS_Int32      size;
  HPTimeStamp     timeStamp;
  XDAS_Int16      type;
  XDAS_Int16      pixelDepth;    
  XDAS_Int16      width;        
  XDAS_Int16      height;     
  XDAS_Int16      imageStride;  
  XDAS_Int16      horzOffset;  
  XDAS_Int16      vertOffset;  
  XDAS_Void*      imageData;    
  XDAS_Int16      inputId;
  XDAS_Int32      focusValue;
  Void*           metaData[DMVAL_MAX_METADATA];
} DMVALimage;
/**
  @struct DMVALobjMeta
  @brief  This structure defines various information for 
          detected object.

  @param  area        : area of the detected object
  @param  xsum        : sum of horizontal coordinates of object blob pixels.
  @param  ysum        : sum of vertical coordinates of object blob pixels
  @param  roiID       : identification number for the zone in which the object occurs
  @param  result      : indicates the event triggered by this object
  @param  objID       : unique identification number for this object
  @param  objBox      : bounding box corresponding to object location in the image.
  @param  objCentroid : center (1st order moment) of the object blob center.
  @param  objVelocity : current measured velocity of the object?s centroid
  @param  dir         : direction of object motion.
 */
typedef struct 
{
  XDAS_Int32    area;
  XDAS_Int32    xsum; 
  XDAS_Int32    ysum;
  XDAS_Int8     roiID;
  XDAS_Int32    result;
  DMVALguid     objID;
  DMVALbox      objBox;
  DMVALpoint    objCentroid;
  DMVALvector2D objVelocity;
  XDAS_Int32    dir;
} DMVALobjMeta;
/**
  @struct DMVALout
  @brief  This structure contains all information generated from 
          DMVAL through DMVAL_Process API.

  @param  size            : size of this structure. Will be filled by DMVAL
  @param  modeResult      : specifies what event was detected in current frame
  @param  numObjects      : number of objects detected in current frame. 
                            Populated only when a valid event is detected
  @param  objMetadata     : array of structures containing meta-data information 
                            about all detected objects. Populated only when an event 
                            is detected
  @param  Rsvd            : 
  @param  freeBufId       : free Id for input buffer that user can free. This array
                            contains the id of the input frames which can be
                            freed aby application. An element as '0' indiacates end of
                            the array.
  @param  focusValueReset : indicates application to reset the focus statistics 
                            collection
                            

*/

typedef struct
{
  XDAS_Int32      size;
  XDAS_Int32      modeResult;
  XDAS_Int16      numObjects;
  DMVALobjMeta    objMetadata[DMVAL_MAX_OBJECTS];    
  XDAS_Int32      Rsvd;
  XDAS_Int16      freeBufId[DMVAL_MAX_BUFFER_LOCK];
  XDAS_Int8       focusValueReset; 
  XDAS_Int32      Rsvd2;
} DMVALout;
/**
  @struct DMVALparams
  @brief  This structure all the parameters needed to create 
          a DMVAL handle
  @param  size              :  size of the structure @DMVALparams, needs to
                               provided by application and it will be validated
                               by DMVAL algorithm.
  @param  maxDetectMode     :  maximum possible detect modes that can be enabled
                               with current handle. For each detect mode (event)
                               to be enabled, respective bit position need to be
                               set in this argument.
  @param  imagType          :  image data type, refer @DMVALimgType

  @param  maxWidth          :  maximum width of image that is supported with current
                               handle

  @param  edmaBaseAddr      :  EDMA3 base address

  @param  edmaChId          :  EDMA3.0 channel number that will be used by DMVAL

  @param  edmaQId           :  EDMA3.0 queue number that will be used by DMVAL
                                

*/
typedef struct
{
  XDAS_Int32          size;
  XDAS_Int32          detectMode;
  XDAS_Int16          imgType;
  XDAS_Int16          maxWidth;
  XDAS_Int16          maxHeight;
  XDAS_Int32          edmaBaseAddr;
  XDAS_Int16          edmaChId;
  XDAS_Int16          edmaQId;
}DMVALparams;

/**
  @struct DMVALhandle
  @brief  This interface is mainly to communicate the algorithm memory need to 
          application. DMVAL communicates its memory need based on input 
          provides as part of crateParams and populates the handle for memory 
          size, alignment needs
  @param  size           : size of the structure DMVALhandle, need to be passed
                           by application. And DMVAL algorithm will validate it.
  @param  numMem         : number of memory chunks needed for DMVAL creation
  @param  memTab         : memory attributes for each chunk. MemTab[0] is 
                           the persisitant memory required by DMVAL. 
*/
typedef struct {
  XDAS_Int32          size;
  XDAS_Int32          numMem;
  IALG_MemRec         memTab[DMVAL_MAX_MEM_TABS];
}DMVALhandle;

/**
*******************************************************************************
 * @fn         DMVAL_getMemReq
 *
 * @brief      Provides memory need of DMVAL based on the elements of createParas
 *
 * @param[in,out]  handle    : DMVAL handle , maintained by algorithm.
 * @param[in]   createParams : create time parameters
 *
 * @return      status        : status of current API call
 ******************************************************************************
 */
XDAS_Int32 DMVAL_getMemReq(DMVALhandle     *handle,
                           DMVALparams*     createParams
                       );

/**
*******************************************************************************
 * @fn         DMVAL_create
 *
 * @brief      Accepts the memory allocated by application 
               and configures its internal data structures Memory allocation happens 
               as per maxDetectMode set at create params while passing through the API
               DMVAL_getMemReq.
 *
 * @param[in]  handle          : DMVAL handle , maintained by algorithm.
 * @param[in]   createParams   : create time parameters
 *
 * @return      status        : status of current API call
 ******************************************************************************
 */
XDAS_Int32 
DMVAL_create(DMVALhandle  * handle,
               DMVALparams*     createParams);
/**
*******************************************************************************
 * @fn         DMVAL_initModule
 *
 * @brief      Initializes the DMVAL handle as per the current algorithm selected
 *
 * @param[in]  handle      : DMVAL handle , maintained by algorithm.
 * @param[in]  detectMode  : current active algorithm. Change in active 
 *                           algorithm requires this API to be called with 
 *                           new active algorithm as second argument. If two
 *                           algorihms are need to be initialized then bits
 *                           corresponding to each algorithm
 * 
 * @return     status      : status of current API call
 ******************************************************************************
 */
XDAS_Int32
DMVAL_initModule(DMVALhandle * handle,XDAS_Int32 currentDetectMode);

/**
*******************************************************************************
 * @fn         DMVAL_resetModule
 *
 * @brief      Resets the algorithms of current DMVAL handle. which algorithms
 *             to be reseted, that user has to pass as second argument as 
 *             specific bit field enabled for specific algorithm
 *
 * @param[in]  handle      : DMVAL handle , maintained by algorithm.
 * @param[in]  detectMode  :  this parameter communicates the algorithm which 
 *                            user want to reset. By this bit field user can select - 
 *                            one or multiple algorithms to get reset. Refer 
 *                            @DMVALdetectorResult for the details on bit field
 * 
 * @return     status      : status of current API call
 ******************************************************************************
 */
XDAS_Int32
DMVAL_resetModule(DMVALhandle * handle,XDAS_Int32 detectMode);


/**
*******************************************************************************
 * @fn         DMVAL_process
 *
 * @brief      Does the main frame level processing for selected algorithms
 *
 * @param[in] handle       : DMVAL handle , maintained by algorithm.
 * @param[in] input        : information about input image data
 * @param[in,out] outBuf   : memory pointer where DMVAL result will be 
 *                           written out by algorithm. 
 * @return     status      : status of current API call
 ******************************************************************************
 */
XDAS_Int32  
DMVAL_process(DMVALhandle  * handle,
              DMVALimage   * input,
              DMVALout     * outBuf);


/**
*******************************************************************************
 * @fn         DMVAL_setROI
 *
 * @brief      communicates the co ordinates point for polygon to DMVAL
 *
 * @param[in] handle  : DMVAL handle , maintained by algorithm.
 * @param[in] roi     : pointer to polygon information
 * 
 * @return    status  : status of current API call
 ******************************************************************************
 */
XDAS_Int32 
DMVAL_setROI(DMVALhandle* handle, DMVALpolygon* roi);
/**
*******************************************************************************
 * @fn         DMVAL_setParameter
 *
 * @brief      Set a individual user parameter inside DMVAL algorithm
 *
 * @param[in] handle   : DMVAL handle , maintained by algorithm.
 * @param[in] param    : which parameter is needed to set through this API call
 * @param[in] value    : S16* pointer to the values of the parameter
 * @param[in] dim      : valid elements pointed by the pointer 'value'
 *
 * @return    status   : status of current API call
 ******************************************************************************
 */
XDAS_Int32
DMVAL_setParameter(DMVALhandle* handle,
                   XDAS_Int32 param,
                   XDAS_Int16* value,
                   XDAS_Int32 dim);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
