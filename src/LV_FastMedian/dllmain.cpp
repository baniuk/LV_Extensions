// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

/// Log file name and initialization of Pantheios API
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PSTR("LV_FastMedian");
#ifndef PANTHEIOS_LOG_FILE_NAME
#define PANTHEIOS_LOG_FILE_NAME	"c:\\LV_FastMedian.pantlog"
#else
#error PANTHEIOS_LOG_FILE_NAME already defined!!
#endif

/**
 * \brief Struktura okreœlaj¹ca minimalny poziom b³edu który trafia do danego logu
 *
 * DEBUG jest poziomem najni¿szym, co znaczy ¿e do pliku trafi wszystko. Ta struktura dzia³a
 * jedynie gdy linkuje siê do biblioteki be.N. Kolejnoœæ b³êdów:
 * -# DEBUG
 * -# INFORMATIONAL
 * -# NOTICE
 * -# WARNING
 * -# ERROR
 * -# CRITICAL
 * -# ALERT
 * -# EMERGENCY
 * \n
 * Do konsoli trafi wszystko powy¿ej DEBUG
 */
pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[]  = {
    { toFile,  PANTHEIOS_SEV_DEBUG },
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(PANTHEIOS_SEV_CRITICAL)
};

/**
 * \brief Struktura ³¹cz¹ca poziom b³edu z konkretnym wyjœciem
 *
 * LOGI::File i LOGI::Console ³¹cz¹ siê z pozycjami w PAN_FE_N_SEVERITY_CEILINGS
 */
pan_be_N_t PAN_BE_N_BACKEND_LIST[] = {
    PANTHEIOS_BE_N_STDFORM_ENTRY(toFile, pantheios_be_file, 0),
    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Logging API initialization
		if (pantheios::pantheios_init() < 0)
		{
			std::cerr << "Failed to initialise the Pantheios logging libraries!\n" << std::endl;
			return FALSE;
		}
		else
		{
			/// \warning Do not erase old file. During test library is called many times.
			pantheios_be_file_setFilePath(PSTR(PANTHEIOS_LOG_FILE_NAME), PANTHEIOS_BEID_ALL);
			pantheios::log_INFORMATIONAL("Logger enabled!");
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		pantheios::log_INFORMATIONAL("Logger disabled!");
		pantheios_be_file_setFilePath(NULL, PANTHEIOS_BEID_ALL);
		pantheios::pantheios_uninit();
		break;
	}
	return TRUE;
}

