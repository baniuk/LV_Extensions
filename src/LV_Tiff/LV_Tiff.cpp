/**
 * \file    LV_Tiff.cpp
 * \brief	Obs³uga tiffa
 * \detail Eksportuje nastêpuj¹ce funkcje
 * \li Tiff_GetParams - zwraca rozmiary obrazu
 * \li Tiff_Read - wczytuje obraz
 * \author  PB
 * \date    2014/01/22
 */

#include "stdafx.h"

/** 
 * Odczytuje parametry obrazy wejœciowego i zwraca do LabView celem allokacji pamiêci
 * \param[in] image_name	nazwa i œciezka obrazu wejœciowego
 * \param[out] _nrows	liczba wierszy obrazu
 * \param[out] _ncols	liczba kolumn obrazu
 * \return Status operacji
 * \retval
 * \li OK - brak b³êdu
 * \li NULL_POINTER
 * \li FILE_READ_ERROR
 * \li OTHER_ERROR - nieokreœlony b³¹d
 * \see http://www.libtiff.org/man/TIFFGetField.3t.html
 * \see http://www.libtiff.org/libtiff.html
*/
extern "C" __declspec(dllexport) BYTE Tiff_GetParams(const char* image_name, UINT16* const _nrows, UINT16* const _ncols)
{
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Entering"));
	int TIFFReturnValue;
	UINT32 ncols, nrows;																							// width and height of tiff image
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
	TIFFReturnValue = TIFFGetField(tif,TIFFTAG_IMAGEWIDTH, &nrows);												// read height
	if(TIFFReturnValue!=1)																						// error
	{
		PANTHEIOS_TRACE_ERROR(PSTR("Error in TIFFGetField"));
		return OTHER_ERROR;
	}
	*_nrows = nrows;
	*_ncols = ncols;
	PANTHEIOS_TRACE_DEBUG(PSTR("Image size [rows;cols]"),  PSTR("["),pantheios::integer(nrows), PSTR(","), pantheios::integer(ncols), PSTR("]"));
	// ---------- Cleaning ----------
	TIFFClose(tif);																								// closing image
	PANTHEIOS_TRACE_INFORMATIONAL(PSTR("Leaving"));
	return OK;
}