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

#ifndef _WSDL_MESSAGE_H_
#define _WSDL_MESSAGE_H_

#include "XmlElement.h"
#include "SoapClass.h"

/**
 * @defgroup WsdlParser WsdlParser
 * WSDL �ļ� ���̺귯��
 */

/**
 * @ingroup WsdlParser
 * @brief WSDL �޽��� ���� Ŭ����
 */
class CWsdlMessage
{
public:
	CWsdlMessage();
	~CWsdlMessage();

	bool Parse( const char * pszText, int iTextLen );
	bool Parse( std::string & strText );

	bool ParseFile( const char * pszFileName );

	CSoapClass * GetSoap( );

private:
	CXmlElement m_clsRoot;
	CSoapClass	m_clsSoap;

	bool ParseXml( );
	bool GetMessageArgList( const char * pszName, SOAP_ARG_LIST & clsArgList );
	bool GetTypeArgList( const char * pszName, SOAP_ARG_LIST & clsArgList );
};

#endif
