

#pragma once


#include <string>



class VCN3dsExporterProgressbarNotifier
{
public:
	VCN3dsExporterProgressbarNotifier(const std::string& name, int totalCount);
	~VCN3dsExporterProgressbarNotifier();

	void AddOneEntry();
	void AddEntry(int count);

private:
	void UpdateUI();

private:
	int mTotalCount;
	int mDoneCount;
	std::string mName;
};



