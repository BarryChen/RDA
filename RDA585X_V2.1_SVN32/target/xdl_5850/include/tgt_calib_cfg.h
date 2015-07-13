////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// file tgt_calib_cfg.h                                                      //
///   This file include the default calibration data for this target          //
///   The content can be generate by the calibration tool in rdahost.exe      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _TGT_CALIB_CFG_H_
#define _TGT_CALIB_CFG_H_

/// Gain Calibration Config Count
#define CALIB_AUDIO_GAIN_ITF_QTY		4

/// VOC Calibration Config Count
#define CALIB_AUDIO_VOC_ITF_QTY		2

// voc itf map
#define DEFAULT_CALIB_AUDIO_VOC_MAP {-1, 0, 0, -1, 1, 1, -1, -1, }

// gain itf map
#define DEFAULT_CALIB_AUDIO_GAIN_MAP {-1, 0, 0, -1, 1, 0, 2, 3, }//receiver ep sp bt bt_ep bt_sp fm aux

///  Audio params, SDF profile. 
#define DEFAULT_CALIB_SDF_CONFIG0 {{ \
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0001}}\
		

#define DEFAULT_CALIB_SDF_CONFIG1 {{ \
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0400,0x0400,0x0400,0x0400,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0000,\
		0x0000,0x0000,0x0000,0x0001}}\
		

#define DEFAULT_CALIB_SDF_ALL {\
		DEFAULT_CALIB_SDF_CONFIG0,\
		DEFAULT_CALIB_SDF_CONFIG1,\
		}


//Audio gains parameters 
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG0  { MIC_GAINS_CONFIG0, DEFAULT_CALIB_AUDIO_OUT_CONFIG0,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG0}

