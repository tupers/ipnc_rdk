
#ifndef SDS_COMMSTRUCT_H_
#include "SDS_CommStruct.h"
#endif

MV_S_CamPara  g_sCamPara[10] =
{
	//IPhone4(TEMP)
	{
			 { //IntrinsicCamPara
			1694.9f, //iOpticalCenter_X
			1208.4f, //iOpticalCenter_Y
			2875, //iFocalLength_X
			2880, //iFocalLength_Y
			30, //iFrameRate
			720, //iImageWidth
			1280, //iImageHeight
			},

		{   //ExtrinsicCamPara
			-0.5f,    //fPitch
				0.0f,    //fRoll
				0.0f,    //fYaw
				1.41f,  //fCameraHeight
				0,    //Set ROI params X
				0,  //Set ROI params Y
				720,  //Set ROI params Width
				1280,   //Set ROI params Height
				0.0f,    //fLateralOffset
				1.7f,    //fVehicleWidth
		},

		{   //Set down sample params
			40,    //XOffset
			0,     //YOffset
			1.0f,     //XZoomRate
			1.0f,     //YZoomRate
		},

		{   //Set color params
			//Road sign color range
			{
				200,     //minimal hue [blue road sign]
				225,     //maximal hue [blue road sign]
				60,     //minimal saturation [blue road sign]
				240,     //maximal saturation [blue road sign]
			},

			{
				140,     //minimal hue [green road sign]
				190,     //maximal hue [green road sign]
				70,     //minimal saturation [green road sign]
				240,     //maximal saturation [green road sign]
			},

			51,		//minimal saturation of road sign detection
		},

		{	//Set control params
			0.225	//iNightExposure
		}

	},

		//COMMON_PARA_1
	{
		{   //ExtrinsicCamPara
			359.5f,  //iOpticalCenter_X
			239.5f,  //iOpticalCenter_Y
			571,    //iFocalLength_X
			571,    //iFocalLength_Y
			30,     //iFrameRate
			1111,   //iImageWidth
			851,   //iImageHeight
		},

		{   //ExtrinsicCamPara
			-2.0f,    //fPitch
			0.0f,    //fRoll
			0.0f,    //fYaw
			1.41f,  //fCameraHeight
			//50,    //Set ROI params X
			//150,  //Set ROI params Y
			//550,  //Set ROI params Width
			//300,   //Set ROI params Height
			0,    //Set ROI params X
			0,  //Set ROI params Y
			1111,  //Set ROI params Width
			851,   //Set ROI params Height
			0.0f,    //fLateralOffset
			1.7f,    //fVehicleWidth

		},

		{   //Set down sample params
			40,    //XOffset
			0,     //YOffset
			1.0f,     //XZoomRate
			1.0f,     //YZoomRate
		},

		{   //Set color params
			//Road sign color range
			{
				225,     //minimal hue [blue road sign]
				240,     //maximal hue [blue road sign]
				80,     //minimal saturation [blue road sign]
				240,     //maximal saturation [blue road sign]
			},

			{
				165,     //minimal hue [green road sign]
				215,     //maximal hue [green road sign]
				50,     //minimal saturation [green road sign]
				240,     //maximal saturation [green road sign]
			},

			51,		//minimal saturation of road sign detection
		},

		{	//Set control params
			0.225	//iNightExposure
		}

	},

		//CAM_SONY_LT18I_PARA
	{
		{   //IntrinsicCamPara
			1694.9f,  //iOpticalCenter_X
			1208.4f,  //iOpticalCenter_Y
			2875,    //iFocalLength_X
			2880,    //iFocalLength_Y
			30,     //iFrameRate
			1111,   //iImageWidth
			851,   //iImageHeight
		},

		{   //ExtrinsicCamPara
			-0.5f,    //fPitch
			0.0f,    //fRoll
			0.0f,    //fYaw
			1.35f,  //fCameraHeight
			0,    //Set ROI params X
			0,  //Set ROI params Y
			1111,  //Set ROI params Width
			851,   //Set ROI params Height
			0.0f,    //fLateralOffset
			1.7f,    //fVehicleWidth

		},

		{   //Set down sample params
			0,    //XOffset
			0,     //YOffset
			0.19607f,     //XZoomRate
			0.19607f,     //YZoomRate
		},

		{   //Set color params
			//Road sign color range
			{
				200,     //minimal hue [blue road sign]
				225,     //maximal hue [blue road sign]
				60,     //minimal saturation [blue road sign]
				240,     //maximal saturation [blue road sign]
			},

			{
				140,     //minimal hue [green road sign]
				190,     //maximal hue [green road sign]
				70,     //minimal saturation [green road sign]
				240,     //maximal saturation [green road sign]
			},

			51,		//minimal saturation of road sign detection
		},

		{	//Set control params
			0.175	//iNightExposure
		}

	},

};



