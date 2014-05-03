
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
#pragma unmanaged
#include "WeatherFileCache.h"

#include "Utils.h"

#pragma managed
#using <WeatherData.dll>
#include <ManagedServices/StringConverter.h>

// converts a managed WeatherHeader to a WeatherHeaderInfo struct
WeatherHeaderInfo ConvertHeaderToStruct(WeatherData::WeatherHeader^ header)
{
	ManagedServices::StringConverter<TCHAR> converter;

	WeatherHeaderInfo headerStruct = {_T("")};
	headerStruct.mCity = converter.Convert(header->Location->City);
	headerStruct.mRegion = converter.Convert(header->Location->Region);
	headerStruct.mCountry = converter.Convert(header->Location->Country);
	headerStruct.mSource = converter.Convert(header->Location->Source);
	headerStruct.mWMO = header->Location->WMO;
	headerStruct.mLat = header->Location->Latitude;
	headerStruct.mLon = header->Location->Longitude;
	headerStruct.mTimeZone = header->Location->TimeZone;
	headerStruct.mElevationMeters = header->Location->ElevationMeters;
	headerStruct.mLeapYearObserved = header->HolidaysAndDaylightSavings->LeapYearObserved;
	headerStruct.mDaylightSavingsStart.x = header->HolidaysAndDaylightSavings->DaylightSavings->StartDay->Month;
	headerStruct.mDaylightSavingsStart.y = header->HolidaysAndDaylightSavings->DaylightSavings->StartDay->Day;
	headerStruct.mDaylightSavingsEnd.x = header->HolidaysAndDaylightSavings->DaylightSavings->EndDay->Month;
	headerStruct.mDaylightSavingsEnd.y = header->HolidaysAndDaylightSavings->DaylightSavings->EndDay->Day;

	// The number of records per hour is accessible in each data period as 
	// header->DataPeriods[i]->RecordPerHourCount.  Each data period can have its
	// own value for this.
	int count =0;
	WeatherDataPeriod period;
	for each(WeatherData::DataPeriodDescriptor^ entry in header->DataPeriods)
	{
		period.mStartMDY.x = entry->PeriodStartDate->Month;
		period.mStartMDY.y = entry->PeriodStartDate->Day;
		period.mStartMDY.z = -1;
		period.mEndMDY.x = entry->PeriodEndDate->Month;
		period.mEndMDY.y = entry->PeriodEndDate->Day;
		period.mEndMDY.z = -1;
		headerStruct.mWeatherDataPeriods.Append(1,&period);
		if(count==0)
			headerStruct.mNumRecordsPerHour = 	entry->RecordPerHourCount;
		else
		{
			if(headerStruct.mNumRecordsPerHour!=entry->RecordPerHourCount)
				headerStruct.mNumRecordsPerHour = -1.0f;
		}
	}

	return headerStruct;
}


DaylightWeatherData ConvertEntryToStruct(WeatherFileCache* cache, WeatherData::WeatherEntry^ entry, bool firstEntry)
{
	DaylightWeatherData dataStruct = {0};
	dataStruct.mYear = entry->Year;

	if(dataStruct.mYear==-1)
		dataStruct.mYear = 2011; //just set it to this year.
	dataStruct.mMonth = entry->Month;
	dataStruct.mDay = entry->Day;
	if(firstEntry)
	{	
		if(entry->Hour == 0) //only seen this with weather files from CNRC.
			cache->SubtractHour(false);
		else //assume we subtract that's normal.
			cache->SubtractHour(true);
	}

	if(cache->SubtractHour())
		dataStruct.mHour = entry->Hour-1;
	else
		dataStruct.mHour = entry->Hour;
	dataStruct.mMinute = entry->Minute;
	if(dataStruct.mMinute>=60)
		dataStruct.mMinute -= 60;
	ManagedServices::StringConverter<TCHAR> converter;
	dataStruct.mDataUncertainty = 
		converter.Convert( entry->DataSourceAndUncertaintyFlags->ToString() );

	dataStruct.mDryBulbTemperature = entry->DryBulbTemperature;
	dataStruct.mDewPointTemperature = entry->DewPointTemperature;
	dataStruct.mRelativeHumidity = entry->RelativeHumidity;
	dataStruct.mAtmosphericStationPressure = entry->AtmosphericStationPressure;
	dataStruct.mExtraterrestrialHorizontalRadiation = entry->ExtraterrestrialHorizontalRadiation;
	dataStruct.mExtraterrestrialDirectNormalRadiation = entry->ExtraterrestrialDirectNormalRadiation;
	dataStruct.mHorizontalInfraredRadiationFromSky = entry->HorizontalInfraredRadiationFromSky;
	dataStruct.mGlobalHorizontalRadiation = entry->GlobalHorizontalRadiation;
	dataStruct.mDirectNormalRadiation = entry->DirectNormalRadiation;
	dataStruct.mDiffuseHorizontalRadiation = entry->DiffuseHorizontalRadiation;
	dataStruct.mGlobalHorizontalIlluminance = entry->GlobalHorizontalIlluminance;
	dataStruct.mDirectNormalIlluminance = entry->DirectNormalIlluminance;
	dataStruct.mDiffuseHorizontalIlluminance = entry->DiffuseHorizontalIlluminance;
	dataStruct.mZenithLuminance = entry->ZenithLuminance;

	return dataStruct;
}
