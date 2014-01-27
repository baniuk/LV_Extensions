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

/**
 * \class TIFFException
 *
 * \brief Class reperesenting obiect of basic exception derrived from \a exception
 * 
 * Keeps copy of exception description string passed to constructor. String can be obtained by TIFFException::what() method.
 */
class TIFFException :
	public exception
{
public:
	/// Creates exception object with message
	TIFFException(const char* message);
	~TIFFException(void);
	/// Shows message
	virtual const char* what() const throw();
private:
	string message;
};

#endif // TIFFException_h__


