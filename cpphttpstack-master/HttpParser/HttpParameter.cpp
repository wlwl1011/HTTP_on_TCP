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
#include "HttpParameter.h"
#include "MemoryDebug.h"

CHttpParameter::CHttpParameter( char cSep ) : m_cSep(cSep)
{
}

CHttpParameter::~CHttpParameter()
{
}

/**
 * @ingroup HttpParser
 * @brief Parameter ���ڿ��� �Ľ��Ͽ� CHttpParameter Ŭ������ ��� ������ �����Ѵ�.
 * @param pszText		HTTP ����� ���� ������ ���ڿ�
 * @param iTextLen	pszText ���ڿ��� ����
 * @returns �����ϸ� �Ľ��� ���̸� �����ϰ� �׷��� ������ -1 �� �����Ѵ�.
 */
int CHttpParameter::Parse( const char * pszText, int iTextLen )
{
	Clear();
	if( pszText == NULL || iTextLen <= 0 ) return -1;

	int		iPos, iValuePos = -1;

	for( iPos = 0; iPos < iTextLen; ++iPos )
	{
		if( pszText[iPos] == '=' )
		{
			m_strName.append( pszText, iPos );
			iValuePos = iPos + 1;
		}
		else if( pszText[iPos] == m_cSep )
		{
			break;
		}
	}

	if( iPos > 0 )
	{
		if( iValuePos != -1 )
		{
			m_strValue.append( pszText + iValuePos, iPos - iValuePos );
		}
		else
		{
			m_strName.append( pszText, iPos );
		}

		return iPos;
	}

	return -1;
}

/**
 * @ingroup HttpParser
 * @brief HTTP �޽����� ���Ե� ���ڿ��� �ۼ��Ѵ�.
 * @param pszText		HTTP ����� ���� ������ ���ڿ� ����
 * @param iTextSize	pszText ������ ũ��
 * @returns �����ϸ� �ۼ��� ���ڿ� ���̸� �����ϰ� �׷��� ������ -1 �� �����Ѵ�.
 */
int CHttpParameter::ToString( char * pszText, int iTextSize )
{
	if( pszText == NULL || iTextSize <= 0 ) return -1;

	if( m_strName.empty() ) return 0;

	if( m_strValue.empty() )
	{
		return snprintf( pszText, iTextSize, "%s", m_strName.c_str() );
	}

	return snprintf( pszText, iTextSize, "%s=%s", m_strName.c_str(), m_strValue.c_str() );
}

/**
 * @ingroup HttpParser
 * @brief ��� ������ �ʱ�ȭ��Ų��.
 */
void CHttpParameter::Clear()
{
	m_strName.clear();
	m_strValue.clear();
}
