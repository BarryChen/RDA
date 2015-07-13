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
#define DEFAULT_CALIB_AUDIO_GAIN_MAP {-1, 1, 0, -1, 1, 0, 2, 3, }

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

#define MIC_GAINS_CONFIG0  { 10, 0, 12 }

#define DEFAULT_CALIB_AUDIO_OUT_CONFIG0  { \
		{0, -26, 0, 0, 0, -26, 0, 0},\
		{0, -25, 0, 0, 0, -25, 0, 0},\
		{0, -24, 0, 0, 0, -24, 0, 0},\
		{0, -23, 0, 0, 0, -23, 0, 0},\
		{0, -22, 0, 0, 0, -22, 0, 0},\
		{3, -21, 0, 0, 3, -21, 0, 0},\
		{3, -20, 0, 0, 3, -20, 0, 0},\
		{3, -19, 0, 0, 3, -19, 0, 0},\
		{3, -18, 0, 0, 3, -18, 0, 0},\
		{6, -17, 0, 0, 6, -17, 0, 0},\
		{6, -16, 0, 0, 6, -16, 0, 0},\
		{6, -15, 0, 0, 6, -15, 0, 0},\
		{6, -14, 0, 0, 6, -14, 0, 0},\
		{6, -13, 0, 0, 6, -13, 0, 0},\
		{9, -12, 0, 0, 9, -12, 0, 0},\
		{9, -11, 0, 0, 9, -11, 0, 0},\
		{9, -10, 0, 0, 9, -10, 0, 0},\
		{9, -9, 0, 0, 9, -9, 0, 0},\
		{12, -8, 0, 0, 12, -8, 0, 0},\
		{12, -7, 0, 0, 12, -7, 0, 0},\
		{12, -6, 0, 0, 12, -6, 0, 0},\
		{12, -5, 0, 0, 12, -5, 0, 0},\
		{12, -4, 0, 0, 12, -4, 0, 0},\
		{15, -3, 0, 0, 15, -3, 0, 0},\
		{15, -2, 0, 0, 15, -2, 0, 0},\
		{15, -1, 0, 0, 15, -1, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{18, 1, 0, 0, 18, 1, 0, 0},\
		{18, 2, 0, 0, 18, 2, 0, 0},\
		{18, 3, 0, 0, 18, 3, 0, 0},\
		{18, 4, 0, 0, 18, 4, 0, 0},\
		{21, 5, 0, 0, 21, 5, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG0  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG1  { MIC_GAINS_CONFIG1, DEFAULT_CALIB_AUDIO_OUT_CONFIG1,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG1}

#define MIC_GAINS_CONFIG1  { 10, 0, 12 }

#define DEFAULT_CALIB_AUDIO_OUT_CONFIG1  { \
		{0, -26, 0, 0, 0, -26, 0, 0},\
		{0, -25, 0, 0, 0, -25, 0, 0},\
		{0, -24, 0, 0, 0, -24, 0, 0},\
		{0, -23, 0, 0, 0, -23, 0, 0},\
		{0, -22, 0, 0, 0, -22, 0, 0},\
		{3, -21, 0, 0, 3, -21, 0, 0},\
		{3, -20, 0, 0, 3, -20, 0, 0},\
		{3, -19, 0, 0, 3, -19, 0, 0},\
		{3, -18, 0, 0, 3, -18, 0, 0},\
		{6, -17, 0, 0, 6, -17, 0, 0},\
		{6, -16, 0, 0, 6, -16, 0, 0},\
		{6, -15, 0, 0, 6, -15, 0, 0},\
		{6, -14, 0, 0, 6, -14, 0, 0},\
		{6, -13, 0, 0, 6, -13, 0, 0},\
		{9, -12, 0, 0, 9, -12, 0, 0},\
		{9, -11, 0, 0, 9, -11, 0, 0},\
		{9, -10, 0, 0, 9, -10, 0, 0},\
		{9, -9, 0, 0, 9, -9, 0, 0},\
		{12, -8, 0, 0, 12, -8, 0, 0},\
		{12, -7, 0, 0, 12, -7, 0, 0},\
		{12, -6, 0, 0, 12, -6, 0, 0},\
		{12, -5, 0, 0, 12, -5, 0, 0},\
		{12, -4, 0, 0, 12, -4, 0, 0},\
		{15, -3, 0, 0, 15, -3, 0, 0},\
		{15, -2, 0, 0, 15, -2, 0, 0},\
		{15, -1, 0, 0, 15, -1, 0, 0},\
		{15, 0, 0, 0, 15, 0, 0, 0},\
		{18, 1, 0, 0, 18, 1, 0, 0},\
		{18, 2, 0, 0, 18, 2, 0, 0},\
		{18, 3, 0, 0, 18, 3, 0, 0},\
		{18, 4, 0, 0, 18, 4, 0, 0},\
		{21, 5, 0, 0, 21, 5, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG1  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG2  { MIC_GAINS_CONFIG2, DEFAULT_CALIB_AUDIO_OUT_CONFIG2,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG2}

#define MIC_GAINS_CONFIG2  { 10, 0, 12 }

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
		{21, 0, 0, 0, 21, 0, 0, 0}}

#define DEFAULT_CALIB_AUDIO_SIDE_CONFIG2  { \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, \
		-6, -6, -6, -6, -6, -6, -6, -6, }
#define DEFAULT_CALIB_AUDIO_GAINS_CONFIG3  { MIC_GAINS_CONFIG3, DEFAULT_CALIB_AUDIO_OUT_CONFIG3,  DEFAULT_CALIB_AUDIO_SIDE_CONFIG3}

#define MIC_GAINS_CONFIG3  { 10, 0, 12 }

#define DEFAULT_CALIB_AUDIO_OUT_CONFIG3  { \
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
		{21, 0, 0, 0, 21, 0, 0, 0}}

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
		1, 0, 0, 50, 0, 8, 127, 31}

#define DEFAULT_CALIB_ECHO_CANCEL_CONFIG0  {\
		0, 0, 0, 0, 0, 0, 0}

#define DEFAULT_CALIB_ECHO_CONFIG1  {DEFAULT_CALIB_AUDIO_PARAMS_CONFIG1, DEFAULT_CALIB_ECHO_CANCEL_CONFIG1}

#define DEFAULT_CALIB_AUDIO_PARAMS_CONFIG1  {\
		1, 0, 0, 40, 1, 0, 127, 31}

#define DEFAULT_CALIB_ECHO_CANCEL_CONFIG1  {\
		0, 0, 0, 1, 0, 8, 0}


//SBC Filte paramter 
#define DEFAULT_CALIB_SBC_FILTER_CONFIG  {\
		0, 0, 0x0400}


#endif //_TGT_CALIB_CFG_H_

