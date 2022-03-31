/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "SipPlatformDefine.h"
#include <stdio.h>
#include "MemoryDebug.h"

bool TestHtmlAttribute( );
bool TestHtmlClass();
bool TestHtmlElement( );
bool TestHtmlGetText();
bool TestHtmlUrlList( const char * pszHtmlFileName );

int main( int argc, char * argv[] )
{
	bool bOk = false;

#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( argc == 3 )
	{
		if( !strcmp( argv[1], "urllist" ) )
		{
			TestHtmlUrlList( argv[2] );
		}

		return 0;
	}

	if( TestHtmlGetText() == false ) goto FUNC_END;
	if( TestHtmlElement() == false ) goto FUNC_END;
	if( TestHtmlAttribute() == false ) goto FUNC_END;
	if( TestHtmlClass() == false ) goto FUNC_END;

	bOk = true;

FUNC_END:

	if( bOk )
	{
		printf( "SUCCESS!!!\n" );
	}
	else
	{
		printf( "ERROR!!!\n" );
	}

	return 0;
}
