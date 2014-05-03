//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/

#include "citylist.h"
#include <strclass.h>
#include <maxapi.h>
#include <winutil.h>
#include <Path.h>
#include <algorithm>

CityList* CityList::sInstance = nullptr;

CityList& CityList::GetInstance()
{
	if(nullptr == sInstance)
	{
		sInstance = new CityList();
	}

	return *sInstance;
}

void CityList::Destroy()
{
	delete sInstance;
	sInstance = nullptr;
}

CityList::CityList()
	: mpDefaultCity(nullptr)
{

}

CityList::~CityList()
{

}

bool CityList::byname(const Entry& p1, const Entry& p2)
{
	const TCHAR* e1 = p1.name;
	const TCHAR* e2 = p2.name;
	if ((e1[0] == _T('+')) || (e1[0] == _T('*')))
		++e1;
	if ((e2[0] == _T('+')) || (e2[0] == _T('*')))
		++e2;
	return _tcscmp(e1, e2) < 0;
}

bool CityList::parseLine(MaxSDK::Util::TextFile::Reader& fp, Entry& entry)
{
	TSTR lineRead = fp.ReadLine();
	if(!lineRead.isNull() && !lineRead.StartsWith(_T(';')))
	{
		const TCHAR* p = lineRead.data();

		int last_index;
		float latitude = 0.0f;
		float longitude = 0.0f;
		// What an AWESOME function!!!
		if (_stscanf_s(p, _T("%f   %f         %n"), &latitude, &longitude, &last_index) == 2 )
		{
			entry.latitude = latitude;
			entry.longitude = longitude; // mz dec 2007, no longer true, we fixed this hopefully pfbreton 13 04 2006: the rest of the max code assumes that negative longitude represents EST, but the Acad data provides us with positive values for EST so I flip the value here
			// the scan function gets strings up to the first white space character... :(
			// Since the cities also need the state, which is separated by a space, scan won't work.
			// Thank goodness that scanf will return the index of the last string it read. Hence I grab a substring for the city.
			TSTR city = lineRead.Substr(last_index, (lineRead.length() - last_index));
			_tcscpy_s(entry.name, MAX_PATH, city.data());
			return true;
		}
	}
	return false;
}

void CityList::initializeList()
{
	mpDefaultCity = nullptr;
	mCityList.clear();
	
	Interface* ip = GetCOREInterface();
	MaxSDK::Util::Path cityFile( ip->GetDir(APP_PLUGCFG_LN_DIR) );
	cityFile.Append( _T("\\sitename.txt") );
	
	if (!cityFile.Exists())
	{
		return;
	}
	//We must ensure we are in ANSII C before opening the file, 
	//because the floating point markers in sitename.txt are "."
	MaxLocaleHandler locale;

	// Open the city file.
	MaxSDK::Util::TextFile::Reader fp;
	Interface14 *iface = GetCOREInterface14();
	UINT codepage  = iface-> DefaultTextLoadCodePage();

	fp.Open(cityFile.GetCStr(), codepage);
	if (!fp.IsFileOpen())
		return;			// No file, return with no cities

	// First count the cities in the file.
	size_t count = 0;
	Entry temp;
	while (!fp.IsEndOfFile()) {
		if (parseLine(fp, temp)) {
			++count;
		}
	}

	if (count <= 0)
		return;		// No Cities

	mCityList.reserve(count);

	// rewind to the start
	fp.Seek( 0L, SEEK_SET);
	size_t i = 0;
	while (!fp.IsEndOfFile())
	{
		Entry entry;
		if (parseLine(fp, entry)) // pass by reference
		{
			mCityList.push_back(entry);
			if (entry.name[0] == _T('*'))
			{
				mpDefaultCity = &(mCityList[mCityList.size() - 1]);
			}
		}
	}
	fp.Close();

	//Now we can restore to the initial locale.
	locale.RestoreLocale();

	std::sort(mCityList.begin(), mCityList.end(), byname);
}

void CityList::init()
{
	if (mCityList.empty()) 
		initializeList();
}

CityList::Entry& CityList::operator[]( size_t i )
{
	return mCityList[i];
}

CityList::Entry* CityList::GetDefaultCity()
{
	return mpDefaultCity;
}

UINT CityList::Count()
{
	return (UINT)mCityList.size();
}

