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
/**********************************************************************
 *<
	FILE: weatherdata.h

	DESCRIPTION:	Weather Data UI and various structs

	CREATED BY: Michael Zyracki

	HISTORY: Created Nov 2007

 *>	Copyright (c) 2007, All Rights Reserved.
 **********************************************************************/

#pragma once

#include <list>
#include <vector>
#include <strclass.h>
#include <maxtypes.h>
// forward declarations
class ILoad;
class ISave;

/*
	Following are cache values coming out from the WeatheData reader. We do this for multiple reasons.
	1)  The WeatherFile reader expects a weather file as input and we need to make sure that we correctly close the file
	handle so we need to allocate it on the stack and then deallocate it.  The current interface doesn't allow us to
	open and close files easily
	2) We don't want all of the information that's in the weather. Saving subsections will save memory.
	3) We may need special data structures, instead of just a single list to point at each period and to have a special
	iterator.
*/
class Int3
{
public:
	Int3(): x(0),y(0),z(0){};
	Int3(int a, int b, int c): x(a),y(b),z(c){};
	int x,y,z;
};

struct WeatherDataPeriod
{
	Int3 mStartMDY; //year will be blank usually
	Int3 mEndMDY; //year will be blank usually
};

struct WeatherHeaderInfo
{
	TSTR mCity;
	TSTR mRegion;
	TSTR mCountry;
	TSTR mSource;
	int mWMO;
	float mLat;
	float mLon;
	float mTimeZone;
	float mElevationMeters;
	bool  mLeapYearObserved;
	Tab<WeatherDataPeriod> mWeatherDataPeriods;
	float mNumRecordsPerHour; //will be -1 if they are all different
	Int3 mDaylightSavingsStart;
	Int3 mDaylightSavingsEnd;
};

struct MDY
{
	int month;
	int day;
	int year;
};


struct DaylightWeatherData
{
	int mYear;
	int mMonth;
	int mDay;
	int mHour;
	int mMinute;
	TSTR mDataUncertainty;
	float mDryBulbTemperature; //0,1
	float mDewPointTemperature;//2,3
	float mRelativeHumidity; //4,5
	float mAtmosphericStationPressure; //6,7
	float mExtraterrestrialHorizontalRadiation;
	float mExtraterrestrialDirectNormalRadiation;
	float mHorizontalInfraredRadiationFromSky; //8.9
	float mGlobalHorizontalRadiation;  //10,11
	float mDirectNormalRadiation;  //12,13
	float mDiffuseHorizontalRadiation;  //14,15
	float mGlobalHorizontalIlluminance;  //16,17
	float mDirectNormalIlluminance;     //18,19
	float mDiffuseHorizontalIlluminance; //20,21
	float mZenithLuminance;   //22,23

};

typedef std::list<DaylightWeatherData> DaylightWeatherDataEntries;
typedef std::vector<DaylightWeatherData> DaylightWeatherDataVector;


//the values of these parameters may effect the cache of the filter.
struct WeatherCacheFilter
{
	//this enum and string id table need to be kept in sync. eEnd is always the last one.
	enum FramePer{ePeriod =0,eDay,eWeek,eMonth,eSeason,eEnd};
	BOOL mSkipHours;
	int mSkipHoursStart;
	int mSkipHoursEnd;
	BOOL mSkipWeekends;
	FramePer mFramePer;
	WeatherCacheFilter() :mSkipHours(FALSE),mSkipHoursStart(18), mSkipHoursEnd(8), mSkipWeekends(FALSE),mFramePer(ePeriod){}
};

//This struct contains all of the information that can get passed into and out of the weather file dialog.
//This gets filled out as input into the dialog and then filled out on exit as the dialog operates.
struct WeatherUIData
{
	TSTR mFileName; //this is the full path file name, as a string.
	BOOL mAnimated; //if TRUE then animated with start and end, otherwise single date.
	Int3 mSpecificTimeMDY;
	Int3 mSpecificTimeHMS;
	Int3 mAnimStartTimeMDY;
	Int3 mAnimStartTimeHMS;
	Int3 mAnimEndTimeMDY;
	Int3 mAnimEndTimeHMS;
	WeatherCacheFilter mFilter;
	bool mResetTimes;
	WeatherUIData(): mAnimated(FALSE),mSpecificTimeMDY(1,1,2007),mSpecificTimeHMS(12,0,0),
		mAnimStartTimeMDY(1,1,2007),mAnimStartTimeHMS(12,0,0),
		mAnimEndTimeMDY(12,31,2007),mAnimEndTimeHMS(12,0,0), mResetTimes(true)
		{};
	IOResult Load(ILoad *iload);
	IOResult Save(ISave *isave);

};

