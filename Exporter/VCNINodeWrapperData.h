


#ifndef VCN_INODE_WRAPPER_DATA
#define VCN_INODE_WRAPPER_DATA

#include <vector>
#include <map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "VCNUtils/Vector.h"
#include "VCNLua/LuaTrigger.h"



class VCNINodeWrapper;

typedef unsigned int VCNID;

// Supported editor types
enum VCNAttributeType
{
	ATTR_UNDEFINED	        = 0,
	ATTR_ID					= 1,
	ATTR_INTEGER			= 2, 
	ATTR_UNSIGNED_INTEGER	= 3, 
	ATTR_FLOAT              = 4,
	ATTR_DOUBLE             = 5,
	ATTR_STRING             = 6,
	ATTR_CHAR               = 7,
	ATTR_VECTOR2            = 8,
	ATTR_VECTOR3            = 9,
	ATTR_VECTOR4            = 10,
	ATTR_BOOL               = 11,
	ATTR_LUA_TRIGGER        = 12
};

// The key is represented by a pair <type, name>
struct VCNINodeAttributeKey
{
	VCNAttributeType type;
	std::string name;
	bool operator < (const VCNINodeAttributeKey& x) const
	{
		if (type != x.type)
		{
			return type < x.type;
		}
		else
		{
			return name < x.name;
		}
	}
};

// Wrapper for attributes (responsible to create and destroy the objects)
class VCNINodeAttributeValue
{
public:

	static VCNINodeAttributeValue BuildVCNINodeAttributeFromType(VCNAttributeType type, const boost::property_tree::basic_ptree<std::string, std::string, std::less<std::string>>& tree);


	//VCNINodeAttributeValue(VCNID val);
	VCNINodeAttributeValue();
	VCNINodeAttributeValue(int val);
	VCNINodeAttributeValue(unsigned int val);
	VCNINodeAttributeValue(float val);
	VCNINodeAttributeValue(double val);
	VCNINodeAttributeValue(const std::string& val);
	VCNINodeAttributeValue(const LuaTrigger& val);
	VCNINodeAttributeValue(char val);
	VCNINodeAttributeValue(const Vector2& val);
	VCNINodeAttributeValue(const Vector3& val);
	VCNINodeAttributeValue(const Vector4& val);
	VCNINodeAttributeValue(const VCNINodeAttributeValue& source);
	~VCNINodeAttributeValue();

	void AddVCNINodeAttributeFromType(boost::property_tree::basic_ptree<std::string, std::string, std::less<std::string>>& tree) const;

	std::string ToString() const;

	VCNINodeAttributeValue& operator= (const VCNINodeAttributeValue & other);
	bool operator== (const VCNINodeAttributeValue& other);

	void SetValue(int val);
	void SetValue(unsigned int val);
	void SetValue(float val);
	void SetValue(double val);
	void SetValue(const std::string& val);
	void SetValue(const LuaTrigger& val);
	void SetValue(char val);
	void SetValue(const Vector2& val);
	void SetValue(const Vector3& val);
	void SetValue(const Vector4& val);
	void SetValue(bool val);

	inline const int* GetValueAsInt() const 
	{
		return (int*) mData;
	}

	inline const unsigned int* GetValueAsUnsignedInt() const 
	{
		return (unsigned int*) mData;
	}

	inline const float* GetValueAsFloat() const 
	{
		return (float*) mData;
	}

	inline const double* GetValueAsDouble() const 
	{
		return (double*) mData;
	}

	inline const std::string* GetValueAsString() const 
	{
		return (std::string*) mData;
	}

	inline const char* GetValueAsChar() const 
	{
		return (char*) mData;
	}

	inline const bool* GetValueAsBool() const 
	{
		return (bool*) mData;
	}

	inline const Vector2* GetValueAsVector2() const 
	{
		return (Vector2*) mData;
	}

	inline const Vector3* GetValueAsVector3() const 
	{
		return (Vector3*) mData;
	}

	inline const Vector4* GetValueAsVector4() const 
	{
		return (Vector4*) mData;
	}

	inline const LuaTrigger* GetValueAsLuaTrigger() const 
	{
		return (LuaTrigger*) mData;
	}

	inline const void* GetValueAsPtr() const 
	{
		return mData;
	}

	inline const VCNAttributeType GetType() const
	{
		return mType;
	}

private:
	void DestroyData();

	VCNAttributeType mType;
	void* mData; // Ptr to the data
};



class VCNINodeComponent
{
public:
	VCNINodeComponent();
	VCNINodeComponent(VCNINodeWrapper* nodeWrapper);
	VCNINodeComponent(VCNINodeWrapper* nodeWrapper, const std::string& json);
	VCNINodeComponent(const VCNINodeComponent& source);
	~VCNINodeComponent();

	std::string BuildJsonRepresentation() const;

	inline const std::string& GetName() const { return mName; }
	inline void SetName(const std::string& val) { mName = val; MakeNodeWrapperDirty(); }

	void MakeNodeWrapperDirty();

	bool AddOrEditAttribute(VCNAttributeType type, const std::string& name, const VCNINodeAttributeValue& value);

	VCNINodeComponent& operator= (const VCNINodeComponent & other);


	inline std::map<VCNINodeAttributeKey, VCNINodeAttributeValue>::iterator GetAttributesBegin() { return mAttributes.begin(); }
	inline std::map<VCNINodeAttributeKey, VCNINodeAttributeValue>::iterator GetAttributesEnd() { return mAttributes.end(); }
private:
	std::string mName;
	std::map<VCNINodeAttributeKey, VCNINodeAttributeValue> mAttributes;
protected:
	VCNINodeWrapper* mNodeWrapper;

};



typedef std::vector<VCNINodeComponent> VCNINodeWrapperData;




#endif