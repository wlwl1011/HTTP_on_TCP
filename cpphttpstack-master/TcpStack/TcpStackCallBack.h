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

#ifndef _TCP_STACK_CALLBACK_H_
#define _TCP_STACK_CALLBACK_H_

#include "TcpSessionList.h"

/**
 * @ingroup TcpStack
 * @brief CTcpStack �� ����ϴ� ���� callback �������̽�
 */
class ITcpStackCallBack
{
public:
	virtual ~ITcpStackCallBack(){};

	/**
	 * @ingroup TcpStack
	 * @brief TCP Ŭ���̾�Ʈ�� ���� �̺�Ʈ �ڵ鷯
	 * @param pclsSessionInfo ���� ����
	 * @return TCP Ŭ���̾�Ʈ ������ ����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
	 */
	virtual bool InComingConnected( CTcpSessionInfo * pclsSessionInfo ) = 0;

	/**
	 * @ingroup TcpStack
	 * @brief TCP Ŭ���̾�Ʈ ������ ���� �̺�Ʈ �ڵ鷯
	 * @param pclsSessionInfo ���� ����
	 */
	virtual void SessionClosed( CTcpSessionInfo * pclsSessionInfo ) = 0;

	/**
	 * @ingroup TcpStack
	 * @brief TCP ��Ŷ ���� �̺�Ʈ �ڵ鷯
	 * @param pszPacket				���� ��Ŷ
	 * @param iPacketLen			���� ��Ŷ ����
	 * @param pclsSessionInfo ���� ����
	 * @returns TCP ������ �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
	 */
	virtual bool RecvPacket( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo ) = 0;

	/**
	 * @ingroup TcpStack
	 * @brief SendAll �� �����ص� �Ǵ� �������� �˻��Ѵ�.
	 * @param pclsSessionInfo ���� ����
	 * @returns SendAll �� �����ص� �Ǵ� �����̸� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
	 */
	virtual bool IsSendAll( CTcpSessionInfo * pclsSessionInfo ){ return false; };

	/**
	 * @ingroup TcpStack
	 * @brief SendAll �� ������ ���� ���Ǵ� Send �Լ� ȣ����, ȣ��Ǵ� �̺�Ʈ �ڵ鷯
	 * @param pclsSessionInfo ���� ����
	 * @param pszPacket				������ ��Ŷ
	 * @param iPacketLen			���� ��Ŷ ����
	 */
	virtual void AfterSendAllPerSession( CTcpSessionInfo * pclsSessionInfo, const char * pszPacket, int iPacketLen ){};
};

#endif
