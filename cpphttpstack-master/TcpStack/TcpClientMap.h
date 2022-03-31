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

#ifndef _TCP_CLIENT_MAP_H_
#define _TCP_CLIENT_MAP_H_

#include "SipMutex.h"
#include <string>
#include <list>
#include <map>

/**
 * @ingroup TcpStack
 * @brief TCP ������ ����
 */
class CTcpClientIndex
{
public:
	CTcpClientIndex();

	/** TCP ������ ��ȣ */
	int		m_iThreadIndex;

	/** TCP ������ ���� ���� ��ȣ */
	int		m_iSessionIndex;
};

typedef std::list< std::string * > SEND_PACKET_LIST;

/**
 * @ingroup TcpStack
 * @brief TCP Ŭ���̾�Ʈ ����
 */
class CTcpClientInfo
{
public:
	CTcpClientInfo();

	bool	m_bTrying;
	CTcpClientIndex	m_clsIndex;
	SEND_PACKET_LIST m_clsSendPacketList;
};

typedef std::map< std::string, CTcpClientInfo > TCP_CLIENT_MAP;

class CTcpStack;

/**
 * @ingroup TcpStack
 * @brief TCP Ŭ���̾�Ʈ�� �����Ͽ��� TCP ��Ŷ�� �����ϴ� Ŭ����
 */
class CTcpClientMap
{
public:
	CTcpClientMap();
	~CTcpClientMap();

	bool Create( CTcpStack * pclsStack );

	bool Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession );
	bool Delete( const char * pszIp, int iPort );
	bool DeleteSendPacketList( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList );

	bool SetConnected( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex );
	bool Insert( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex );

private:
	bool Delete( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList );
	void GetKey( const char * pszIp, int iPort, std::string & strKey );

	TCP_CLIENT_MAP	m_clsMap;
	CSipMutex				m_clsMutex;
	CTcpStack			* m_pclsStack;
};

#endif
