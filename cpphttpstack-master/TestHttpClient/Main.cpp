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

#include "HttpClient.h"
#include "SipTcp.h"
#include "Log.h"
#include "TestHttpClient.h"
#include "MemoryDebug.h"

int main( int argc, char * argv[] )
{
#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( argc >= 2 )
	{
		if( !strcmp( argv[1], "soap" ) )
		{
			TestHttpClientSoap( argc, argv );
		}
		else if( !strcmp( argv[1], "post" ) )
		{
			TestHttpClientPost( argc, argv );
		}
		else if( !strcmp( argv[1], "upload" ) )
		{
			TestHttpClientUpload( argc, argv );
		}
		else if( !strcmp( argv[1], "2" ) )
		{
			TestHttpClient2Get( argc, argv );
		}
		else if( !strcmp( argv[1], "loop" ) )
		{
			TestHttpClient2Loop( argc, argv );
		}
		else
		{
			TestHttpClientGet( argc, argv );
		}
	}

	return 0;
}
