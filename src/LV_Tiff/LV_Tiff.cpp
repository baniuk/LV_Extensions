/**
 * \file    LV_Tiff.cpp
 * \brief	Holds Tiff related operations
 * \details Exports the following functions:
 * - Tiff_GetParams - Returns size of the image
 * - Tiff_ReadImage - Loads image into user's buffer
 * \pre libtiff3.dll and other dependencies must be on path
 * \author  PB
 * \date    2014/01/22
 * \warning Must be compiled with /EHa for Debug to run tests
 */

#include "stdafx.h"
#include "LV_Tiff.h"

/** 
 * Reads size of the image and return dimensions to LabView due to memory allocation needs.
 * \param[in] image_name	name and path to the input image
 * \param[out] _nrows	number of rows (height)
 * \param[out] _ncols	number of cols (width)
 * \return operation status
 * \retval error_codes defined in error_codes.h
 * \li OK - no error
 * \li NULL_POINTER - NULL pointer passed to function
 * \li FILE_READ_ERROR - Problem with file reading or interpreting
 * \li OTHER_ERROR - Undefined error
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
 * \see http://www.awaresystems.be/imaging/tiff/astifftagviewer.html to check Tiff tags
 * \see error_codes.h
 * \todo check supported tiffs in every function
*/
extern "C" __declspec(dllexport) BYTE Tiff_GetParams(const char* image_name, UINT16* const _nrows, UINT16* const _ncols)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	int TIFFReturnValue;
	UINT32 ncols, nrows;																							// width and height of tiff image
	tsize_t sizeOfTiff;
	TIFF* tif;										// handler of file
	if(NULL==_nrows || NULL==_ncols)																				// Something wrong on LV side
	{
		PANTHEIOS_TRACE_CRITICAL(PSTR("NULL input pointer"));
		return NULL_POINTER;
	}
	// ---------- Reading tiff ----------
	TIFFSetWarningHandler(WarnHandler);													// redirecting warnings to log
	TIFFSetErrorHandler(ErrorHandler);													// redirecting errors to log
	try
	{
		tif = TIFFOpen(image_name, "r");												// open image
	}
	catch(TIFFException& e)
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Caught exception in TIFFOpen "),e.what());
		return FILE_READ_ERROR;
	}
	PANTHEIOS_TRACE_DEBUG(PSTR("File to open: "), image_name);
	if(NULL==tif)																								// error during opening
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in opening image: "), image_name);
		return FILE_READ_ERROR;
	}
	// ---------- Reading Tiff properties ----------
	TIFFReturnValue = TIFFGetField(tif,TIFFTAG_IMAGEWIDTH, &ncols);												// read width
	if(TIFFReturnValue!=1)																						// error
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		return OTHER_ERROR;
	}
	TIFFReturnValue = TIFFGetField(tif,TIFFTAG_IMAGELENGTH, &nrows);												// read height
	if(TIFFReturnValue!=1)																						// error
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		return OTHER_ERROR;
	}
	*_nrows = nrows;
	*_ncols = ncols;
	// get some additional information
	PANTHEIOS_TRACE_DEBUG(PSTR("Image size [rows;cols]"),  PSTR("["),pantheios::integer(nrows), PSTR(","), pantheios::integer(ncols), PSTR("]"));
	sizeOfTiff = TIFFScanlineSize(tif);
	PANTHEIOS_TRACE_DEBUG(PSTR("TIFFScanlineSize in bytes: "), pantheios::integer(sizeOfTiff));
	tstrip_t ts = TIFFNumberOfStrips(tif);
	PANTHEIOS_TRACE_DEBUG(PSTR("TIFFNumberOfStrips: "), pantheios::integer(ts));
	PANTHEIOS_TRACE_DEBUG(PSTR("TIFFStripSize: "), pantheios::integer(TIFFStripSize(tif)));
	
	// ---------- Cleaning ----------
	TIFFClose(tif);																								// closing image
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
	return OK;
}

