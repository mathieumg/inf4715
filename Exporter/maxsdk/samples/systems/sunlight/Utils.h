#pragma once

#include "weatherdata.h"
// forward declaration
class WeatherFileCache;

#pragma managed(push, on)

#using <WeatherData.dll>

WeatherHeaderInfo ConvertHeaderToStruct(WeatherData::WeatherHeader^ header);
DaylightWeatherData ConvertEntryToStruct(WeatherFileCache* cache, WeatherData::WeatherEntry^ entry, bool firstConversion);

#pragma managed(pop)

