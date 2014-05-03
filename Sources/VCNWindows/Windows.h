///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Windows service interface
///

#ifndef VCNWINDOWS_H
#define VCNWINDOWS_H

#pragma once

#include "VCNCore/System.h"

// Forward declarations
struct VCNWindowsImpl;

class VCNWindows : public VCNSystem
{
	VCN_CLASS;

public:

	VCNWindows(void);

	virtual ~VCNWindows(void);

	/// VCNCore overrides

	/// Called at startup
	virtual VCNBool Initialize() override;

	/// Called when the app closes
	virtual VCNBool Uninitialize() override;

	/// Track stats of OS
	virtual VCNBool Process(const float elapsedTime) override;

	/// VCNSystem overrides

	virtual VCNUInt64 GetUsedMemory() const override;
	virtual VCNUInt64 GetMinUsedMemory() const override;
	virtual VCNUInt64 GetMaxUsedMemory() const override;
	virtual VCNUInt64 GetFreeMemory() const override;
	virtual VCNFloat32 GetDMPS() const override;
	virtual void FindFilesRecursively(const VCNTChar* lpFolder, const VCNTChar* lpFilePattern, std::function<void(const VCNTChar* path)> func) override;

private:

	/// Used to declare OS specific handles
	std::auto_ptr<VCNWindowsImpl> mImpl;
};

#endif // VCNWINDOWS_H