/** 
 * Reads Tiff image under following assumpions:
 * \li only one page
 * \li PLANAR_CONFIG tag is PLANARCONFIG_CONTIG (1) - there is only one plane
 * \li 16 bits
 * \li strip config
 * These parameters can be verified using TiffTagViewer
 * \param[in] image_name	name and path to the input image
 * \param[out] _data	pointer to memory block that will hold read image
 * \return operation status
 * \retval error_codes defined in error_codes.h
 *  \li OK - no error
 *  \li NULL_POINTER - NULL pointer passed to function
 *  \li FILE_READ_ERROR - Problem with file reading or interpreting
 *  \li OTHER_ERROR - Undefined error
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
 * \see http://www.awaresystems.be/imaging/tiff/astifftagviewer.html to check Tiff tags
 * \see error_codes.h
 * \warning Assumes that every strip has the same number of bytes. In this case there are 'rows' strips of size 2*cols bytes.
*/
extern "C" __declspec(dllexport) BYTE Tiff_ReadImage(const char* image_name, UINT16* const _data)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	tsize_t sizeOfTiff;
	tstrip_t strip;
	tdata_t buf;
	TIFF* tif;										// handler of file
	if(NULL==_data)																				// Something wrong on LV side
	{
		PANTHEIOS_TRACE_CRITICAL(PSTR("NULL input pointer"));
		return NULL_POINTER;
	}
	// ---------- Reading tiff ----------
	TIFFSetWarningHandler(WarnHandler);													// redirecting warnings to log
	TIFFSetErrorHandler(ErrorHandler);													// redirecting errors to log
	try
	{
		tif = TIFFOpen(image_name, "r");												// open image
	}
	catch(TIFFException& e)	// caught also read/write exceptions
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Caught exception in TIFFOpen "),e.what());
		return FILE_READ_ERROR;
	}																	// open image
	PANTHEIOS_TRACE_DEBUG(PSTR("File to open: "), image_name);
	if(NULL==tif)																								// error during opening
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in opening image: "), image_name);
		TIFFClose(tif);
		return FILE_READ_ERROR;
	}
	// test conditions: notiles, 16bit, 1sample per pixel	
	if(OK == checkTIFF(tif))
	{
		// ---------- Loading tiff (example from doc) ----------
		// Image is loaded into internally allocated memory and then coiped to user memory
		buf = _TIFFmalloc(TIFFStripSize(tif));
		UINT32 index = 0;
		UINT16* p;
		p = (UINT16 *)buf;
		for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
		{
			sizeOfTiff = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
			if(-1==sizeOfTiff)
			{
				PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFReadEncodedStrip"));
				TIFFClose(tif);
				_TIFFfree(buf);
				return OTHER_ERROR;
			}
			for(int i = 0; i < sizeOfTiff; i++)
			{
				_data[index] = *(p + i);
				if(i == (sizeOfTiff / 2) - 1) {index++; break;}
				index++;
			}
		}
		PANTHEIOS_TRACE_DEBUG(PSTR("TIFFReadEncodedStrip put "), pantheios::integer(sizeOfTiff),PSTR(" bytes"));
		_TIFFfree(buf);
	}
	else
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Unsuported Tiff"));
		TIFFClose(tif);
		return FILE_READ_ERROR;
	}
	TIFFClose(tif);
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
	return OK;
}

