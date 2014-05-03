
#include "VCN3dsExporterProgressbarNotifier.h"
#include "VCN3dsExporterBridge\VCN3dsExporterBridge.h"




VCN3dsExporterProgressbarNotifier::VCN3dsExporterProgressbarNotifier( const std::string& name, int totalCount )
{
	mTotalCount = totalCount;
	mName = name;
	mDoneCount = 0;
	VCN3dsExporterBridge::VCN3dsExporterBridgeManager::EnableProgressWindow(mName);
}

VCN3dsExporterProgressbarNotifier::~VCN3dsExporterProgressbarNotifier()
{
	VCN3dsExporterBridge::VCN3dsExporterBridgeManager::DisableProgressWindow();
}

void VCN3dsExporterProgressbarNotifier::AddOneEntry()
{
	AddEntry(1);
}

void VCN3dsExporterProgressbarNotifier::AddEntry( int count )
{
	mDoneCount += count;
	UpdateUI();
}

void VCN3dsExporterProgressbarNotifier::UpdateUI()
{
	VCN3dsExporterBridge::VCN3dsExporterBridgeManager::SetProgressWindowValue(100.0f * (float)mDoneCount / (float)mTotalCount);
}
