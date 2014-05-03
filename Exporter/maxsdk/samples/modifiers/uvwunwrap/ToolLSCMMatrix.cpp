

/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   


*/


#include "ToolLSCM.h"


LSCMMatrix::LSCMMatrix()
{

}
LSCMMatrix::~LSCMMatrix()
{
	Free();
}

void LSCMMatrix::SetSize( int size)
{
	Free();
	mRow.SetCount(size);
	for (int i = 0; i < mRow.Count(); i++)
	{
		mRow[i] = new LSCMRow();
	}
}
int LSCMMatrix::Size()
{
	return mRow.Count();
}
void LSCMMatrix::Free()
{
	for (int i = 0; i < mRow.Count(); i++)
	{
		if (mRow[i])
			delete mRow[i];
		mRow[i] = NULL;
	}
	mRow.SetCount(0);
}

LSCMRow* LSCMMatrix::GetRow(int index)
{
	if ((index < 0) || (index >= Size()))
		return NULL;
	return mRow[index];
}