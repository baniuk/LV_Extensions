/**
 * \file    TIFFException.cpp
 * \brief	Defines own exception object to be thrown in case of TIFF error
 * \author  PB
 * \date    2014/01/25
 */

#include "stdafx.h"
#include "TIFFException.h"


TIFFException::TIFFException(void)
{
}


TIFFException::~TIFFException(void)
{
}

const char* TIFFException::what() const throw()
{
	return "Problem with TIFF image";
}
