/*
 *  HalfFloat.cpp
 *  
 *
 *  Created by Richard Wright on 12/6/10.
 *  Copyright 2010-2023 Starstone Software Systems, Inc. All rights reserved.
 *
 */

#include "HalfFloat.h"


hfloat convertFloatToHFloat(float *f) {
    unsigned int x = *(unsigned int *)f; 
    unsigned int sign = (unsigned short)(x >> 31); 
    unsigned int mantissa; 
    unsigned int exp; 
    hfloat hf;
    
    // get mantissa 
    mantissa = x & ((1 << 23) - 1); 
    
    // get exponent bits 
    exp = x & FLOAT_MAX_BIASED_EXP; 
    if (exp >= HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP) {
        // check if the original single precision float number is a NaN 
        if (mantissa && (exp == FLOAT_MAX_BIASED_EXP)) {
        } else {
            // we have a single precision NaN 
            mantissa = (1 << 23) - 1;
            // 16-bit half-float representation stores number as Inf mantissa = 0;
        } hf = (((hfloat)sign) << 15) | (hfloat)(HALF_FLOAT_MAX_BIASED_EXP) |(hfloat)(mantissa >> 13);
    } // check if exponent is <= -15 
    else if (exp <= HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) {
        exp = (HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) >> 23;
        mantissa >>= (14 + exp);
        
        hf = (((hfloat)sign) << 15) | (hfloat)(mantissa);
    }
    else {
        // store a denorm half-float value or zero exp = (HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP - exp) >> 23; mantissa >>= (14 + exp);
        hf = (((hfloat)sign) << 15) | (hfloat)((exp - HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) >> 13) | (hfloat)(mantissa >> 13);
    } 
    return hf;
}



float convertHFloatToFloat(hfloat hf) {
    unsigned int sign = (unsigned int)(hf >> 15); 
    unsigned int mantissa = (unsigned int)(hf & ((1 << 10) - 1)); 
    unsigned int exp = (unsigned int)(hf & HALF_FLOAT_MAX_BIASED_EXP); 
    unsigned int f;
    
    if (exp == HALF_FLOAT_MAX_BIASED_EXP) {
        // we have a half-float NaN or Inf 
        // half-float NaNs will be converted to a single precision NaN 
        // half-float Infs will be converted to a single precision Inf 
        exp = FLOAT_MAX_BIASED_EXP; 
        if (mantissa)
            mantissa = (1 << 23) - 1; // set all bits to indicate a NaN
    } 
    else if (exp == 0x0) 
    {
        // convert half-float zero/denorm to single precision value 
        if (mantissa) {
            mantissa <<= 1; 
            exp = HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP; 
            // check for leading 1 in denorm mantissa 
            while ((mantissa & (1 << 10)) == 0) 
            {
                // for every leading 0, decrement single precision exponent by 1 
                // and shift half-float mantissa value to the left 
                mantissa <<= 1; 
                exp -= (1 << 23);
            } 
            // clamp the mantissa to 10-bits 
            mantissa &= ((1 << 10) - 1); 
            // shift left to generate single-precision mantissa of 23-bits
            mantissa <<= 13;
        }
    }
    else {
        // shift left to generate single-precision mantisa of 23-bits
        mantissa <<= 13;
        // generte single precision biased exponent value
        exp = (exp << 13) + HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
    }
    
    f = (sign << 31) | exp | mantissa; 
    return *((float *)&f);
}
