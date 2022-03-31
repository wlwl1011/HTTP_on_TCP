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

#include "CallMap.h"

CCallMap gclsCallMap;

CCallMap::CCallMap()
{
}

CCallMap::~CCallMap()
{
}

/**
 * @ingroup TestWebRtc
 * @brief ��ȭ ������ �����Ѵ�.
 * @param pszCaller �߽��� ���̵�
 * @param pszCallee ������ ���̵�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CCallMap::Insert( const char * pszCaller, const char * pszCallee )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszCaller );
	if( itMap == m_clsMap.end() )
	{
		itMap = m_clsMap.find( pszCallee );
		if( itMap == m_clsMap.end() )
		{
			m_clsMap.insert( CALL_MAP::value_type( pszCaller, pszCallee ) );
			m_clsMap.insert( CALL_MAP::value_type( pszCallee, pszCaller ) );
			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief ��ȭ ���� ���̵� �˻��Ѵ�.
 * @param pszUserId		����� ���̵�
 * @param strOtherId	��ȭ ���� ���̵�
 * @returns �˻��Ǹ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CCallMap::Select( const char * pszUserId, std::string & strOtherId )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		strOtherId = itMap->second;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup TestWebRtc
 * @brief ��ȭ ������ �����Ѵ�.
 * @param pszUserId ����� ���̵�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CCallMap::Delete( const char * pszUserId )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		std::string strOther = itMap->second;
		m_clsMap.erase( itMap );

		itMap = m_clsMap.find( strOther );
		if( itMap != m_clsMap.end() )
		{
			m_clsMap.erase( itMap );
		}
	}
	m_clsMutex.release();

	return bRes;
}
