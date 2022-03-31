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

#ifndef _SOAP_CLASS_H_
#define _SOAP_CLASS_H_

#include "SoapMethod.h"

/**
 * @ingroup WsdlParser
 * @brief SOAP ���񽺸� �����ϴ� Ŭ����
 */
class CSoapClass
{
public:
	CSoapClass();
	~CSoapClass();

	bool MakeSource( const char * pszFolder );

	/** ���� �̸� */
	std::string m_strName;

	/** ���� URL - HTTP POST URL */
	std::string m_strUrl;

	/** ���� namespave */
	std::string m_strNameSpace;

	/** ���񽺿� ���Ե� �޼ҵ� �� */
	SOAP_METHOD_MAP m_clsMethodMap;

private:
	void GetDefineName( const char * pszName, std::string & strDefine );
	bool MakeHeaderFile( const char * pszClassName, const char * pszFileName, const char * pszFilePath );
	bool MakeSourceFile( const char * pszClassName, const char * pszFileName, const char * pszFilePath );
};

#endif
