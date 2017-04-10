/*
Syn's AyyWare Framework
*/

#pragma once

// Includes
#include "CommonIncludes.h"
#include <time.h>

// Colors for the console
//Define extra colours
#define FOREGROUND_WHITE		    (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_YELLOW       	(FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_CYAN		        (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA	        (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_BLACK		    0

#define FOREGROUND_INTENSE_RED		(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_GREEN	(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_BLUE		(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_WHITE	(FOREGROUND_WHITE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_YELLOW	(FOREGROUND_YELLOW | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_CYAN		(FOREGROUND_CYAN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_MAGENTA	(FOREGROUND_MAGENTA | FOREGROUND_INTENSITY)

// Utilities Namespace
// Purpose: Contains misc functionality for memory related functionality
namespace Utilities
{
	// Debug console controls
	void OpenConsole(std::string Title);
	void CloseConsole();
	void Log(const char *fmt, ...);

	void EnableLogFile(std::string filename);

	void SetConsoleColor(WORD color);

	// Misc Shizz
	std::string GetTimeString();

	// Memory utils
	// Purpose: Provides memeory related functionality (pattern scanning ect)
	namespace Memory
	{
		// Waits for a module to be available, before returning it's base address
		DWORD WaitOnModuleHandle(std::string moduleName);

		// Attempts to locate the given signature and mask in a memory range
		// Returns the address at which it has been found
		DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);

		// Attempts to locate the given text in a memory range
		// Returns the address at which it has been found
		DWORD FindTextPattern(std::string moduleName, char* string);

		class CVMTHookManager
		{
		public:
			CVMTHookManager(void)
			{
				memset(this, 0, sizeof(CVMTHookManager));
			}

			CVMTHookManager(PDWORD* ppdwClassBase)
			{
				bInitialize(ppdwClassBase);
			}

			~CVMTHookManager(void)
			{
				UnHook();
			}
			bool bInitialize(PDWORD* ppdwClassBase)
			{
				m_ppdwClassBase = ppdwClassBase;
				m_pdwOldVMT = *ppdwClassBase;
				m_dwVMTSize = dwGetVMTCount(*ppdwClassBase);
				m_pdwNewVMT = new DWORD[m_dwVMTSize];
				memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(DWORD) * m_dwVMTSize);
				*ppdwClassBase = m_pdwNewVMT;
				return true;
			}
			bool bInitialize(PDWORD** pppdwClassBase) // fix for pp
			{
				return bInitialize(*pppdwClassBase);
			}

			void UnHook(void)
			{
				if (m_ppdwClassBase)
				{
					*m_ppdwClassBase = m_pdwOldVMT;
				}
			}

			void ReHook(void)
			{
				if (m_ppdwClassBase)
				{
					*m_ppdwClassBase = m_pdwNewVMT;
				}
			}

			int iGetFuncCount(void)
			{
				return (int)m_dwVMTSize;
			}

			DWORD dwGetMethodAddress(int Index)
			{
				if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
				{
					return m_pdwOldVMT[Index];
				}
				return NULL;
			}

			PDWORD pdwGetOldVMT(void)
			{
				return m_pdwOldVMT;
			}

			DWORD dwHookMethod(DWORD dwNewFunc, unsigned int iIndex)
			{
				if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
				{
					m_pdwNewVMT[iIndex] = dwNewFunc;
					return m_pdwOldVMT[iIndex];
				}

				return NULL;
			}

		private:
			DWORD dwGetVMTCount(PDWORD pdwVMT)
			{
				DWORD dwIndex = 0;

				for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
				{
					if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
					{
						break;
					}
				}
				return dwIndex;
			}
			PDWORD*    m_ppdwClassBase;
			PDWORD    m_pdwNewVMT, m_pdwOldVMT;
			DWORD    m_dwVMTSize;
		};
	};
};

