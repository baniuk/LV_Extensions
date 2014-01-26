/**
 * \file    LV_Tiff.h
 * \brief	Headers and definitions for LV_Tiff.dll		
 * \author  PB
 * \date    2014/01/22
 */

#ifndef LV_Tiff_h__
#define LV_Tiff_h__

/// Handles warnings pushed by libtiff
EXPORTTESTING void WarnHandler(const char* title, const char* format, va_list params);
/// Handles errors pushed by libtiff
EXPORTTESTING void ErrorHandler(const char* title, const char* format, va_list params);
/// Check Tiff image for selected properties
EXPORTTESTING BYTE checkTIFF(TIFF* tif);

#endif // LV_Tiff_h__
