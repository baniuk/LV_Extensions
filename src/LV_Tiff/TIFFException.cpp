/**
 * \file    TIFFException.cpp
 * \brief	Defines own exception object to be thrown in case of TIFF error
 * \author  PB
 * \date    2014/01/25
 */

#include "stdafx.h"
#include "TIFFException.h"

/**
 * Creates exception object with message.
 * \param[in] message Message to be writen in the object
 */ 
TIFFException::TIFFException(const char* message)
{
	this->message = message;
}


TIFFException::~TIFFException(void)
{
}

/**
 * \details Return description of exception passed to obiect in constructor.
 * \return Pointer to string
 */ 
const char* TIFFException::what() const throw()
{
	return message.c_str();
}
