///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "D3D9Help.h"

HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName )
{
	UINT cbLength;

	if( Name != NULL )
	{
		cbLength = ( UINT )strlen( Name ) + 1;
		*pNewName = new CHAR[cbLength];
		if( *pNewName == NULL )
			return E_OUTOFMEMORY;
		memcpy( *pNewName, Name, cbLength * sizeof( CHAR ) );
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}
