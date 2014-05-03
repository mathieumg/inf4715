



#include "inode.h"
#include "VCNINodeWrapperData.h"
#include "VCNLua\LuaTrigger.h"


#ifndef VCN_INODE_WRAPPER
#define VCN_INODE_WRAPPER


// Struct used in the first AppDataChunk
struct VCNINodeWrapperDataChunk
{
	int componentCount;
	char name[256];
};

class VCNINodeComponent;

/*
The AppDataChunk contains this:
	Sub-chunk 0 : Node information (VCNINodeWrapperData)
	Sub-chunk 1..n : Every component (n components). The serialization is done like this: every component has a jsonData that represents the attributes of the component
*/

// This class is a wrapper for INode objects. It should not exit for a long period of time and is build from the INode's AppDataChunk
// Should be constructed everytime the INode is selected or if it's modified
class VCNINodeWrapper
{
public:
	VCNINodeWrapper(INode& inode);
	~VCNINodeWrapper();

	inline const std::string& GetName() const { return mName; }
	inline void SetName(const std::string& val) { mName = val; MakeDirty(); }

	// TODO: limit one instance of each component on the node
	bool AddCompnent(const VCNINodeComponent& component);
	// TODO: Remove component

	inline void MakeDirty() { mIsDirty = true; }

	inline void SaveIfDirty() { if (mIsDirty) SaveFromINode(); }

	inline VCNINodeWrapperData GetData() { return mComponents; }

	inline void SetData(const VCNINodeWrapperData& newData) { mComponents = newData; mIsDirty = true; }
private:
	void LoadFromINode();
	void SaveFromINode();


private:
	std::string mName;
	INode& mNode;
	VCNINodeWrapperData mComponents;
	bool mIsDirty;
};


#endif


