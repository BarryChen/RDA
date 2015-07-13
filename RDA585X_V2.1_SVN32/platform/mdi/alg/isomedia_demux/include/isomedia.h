#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _GF_ISOMEDIA_H_
#define _GF_ISOMEDIA_H_


#include "mpeg4_odf.h"

enum
{
	/*reserved, internal use in the lib. Indicates the track complies to MPEG-4 system
	specification, and the usual OD framework tools may be used*/
	GF_ISOM_SUBTYPE_MPEG4		= GF_FOUR_CHAR_INT( 'M', 'P', 'E', 'G' ),
	
	/*reserved, internal use in the lib. Indicates the track is of GF_ISOM_SUBTYPE_MPEG4
	but it is encrypted.*/
	GF_ISOM_SUBTYPE_MPEG4_CRYP	= GF_FOUR_CHAR_INT( 'E', 'N', 'C', 'M' ),

	/*AVC/H264 media type - not listed as an MPEG-4 type, ALTHOUGH this library automatically remaps
	GF_AVCConfig to MPEG-4 ESD*/
	GF_ISOM_SUBTYPE_AVC_H264		= GF_FOUR_CHAR_INT( 'a', 'v', 'c', '1' ),

	/*3GPP(2) extension subtypes*/
	GF_ISOM_SUBTYPE_3GP_H263		= GF_FOUR_CHAR_INT( 's', '2', '6', '3' ),
	GF_ISOM_SUBTYPE_3GP_AMR		= GF_FOUR_CHAR_INT( 's', 'a', 'm', 'r' ),
	GF_ISOM_SUBTYPE_3GP_AMR_WB	= GF_FOUR_CHAR_INT( 's', 'a', 'w', 'b' ),
	GF_ISOM_SUBTYPE_3GP_EVRC		= GF_FOUR_CHAR_INT( 's', 'e', 'v', 'c' ),
	GF_ISOM_SUBTYPE_3GP_QCELP	= GF_FOUR_CHAR_INT( 's', 'q', 'c', 'p' ),
	GF_ISOM_SUBTYPE_3GP_SMV		= GF_FOUR_CHAR_INT( 's', 's', 'm', 'v' )
};

#endif
#endif

