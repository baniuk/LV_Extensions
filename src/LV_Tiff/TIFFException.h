/**
 * \file    TIFFException.h
 * \brief	Defines own exception object to be thrown in case of TIFF error
 * \author  PB
 * \date    2014/01/25
 */

#ifndef TIFFException_h__
#define TIFFException_h__

#include <exception>

using namespace std;

class TIFFException :
	public exception
{
public:
	TIFFException(void);
	~TIFFException(void);
	virtual const char* what() const throw();
};

#endif // TIFFException_h__