/** 
 * Writes Tiff image to file
 * \param[in] image_name - name and path to the input image
 * \param[in] _data	- pointer to memory block that will hold read image
 * \param[in] _nclos - number of columns of the image (width)
 * \param[in] _nrows - number of rows of the image (height)
 * \return operation status
 * \retval error_codes defined in error_codes.h
 *  \li OK - no error
 *  \li NULL_POINTER - NULL pointer passed to function
 *  \li FILE_READ_ERROR - Problem with file reading or interpreting (can appear on saving too)
 *  \li OTHER_ERROR - Undefined error
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
 * \see http://www.awaresystems.be/imaging/tiff/astifftagviewer.html to check Tiff tags
 * \see error_codes.h
 * \warning Assumes that every strip has the same number of bytes. In this case there are 'rows' strips of size 2*cols bytes.
*/
extern "C" __declspec(dllexport) BYTE Tiff_WriteImage(const char* image_name, UINT16* const _data, UINT16 _nrows, UINT16 _ncols)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	TIFF* tif;										// handler of file
	if(NULL==_data)																				// Something wrong on LV side
	{
		PANTHEIOS_TRACE_CRITICAL(PSTR("NULL input pointer"));
		return NULL_POINTER;
	}
	TIFFSetWarningHandler(WarnHandler);													// redirecting warnings to log
	TIFFSetErrorHandler(ErrorHandler);													// redirecting errors to log
	try
	{
		tif = TIFFOpen(image_name, "w");												// open image
	}
	catch(TIFFException& e) // caught also read/write exceptions
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Caught exception in TIFFOpen "), e.what());
		return FILE_READ_ERROR;
	}
	// set fields
	try
	{
		if(0==TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, _ncols)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_IMAGELENGTH, _nrows)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, _nrows)) throw TIFFException("Error TIFFSetField");

		if(0==TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG)) throw TIFFException("Error TIFFSetField");

		if(0==TIFFSetField(tif, TIFFTAG_XRESOLUTION, 150.0)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_YRESOLUTION, 150.0)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH)) throw TIFFException("Error TIFFSetField");
		if(0==TIFFSetField(tif, TIFFTAG_SOFTWARE, "LV")) throw TIFFException("Error TIFFSetField");
	}
	catch(TIFFException& e) // caught also read/write exceptions
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Caught exception in TIFFSetField "),e.what());
		TIFFClose(tif);
		return OTHER_ERROR;
	}
	// write data to file
	if(-1==TIFFWriteEncodedStrip(tif, 0, _data, _nrows * _ncols * 2))
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFWriteEncodedStrip"));
		TIFFClose(tif);
		return FILE_READ_ERROR;
	}
	TIFFClose(tif);
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
	return OK;
}

 /** 
 * Process all warnings called by LibTiff.
 * This function is called by libtiff every time when an warn is generated. The message passed here will be copied to log file with formatting.
 * \param[in] format is a printf(3S) format string
 * \param[in] params any number arguments
 * \param[in] title if non-zero, is printed before the message; it typically is used to identify the software module in which an error is detected.
 * \see http://www.libtiff.org/libtiff.html#Errors
*/
 EXPORTTESTING void WarnHandler(const char* title, const char* format, va_list params)
 {
	 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	 char message[MessgaeBufforSize];				// to hold Message passed by library 
	 sprintf_s(message,MessgaeBufforSize,format,params);
	 PANTHEIOS_TRACE_WARNING(title,message);
	 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
 }

 /** 
 * Process all errors pushed by LibTiff.
 * This function is called by libtiff every time when an warn is generated. The message passed here will be copied to log file with formatting.
 * \param[in] format is a printf(3S) format string
 * \param[in] params any number arguments
 * \param[in] title if non-zero, is printed before the message; it typically is used to identify the software module in which an error is detected.
 * \see http://www.libtiff.org/libtiff.html#Errors
 * \throw TIFFException( \a message ), where \a message comes from libtiff
*/
 EXPORTTESTING void ErrorHandler( const char* title, const char* format, va_list params )
 {
	 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	 char message[MessgaeBufforSize];				// to hold Message passed by library 
	 sprintf_s(message,MessgaeBufforSize,format,params);
	 PANTHEIOS_TRACE_ERROR(title,message);
	 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
	 throw TIFFException(message);								// throw exception with nessage
 }

 /**
  * Check TIFF image for selected properties.
  * \param[in] tiff Handler from TIFFOpen
  * \return operation status
  * \retval error_codes defined in error_codes.h
  * \li OK - no error
  * \li UNSUPPORTED_IMAGE - if image is unsuperted
  * \li OTHER_ERROR - Undefined error
  * \warning tif must be initialized before
  */ 
 EXPORTTESTING BYTE checkTIFF( TIFF* tif )
 {
	 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	 _ASSERT(tif);	// if NULL pointer
	 UINT16 bitsPerSample;																		// number of bits on pixel
	 UINT16 samplesPerPixel;		// SamplesPerPixel is usually 1 for bilevel, grayscale, and palette-color images. SamplesPerPixel is usually 3 for RGB images.
	 UINT32 tileWidth;
	 int TIFFReturnValue;

	 // check supported tiff format
	 TIFFReturnValue = TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	 if(TIFFReturnValue!=1)																						// error
	 {
		 PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		 return OTHER_ERROR;
	 }
	 TIFFReturnValue = TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	 if(TIFFReturnValue!=1)																						// error
	 {
		 PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		 return OTHER_ERROR;
	 }

	 if( TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth)==NULL &&		// no tiles
		 16==bitsPerSample &&											// 16 bit image
		 1==samplesPerPixel)											// 1 sample per pixel
	 {
		 PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
		 return OK;
	 }
	 else
	 {
		 PANTHEIOS_TRACE_ERROR(PSTR("Unsupported image"));
		 return UNSUPPORTED_IMAGE;	 
	 }
 }
