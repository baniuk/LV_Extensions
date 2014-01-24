/**
 * \file    LV_Tiff.cpp
 * \brief	Holds Tiff related operations
 * \details Eksportuje następujące funkcje
 * - Tiff_GetParams - zwraca rozmiary obrazu
 * - Tiff_GetImage - wczytuje obraz
 * \pre libtiff3.dll and other dependencies must be on path
 * \author  PB
 * \date    2014/01/22
 */

#include "stdafx.h"

/** 
 * Reads size of the image and return dimmensions to LabView due to memory allocation needs.
 * \param[in] image_name	name and path to the input image
 * \param[out] _nrows	number of rows (height)
 * \param[out] _ncols	number of cols (width)
 * \return operation status
 * \retval error_codes
 * \li OK - no error
 * \li NULL_POINTER
 * \li FILE_READ_ERROR
 * \li OTHER_ERROR
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
 * \see http://www.awaresystems.be/imaging/tiff/astifftagviewer.html to check Tiff tags
 * \todo Supress warnings on read specified Tiffs - incorrect count for field "DateTime" (25, expecting 20); tag trimmed, see tests
 * \todo check supported tiffs in every function
*/
extern "C" __declspec(dllexport) BYTE Tiff_GetParams(const char* image_name, UINT16* const _nrows, UINT16* const _ncols)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	int TIFFReturnValue;
	UINT32 ncols, nrows;																							// width and height of tiff image
	tsize_t sizeOfTiff;
	if(NULL==_nrows || NULL==_ncols)																				// Something wrong on LV side
	{
		PANTHEIOS_TRACE_CRITICAL(PSTR("NULL input pointer"));
		return NULL_POINTER;
	}
	// ---------- Reading tiff ----------
	TIFF* tif = TIFFOpen(image_name, "r");																		// open image
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
 * \retval error_codes
 * \li OK - no error
 * \li NULL_POINTER
 * \li FILE_READ_ERROR - also when unsuported tiff format
 * \li OTHER_ERROR
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
 * \see http://www.awaresystems.be/imaging/tiff/astifftagviewer.html to check Tiff tags
 * \todo Supress warnings on read specified Tiffs - incorrect count for field "DateTime" (25, expecting 20); tag trimmed, see tests
 * \warning Assumes that every srips has the same number of bytes. In this case there are 'rows' strips of size 2*cols bytes.
 * \todo Add protectiona agains wrong or unsupported tiff
*/
extern "C" __declspec(dllexport) BYTE Tiff_GetImage(const char* image_name, UINT16* const _data)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	tsize_t sizeOfTiff;
	tstrip_t strip;
	tdata_t buf;
	UINT16 bitsPerSample;																		// number of bits on pixel
	UINT16 samplesPerPixel;		// SamplesPerPixel is usually 1 for bilevel, grayscale, and palette-color images. SamplesPerPixel is usually 3 for RGB images.
	UINT32 tileWidth;
	int TIFFReturnValue;
	if(NULL==_data)																				// Something wrong on LV side
	{
		PANTHEIOS_TRACE_CRITICAL(PSTR("NULL input pointer"));
		return NULL_POINTER;
	}
	// ---------- Reading tiff ----------
	TIFF* tif = TIFFOpen(image_name, "r");																		// open image
	PANTHEIOS_TRACE_DEBUG(PSTR("File to open: "), image_name);
	if(NULL==tif)																								// error during opening
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in opening image: "), image_name);
		TIFFClose(tif);
		return FILE_READ_ERROR;
	}
	// check supported tiff format
	TIFFReturnValue = TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	if(TIFFReturnValue!=1)																						// error
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		TIFFClose(tif);
		return OTHER_ERROR;
	}
	TIFFReturnValue = TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	if(TIFFReturnValue!=1)																						// error
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		TIFFClose(tif);
		return OTHER_ERROR;
	}
	// test conditions: notiles, 16bit, 1sample per pixel	
	if(	TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth)==NULL &&		// no tiles
		16==bitsPerSample &&											// 16 bit image
		1==samplesPerPixel)												// 1 sample per pixel
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