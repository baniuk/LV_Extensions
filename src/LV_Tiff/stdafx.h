// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <crtdbg.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include "tiffio.h"
#include "Pantheios_header.h"
#include "TIFFException.h"
#include "error_codes.h"


// TODO: reference additional headers your program requires here

/// size of mesage buffor passed from LibTiff 
#define MessgaeBufforSize 1024

/// Defines macro for exporting private functions from DLLs. 
#ifdef _DEBUG
#define EXPORTTESTING extern "C" __declspec(dllexport)
#else
#define EXPORTTESTING 
#endif