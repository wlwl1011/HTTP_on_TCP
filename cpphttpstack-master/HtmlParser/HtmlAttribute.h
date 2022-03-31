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

#ifndef _HTML_ATTRIBUTE_H_
#define _HTML_ATTRIBUTE_H_

#include <string>
#include <map>

/**
 * @ingroup HtmlParser
 * @brief HTML ��Ʈ����Ʈ�� ���� �����ϴ� Ŭ����
 */
class CHtmlAttributeValue
{
public:
	CHtmlAttributeValue();

	/** ��Ʈ����Ʈ �� ������ ( ' �Ǵ� " ) */
	char m_cSep;

	/** ��Ʈ����Ʈ �� */
	std::string m_strValue;
};

/**
 * @ingroup HtmlParser
 * @brief HTML attribute map. key �� attribute �̸��̰� value �� attribute ���̴�.
 */
typedef std::map< std::string, CHtmlAttributeValue > HTML_ATTRIBUTE_MAP;

#endif
