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
#include "TcpStack.h"
#include "TcpThread.h"
#include "TcpClientMap.h"
#include "Log.h"
#include "MemoryDebug.h"

CTcpClientIndex::CTcpClientIndex() : m_iThreadIndex(-1), m_iSessionIndex(-1)
{
}

CTcpClientInfo::CTcpClientInfo() : m_bTrying(true)
{
}

CTcpClientMap::CTcpClientMap() : m_pclsStack(NULL)
{
}

CTcpClientMap::~CTcpClientMap()
{
}

/**
 * @ingroup TcpStack
 * @brief CTcpStack ��ü�� �����Ѵ�.
 * @param pclsStack CTcpStack ��ü
 * @returns true �� �����Ѵ�.
 */
bool CTcpClientMap::Create( CTcpStack * pclsStack )
{
	m_pclsStack = pclsStack;

	return true;
}

/**
 * @ingroup TcpStack
 * @brief TCP ������ �����Ͽ��� TCP ��Ŷ�� �����Ѵ�. �̹� ����� TCP �������� TCP ��Ŷ�� �����Ѵ�.
 * @param pszIp				TCP ���� IP �ּ�
 * @param iPort				TCP ���� ��Ʈ ��ȣ
 * @param pszPacket		��Ŷ
 * @param iPacketLen	��Ŷ ����
 * @param bConnectIfNoSession	TCP ������ �������� ������ ���ο� TCP ������ ������ ��, ��Ŷ�� �����ϴ°�?
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CTcpClientMap::Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;
	CTcpClientIndex clsIndex;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		if( itMap->second.m_bTrying )
		{
			// TCP connect ���� ���̸� ���۵� ��Ŷ�� �ڷᱸ���� �����Ѵ�.
			std::string * pstrPacket = new std::string();
			if( pstrPacket )
			{
				pstrPacket->append( pszPacket, iPacketLen );
				itMap->second.m_clsSendPacketList.push_back( pstrPacket );
				bRes = true;
			}
		}
		else
		{
			// TCP connected �����̸� �ش� �������� �����ϱ� ���� ������ �����Ѵ�.
			clsIndex = itMap->second.m_clsIndex;
			bRes = true;
		}
	}
	else if( bConnectIfNoSession )
	{
		std::string * pstrPacket = new std::string();
		if( pstrPacket )
		{
			CTcpClientInfo clsInfo;

			pstrPacket->append( pszPacket, iPacketLen );
			m_clsMap.insert( TCP_CLIENT_MAP::value_type( strKey, clsInfo ) );
			itMap = m_clsMap.find( strKey );
			itMap->second.m_clsSendPacketList.push_back( pstrPacket );

			if( StartTcpClientThread( pszIp, iPort, m_pclsStack ) == false )
			{
				delete pstrPacket;
				m_clsMap.erase( itMap );
			}
			else
			{
				bRes = true;
			}
		}
	}
	m_clsMutex.release();

	if( clsIndex.m_iThreadIndex != -1 && clsIndex.m_iSessionIndex != -1 )
	{
		m_pclsStack->Send( clsIndex.m_iThreadIndex, clsIndex.m_iSessionIndex, pszPacket, iPacketLen );
	}

	return bRes;
}

/**
 * @ingroup TcpStack
 * @brief TCP ���� ������ �����Ѵ�.
 * @param pszIp TCP ���� IP �ּ�
 * @param iPort TCP ���� ��Ʈ ��ȣ
 * @returns true �� �����Ѵ�.
 */
bool CTcpClientMap::Delete( const char * pszIp, int iPort )
{
	SEND_PACKET_LIST clsSendPacketList;
	SEND_PACKET_LIST::iterator	itList;

	Delete( pszIp, iPort, clsSendPacketList );
	for( itList = clsSendPacketList.begin(); itList != clsSendPacketList.end(); ++itList )
	{
		//m_pclsStack->m_pclsCallBack->SendPacketError( pszIp, iPort, (*itList)->c_str(), (*itList)->length() );
		delete *itList;
	}

	return true;
}

/**
 * @ingroup TcpStack
 * @brief TCP ���� ��, ������ ��Ŷ ����Ʈ�� �����Ѵ�.
 * @param pszIp		TCP ���� IP �ּ�
 * @param iPort		TCP ���� ��Ʈ ��ȣ
 * @param clsList TCP ���� ��, ������ ��Ŷ ����Ʈ ���� ����
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CTcpClientMap::DeleteSendPacketList( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	clsList.clear();
	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		clsList = itMap->second.m_clsSendPacketList;
		itMap->second.m_clsSendPacketList.clear();
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TcpStack
 * @brief TCP ��������� �����Ѵ�.
 * @param pszIp					TCP ���� IP �ּ�
 * @param iPort					TCP ���� ��Ʈ ��ȣ
 * @param iThreadIndex	TCP ������ ��ȣ
 * @param iSessionIndex TCP ���� ��ȣ ( TCP ������ ���� ���� ��ȣ )
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CTcpClientMap::SetConnected( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		itMap->second.m_clsIndex.m_iThreadIndex = iThreadIndex;
		itMap->second.m_clsIndex.m_iSessionIndex = iSessionIndex;
		itMap->second.m_bTrying = false;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TcpStack
 * @brief TCP ������ �����Ѵ�.
 * @param pszIp					TCP ���� IP �ּ�
 * @param iPort					TCP ���� ��Ʈ ��ȣ
 * @param iThreadIndex	TCP ������ ��ȣ
 * @param iSessionIndex TCP ���� ��ȣ ( TCP ������ ���� ���� ��ȣ )
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CTcpClientMap::Insert( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex )
{
	bool bRes = false;
	std::string strKey;
	CTcpClientInfo clsInfo;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	clsInfo.m_bTrying = false;
	clsInfo.m_clsIndex.m_iThreadIndex = iThreadIndex;
	clsInfo.m_clsIndex.m_iSessionIndex = iSessionIndex;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap == m_clsMap.end() )
	{
		m_clsMap.insert( TCP_CLIENT_MAP::value_type( strKey, clsInfo ) );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TcpStack
 * @brief TCP ���� ������ �����Ѵ�.
 * @param pszIp TCP ���� IP �ּ�
 * @param iPort TCP ���� ��Ʈ ��ȣ
 * @param clsList TCP ���� ��, ������ ��Ŷ ����Ʈ ����� ����
 * @returns �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CTcpClientMap::Delete( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		clsList = itMap->second.m_clsSendPacketList;
		bRes = true;
		m_clsMap.erase( itMap );
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TcpStack
 * @brief �ڷᱸ���� Ű�� �����Ѵ�.
 * @param pszIp TCP ���� IP �ּ�
 * @param iPort TCP ���� ��Ʈ ��ȣ
 * @param strKey �ڷᱸ���� Ű�� ������ ����
 */
void CTcpClientMap::GetKey( const char * pszIp, int iPort, std::string & strKey )
{
	char szPort[6];

	snprintf( szPort, sizeof(szPort), "%d", iPort );

	strKey = pszIp;
	strKey.append( ":" );
	strKey.append( szPort );
}
