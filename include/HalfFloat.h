/*
 *  HalfFloat.h
 *
 *  Created by Richard Wright on 12/6/10.
 *  Copyright 2010-2023 Starstone Software Systems, Inc. All rights reserved.
 *
 */

// -15 stored using a single precision bias of 127 
const unsigned int HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP = 0x38000000; 

// max exponent value in single precision that will be converted 
// to Inf or Nan when stored as a half-float 
const unsigned int HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP = 0x47800000;
// 255 is the max exponent biased value 
const unsigned int FLOAT_MAX_BIASED_EXP = (0xFF << 23);

const unsigned int HALF_FLOAT_MAX_BIASED_EXP = (0x1F << 10); 
typedef unsigned short	hfloat;

hfloat convertFloatToHFloat(float *f);
float convertHFloatToFloat(hfloat hf);
