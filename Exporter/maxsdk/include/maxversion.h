#pragma once

#include "buildnumber.h" // defines VERSION_INT

// VERSION_INT is defined in buildnumber.h and is written to by the builder, inserting the current build number.
// VERSION_INT is used for the fourth least signifanct version component eg. 4.1.1.[36]

// The product and file version could be different.
// For example, VIZ 4.0 works with max files version 4.2

// 3ds Max internal version number is managed using the following scheme:
// Major = Main release number (ex. 3ds Max 2013 (SimCity) = 15).
// Minor = Service Pack or Extension number; SPs would typically be starting at 1, whereas an Extension would
//			typically be starting at 5 (for helping distinguishing between both). Minor version number would
//			normally be incremented by one for subsequent SPs or Extensions.
//			Note that Point number would also be set to 0 in case of an SP or Extension.
// Point = HotFix number; when releasing a HotFix, increment the Point version number by one;
//			must not be reset after a Service Pack or an Extension; simply increment from the last HotFix number.
//
// Example for SimCity = 15:
//			First Release HotFix would be:				15.0.1
//			Next Release HotFix would be:				15.0.2
//			Release SP1 would be:						15.1.0
//			Next Release HotFix after SP1 would be:		15.1.3
//			Next Release HotFix would be:				15.1.4
//			An SP2 before Extension 1 would be:			15.2.0
//			First Release Extension would be:			15.5.0
//			Next HotFix after Extension1 would be:		15.5.5
//			An SP2 after Extension 1 could either be:	15.2.0 or 15.6.0
//			Etc.

// MAX File version:
#ifndef MAX_VERSION_MAJOR
#define MAX_VERSION_MAJOR 15
#endif

#ifndef MAX_VERSION_MINOR
#define MAX_VERSION_MINOR 0
#endif

#ifndef MAX_VERSION_POINT
#define MAX_VERSION_POINT 0
#endif

// MAX Product version
#ifndef MAX_PRODUCT_VERSION_MAJOR
#define MAX_PRODUCT_VERSION_MAJOR 15
#endif

#ifndef MAX_PRODUCT_VERSION_MINOR
#define MAX_PRODUCT_VERSION_MINOR 0
#endif

#ifndef MAX_PRODUCT_VERSION_POINT
#define MAX_PRODUCT_VERSION_POINT 0
#endif

#ifndef MAX_PRODUCT_YEAR_NUMBER
#define MAX_PRODUCT_YEAR_NUMBER 2013
#endif

// MAX_RELEASE_EXTERNAL is an alternative for MAX_RELEASE (plugapi.h)
// Define it when you want the "branded" (UI) version number to be different 
// from the internal one.
//#define MAX_RELEASE_EXTERNAL	10900

// This should be left blank for the main release, and any subsequent HF up until the first Service Pack.
// Service Pack 1, and any subsequent HF up until the next Service Pack, would then need to be labeled "SP1",
// then the same thing starting with Service Pack 2, which would be labeled "SP2", etc.
#ifndef MAX_PRODUCT_VERSION_SP
#define MAX_PRODUCT_VERSION_SP "\0"
#endif


#define _MAX_VERSION(a, b, c,d) a##b##c##d
#define MAX_VERSION _MAX_VERSION(MAX_VERSION_MAJOR, MAX_VERSION_MINOR, MAX_VERSION_POINT,VERSION_INT)


