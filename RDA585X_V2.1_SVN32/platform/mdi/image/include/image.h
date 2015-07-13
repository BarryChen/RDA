////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2012-2012, RDA Technologies, Inc.
//                       All Rights Reserved
//
// This source code is property of RDA. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: image.h
//
// DESCRIPTION:
//   the interface of IMAGE
//
////////////////////////////////////////////////////////////////////////////////


MCI_ERR_T MMC_ReadBitmapPicture(UINT8 *data, char* outbuf,long outbuf_len,
								 short width,short height,
								 short start_x,short start_y,
								 short end_x,short end_y,
								 DEC_UNIT* decUnit, short kind);