#define MIC_GAINS_CONFIG0  { 0, 0, 0 }
//tf/tone
#define DEFAULT_CALIB_AUDIO_OUT_CONFIG0  { \
		{0, -26, 0, 0, 0, -26, 0, 0},\
		{0, -25, 0, 0, 0, -25, 0, 0},\
		{0, -24, 0, 0, 0, -24, 0, 0},\
		{0, -23, 0, 0, 0, -23, 0, 0},\
		{0, -22, 0, 0, 0, -22, 0, 0},\
		{3, -21, 0, 0, 0, -21, 0, 0},\
		{3, -20, 0, 0, 0, -20, 0, 0},\
		{3, -19, 0, 0, 0, -19, 0, 0},\
		{3, -18, 0, 0, 3, -21, 0, 0},\
		{6, -17, 0, 0, 3, -20, 0, 0},\
		{6, -16, 0, 0, 3, -19, 0, 0},\
		{6, -15, 0, 0, 3, -18, 0, 0},\
		{6, -14, 0, 0, 3, -17, 0, 0},\
		{6, -13, 0, 0, 3, -16, 0, 0},\
		{9, -12, 0, 0, 3, -15, 0, 0},\
		{9, -11, 0, 0, 3, -14, 0, 0},\
		{9, -10, 0, 0, 3, -13, 0, 0},\
		{9, -9, 0, 0, 3, -12, 0, 0},\
		{12, -8, 0, 0, 3, -11, 0, 0},\
		{12, -7, 0, 0, 3, -10, 0, 0},\
		{12, -6, 0, 0, 6, -11, 0, 0},\
		{12, -5, 0, 0, 6, -10, 0, 0},\
		{12, -4, 0, 0, 6, -9, 0, 0},\
		{15, -3, 0, 0, 6, -8, 0, 0},\
		{15, -2, 0, 0, 6, -7, 0, 0},\
		{15, -1, 0, 0, 6, -6, 0, 0},\
		{15, 0, 0, 0, 6, -5, 0, 0},\
		{18, 1, 0, 0, 6, -4, 0, 0},\
		{18, 2, 0, 0, 6, -3, 0, 0},\
		{18, 3, 0, 0, 6, -2, 0, 0},\
		{18, 4, 0, 0, 6, -1, 0, 0},\
		{21, 5, 0, 0, 6, 0, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG0  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG1  { MIC_GAINS_CONFIG1, DEFAULT_CALIB_AUDIO_OUT_CONFIG1,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG1}

#define MIC_GAINS_CONFIG1  { 0, 0, 0 }
//bt_ep
#define DEFAULT_CALIB_AUDIO_OUT_CONFIG1  { \
		{0, -26, 0, 0, 0, -26, 0, 0},\
		{0, -25, 0, 0, 0, -25, 0, 0},\
		{0, -24, 0, 0, 0, -24, 0, 0},\
		{0, -23, 0, 0, 0, -23, 0, 0},\
		{0, -22, 0, 0, 0, -22, 0, 0},\
		{3, -21, 0, 0, 0, -21, 0, 0},\
		{3, -20, 0, 0, 0, -20, 0, 0},\
		{3, -19, 0, 0, 0, -19, 0, 0},\
		{3, -18, 0, 0, 3, -18, 0, 0},\
		{6, -17, 0, 0, 3, -17, 0, 0},\
		{6, -16, 0, 0, 3, -16, 0, 0},\
		{6, -15, 0, 0, 3, -15, 0, 0},\
		{6, -14, 0, 0, 3, -14, 0, 0},\
		{6, -13, 0, 0, 3, -13, 0, 0},\
		{9, -12, 0, 0, 3, -12, 0, 0},\
		{9, -11, 0, 0, 3, -11, 0, 0},\
		{9, -10, 0, 0, 6, -11, 0, 0},\
		{9, -9, 0, 0, 6, -10, 0, 0},\
		{12, -8, 0, 0, 6, -9, 0, 0},\
		{12, -7, 0, 0, 6, -8, 0, 0},\
		{12, -6, 0, 0, 6, -7, 0, 0},\
		{12, -5, 0, 0, 6, -6, 0, 0},\
		{12, -4, 0, 0, 6, -5, 0, 0},\
		{15, -3, 0, 0, 6, -4, 0, 0},\
		{15, -2, 0, 0, 9, -4, 0, 0},\
		{15, -1, 0, 0, 9, -3, 0, 0},\
		{15, 0, 0, 0, 9, -2, 0, 0},\
		{18, 1, 0, 0, 9, -1, 0, 0},\
		{18, 2, 0, 0, 9, 0, 0, 0},\
		{18, 3, 0, 0, 9, 1, 0, 0},\
		{18, 4, 0, 0, 9, 2, 0, 0},\
		{21, 5, 0, 0, 9, 3, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG1  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG2  { MIC_GAINS_CONFIG2, DEFAULT_CALIB_AUDIO_OUT_CONFIG2,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG2}

#define MIC_GAINS_CONFIG2  { 0, 0, 0 }
//fm
#define DEFAULT_CALIB_AUDIO_OUT_CONFIG2  { \
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{3, -9, 0, 0, 3, -5, 0, 0},\
		{3, -9, 0, 0, 3, -5, 0, 0},\
		{3, -9, 0, 0, 3, -5, 0, 0},\
		{3, 0, 0, 0, 3, 0, 0, 0},\
		{6, 0, 0, 0, 6, 0, 0, 0},\
		{6, 0, 0, 0, 6, 0, 0, 0},\
		{6, 0, 0, 0, 6, 0, 0, 0},\
		{6, 0, 0, 0, 6, 0, 0, 0},\
		{6, 0, 0, 0, 6, 0, 0, 0},\
		{9, 0, 0, 0, 9, 0, 0, 0},\
		{9, 0, 0, 0, 9, 0, 0, 0},\
		{9, 0, 0, 0, 9, 0, 0, 0},\
		{9, 0, 0, 0, 9, 0, 0, 0},\
		{12, 0, 0, 0, 12, 0, 0, 0},\
		{12, 0, 0, 0, 12, 0, 0, 0},\
		{12, 0, 0, 0, 12, 0, 0, 0},\
		{12, 0, 0, 0, 12, 0, 0, 0},\
		{12, 0, 0, 0, 12, 0, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{18, 0, 0, 0, 18, 0, 0, 0},\
		{18, 0, 0, 0, 18, 0, 0, 0},\
		{18, 0, 0, 0, 18, 0, 0, 0},\
		{18, 0, 0, 0, 18, 0, 0, 0},\
		{21, 0, 0, 0, 18, 0, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG2  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG3  { MIC_GAINS_CONFIG3, DEFAULT_CALIB_AUDIO_OUT_CONFIG3,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG3}

#define MIC_GAINS_CONFIG3  { 0, 0, 0 }
//aux
#define DEFAULT_CALIB_AUDIO_OUT_CONFIG3  { \
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 0, -5, 0, 0},\
		{0, -9, 0, 0, 3, -5, 0, 0},\
		{0, -9, 0, 0, 3, -5, 0, 0},\
		{0, -9, 0, 0, 3, -5, 0, 0},\
		{0, 0, 0, 0, 3, 0, 0, 0},\
		{0, 0, 0, 0, 6, 0, 0, 0},\
		{0, 0, 0, 0, 6, 0, 0, 0},\
		{0, 0, 0, 0, 6, 0, 0, 0},\
		{0, 0, 0, 0, 6, 0, 0, 0},\
		{0, 0, 0, 0, 6, 0, 0, 0},\
		{0, 0, 0, 0, 9, 0, 0, 0},\
		{0, 0, 0, 0, 9, 0, 0, 0},\
		{0, 0, 0, 0, 9, 0, 0, 0},\
		{0, 0, 0, 0, 9, 0, 0, 0},\
		{0, 0, 0, 0, 12, 0, 0, 0},\
		{0, 0, 0, 0, 12, 0, 0, 0},\
		{0, 0, 0, 0, 12, 0, 0, 0},\
		{0, 0, 0, 0, 12, 0, 0, 0},\
		{0, 0, 0, 0, 12, 0, 0, 0},\
		{0, 0, 0, 0, 15, 0, 0, 0},\
		{0, 0, 0, 0, 15, 0, 0, 0},\
		{0, 0, 0, 0, 15, 0, 0, 0},\
		{0, 0, 0, 0, 15, 0, 0, 0},\
		{0, 0, 0, 0, 18, 0, 0, 0},\
		{0, 0, 0, 0, 18, 0, 0, 0},\
		{0, 0, 0, 0, 18, 0, 0, 0},\
		{0, 0, 0, 0, 18, 0, 0, 0},\
		{0, 0, 0, 0, 21, 0, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG3  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_GAINS_ALL {\
		DEFAULT_CALIB_AUDIO_GAINS_CONFIG0,\
		DEFAULT_CALIB_AUDIO_GAINS_CONFIG1,\
		DEFAULT_CALIB_AUDIO_GAINS_CONFIG2,\
		DEFAULT_CALIB_AUDIO_GAINS_CONFIG3,\
		}


//Echo Cancel parameters 
#define DEFAULT_CALIB_ECHO_CONFIG0  {DEFAULT_CALIB_AUDIO_PARAMS_CONFIG0, DEFAULT_CALIB_ECHO_CANCEL_CONFIG0}

#define DEFAULT_CALIB_AUDIO_PARAMS_CONFIG0  {\
		1, 0, 0, 127, 0, 8, 127, 28}///0:aec_enable 2:strong echo

#define DEFAULT_CALIB_ECHO_CANCEL_CONFIG0  {\
		0, 0, 0, 0, 0, 0, 0}

#define DEFAULT_CALIB_ECHO_CONFIG1  {DEFAULT_CALIB_AUDIO_PARAMS_CONFIG1, DEFAULT_CALIB_ECHO_CANCEL_CONFIG1}

#define DEFAULT_CALIB_AUDIO_PARAMS_CONFIG1  {\
		1, 0, 0, 127, 1, 0, 127, 28}

#define DEFAULT_CALIB_ECHO_CANCEL_CONFIG1  {\
		0, 0, 0, 1, 0, 8, 0}


//SBC Filte paramter 
#define DEFAULT_CALIB_SBC_FILTER_CONFIG  {\
		0, 0, 0x0400}

//SBC EQ
#define DEFAULT_CALIB_SBC_EQ  {\
                DEFAULT_CALIB_SBC_EQ_MODE_CFG_MAP,\
                {\
                DEFAULT_CALIB_SBC_EQ_CONFIG1,\
                DEFAULT_CALIB_SBC_EQ_CONFIG2\
                }\
                }

#define DEFAULT_CALIB_SBC_EQ_MODE_CFG_MAP  {\
                0,0,-1,-1,-1,-1,-1,-1}    

#define DEFAULT_CALIB_SBC_EQ_CONFIG1  {\
                0,0,0,0,0,0,0,0,0,0}                

#define DEFAULT_CALIB_SBC_EQ_CONFIG2  {\
                0,0,0,0,0,0,0,0,0,0}                         
#endif //_TGT_CALIB_CFG_H_

