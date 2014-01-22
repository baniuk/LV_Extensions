/**
 * \file    Pantheios_header.h
 * \brief   Includes and definitions for Pantheios libs
 * \details Contains some important definitions having influence on Pantheios work
 *			All additional inserters should be included here.
 *			\li real.hpp
 *			\li pointer.hpp
 *			\li integer.hpp
 *			\li args.hpp
 *			\li m2w.hpp
 *			\li w2m.hpp
 * \warning	Do not contain path and name of log file. Definition PANTHEIOS_LOG_FILE_NAME must be provided in relevan root file 
 * \author  PB
 * \date    2013/09/12
 * \version 0.5
 */
#ifndef Pantheios_header__
#define Pantheios_header__

#include <platformstl/platformstl.h>                /* for platform discrimination */

// pliki nag³ówkowe do funkcji - nie myliæ z linkowaniem dynamicznym poni¿ej
#include <pantheios/pantheios.hpp>

/**
* dodaje do logu nazwê funkcji i linii, musi byæ przed trace.h http://stackoverflow.com/questions/2343821/how-to-include-the-calling-class-and-line-number-in-the-log-using-pantheios
* zmiana wygl¹du logu: http://www.codeproject.com/Articles/27119/Using-Callback-Back-ends-with-the-Pantheios-Loggin
* to musi byc wczesniej przed dolaczeniem trace.h
* gcc nie rozponaje makra __FUNCTION__, ale ma makro __func_, to sa makra standardowe w ANSI C
*/
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_TRACE
	#ifdef __GNUG__ // dla unixa bez nazw funkcji
		#define PANTHEIOS_TRACE_PREFIX  PANTHEIOS_FILELINE_A  
	#else // for win
		#define PANTHEIOS_TRACE_PREFIX         "(" PANTHEIOS_STRINGIZE(__LINE__) "): " __FUNCTION__ ": "
	#endif
#endif /* PANTHEIOS_INCL_PANTHEIOS_H_TRACE */

#include <pantheios/trace.h>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>
// simple - chyba tylko w debug, all zawsze ?? http://sourceforge.net/projects/pantheios/forums/forum/475314/topic/2186546
// N - If you use fe.simple, then all log statements whose severity is less than (=== more severe than, since lower means more severe) the active ceiling will go to all back-ends. 
// The "active ceiling" defaults to PANTHEIOS_SEV_DEBUG (meaning all statements are emitted) in debug builds, and to PANTHEIOS_SEV_NOTICE (meaning all statements with severity emergency, alert, critical, error, 
// warning and notice are emitted) in release builds. You may change the "active ceiling" at any time using pantheios_fe_simple_setSeverityCeiling()
// If you use fe.N, then each log statement may be subjected to filtering on a per-back-end basis, as defined by the contents of the PAN_FE_N_SEVERITY_CEILINGS array.
#include <pantheios/frontends/fe.N.h>
// insertes - do konwersji liczb itp
#include <pantheios/inserters/real.hpp>
#include <pantheios/inserters/pointer.hpp>
#include <pantheios/inserters/integer.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters/m2w.hpp>
#include <pantheios/inserters/w2m.hpp>
#include <pantheios/assert.h>

// http://sourceforge.net/projects/pantheios/forums/forum/475314/topic/2186546
// 
/// Konwersja ci¹gów wymagana przez Pantheios, w zale¿noœci od ustawieñ kompilatora u¿ywa poprawnych ci¹g³ow
#ifndef PSTR
	#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)
#endif
/// Konwersja ³añcuchów CComBSTR do formatu akceptowanego przez makro PANTHEIOS_TRACE_XXX
#ifndef PW2M
	#define PW2M(x)		pantheios::w2m(x.m_str,x.Length())
#endif

// Biblioteki ³¹czone dynamicznie, nie trzeba wpisywac ich nazwy. Dla UNIXA moze byæ inaczej. Przyk³ad rozpoznania:
// http://www.pantheios.org/doc/html/c_2example_8c_8N_2example_8c_8N_8c-example.html#a10
// Trzeba pilnowac co siê ³¹czy, biblioteki s¹ zamienne ale program mo¿e inaczej dzia³aæ. Na przyk³ad jesli zamiast fe.N do³¹czy siê fe.simple to do obu Ÿróde³ pójdzie to samo bez filtrowania
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.N.h>
#include <pantheios/implicit_link/be.N.h>
#include <pantheios/implicit_link/be.file.h>
#include <pantheios/implicit_link/be.fprintf.h>

/// Typ wyliczeniowy okreslaj¹cy cele logów
enum LOGI {
	toFile = 1,  /**< Logowanie do pliku */
	toConsole = 2 /**< Logowanie do konsoli */
};

// adds decoding HRESULT errors (winstl::error_desc_a(err_code))
#include <winstl/error/error_desc.hpp>

#endif // Pantheios_header__