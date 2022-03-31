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

#include "JsonServer.h"
#include "Directory.h"
#include "Log.h"
#include "Setup.h"
#include "MemoryDebug.h"

CHttpStack gclsStack;

int main( int argc, char * argv[] )
{
	if( argc != 2 )
	{
		printf( "[Usage] %s {Setup file full path}\n", argv[0] );
		return 0;
	}

#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( gclsSetup.Read( argv[1] ) == false )
	{
		return 0;
	}

	CJsonServer clsServer;
	CTcpStackSetup clsSetup;

#ifdef WIN32
#ifdef _DEBUG
	CLog::SetLevel( LOG_INFO | LOG_DEBUG | LOG_NETWORK );
#endif
#endif

	// HTTP ���� ��Ʈ ��ȣ�� �����Ѵ�.
	clsSetup.m_iListenPort = 8080;
	clsSetup.m_iMaxSocketPerThread = 1;
	clsSetup.m_iThreadMaxCount = 0;
	clsSetup.m_bUseThreadPipe = false;
	
	/*
	//thread pool �� ����� ���
	clsSetup.m_iMaxSocketPerThread = 10;
	clsSetup.m_iThreadMaxCount = 10;
	clsSetup.m_bUseThreadPipe = true;
	*/

	// HTTP ������ �����Ѵ�. HTTP ��û�� ���ŵǸ� �̿� ���� �̺�Ʈ�� CSimpleHttpServer ��ü�� �����Ѵ�.
	if( gclsStack.Start( &clsSetup, &clsServer ) == false )
	{
		printf( "clsStack.Start error\n" );
		return 0;
	}

	while( clsServer.m_bStop == false )
	{
		sleep(1);
	}

	gclsStack.Stop();

	// ��� �����尡 ����� ������ ����Ѵ�.
	sleep(2);

#ifdef WIN32
	CHttpStack::Release();
	CLog::Release();
#endif

	return 0;
}
