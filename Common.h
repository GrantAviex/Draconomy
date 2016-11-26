//--------------------------------------------------------------------------------
// Common header to only include often used items in one place
//--------------------------------------------------------------------------------
#ifndef Common2_h
#define Common2_h
//--------------------------------------------------------------------------------
//#include <vld.h>
//#define WIN32_LEAN_AND_MEAN // Exclude rarely used stuff
#include <windows.h>
#include <vector>
#include <assert.h>
//--------------------------------------------------------------------------------
void LogWrite( const wchar_t *cTextString );
void LogWrite( std::wstring& TextString );
//--------------------------------------------------------------------------------
#endif Common2_h
//--------------------------------------------------------------------------------
