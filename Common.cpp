//--------------------------------------------------------------------------------
#include "Common.h"
//--------------------------------------------------------------------------------
void LogWrite( const wchar_t *cTextString )
{
#if _DEBUG
	::OutputDebugStringW( cTextString );
	::OutputDebugStringW( L"\n" );
#endif
}
//--------------------------------------------------------------------------------
void LogWrite( std::wstring& TextString )
{
	LogWrite( TextString.c_str() );
}
//--------------------------------------------------------------------------------