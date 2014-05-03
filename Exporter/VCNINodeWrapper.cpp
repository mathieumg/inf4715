
#include "VCNINodeWrapper.h"
#include "VCN3dsExporter.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "stdlib.h"

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


VCNINodeComponent::VCNINodeComponent()
{
	mNodeWrapper = NULL;
}


VCNINodeComponent::VCNINodeComponent(VCNINodeWrapper* nodeWrapper)
{
	mNodeWrapper = nodeWrapper;
}


VCNINodeComponent::VCNINodeComponent( VCNINodeWrapper* nodeWrapper, const std::string& json )
{
	mNodeWrapper = nodeWrapper;

	// Parse json
	ptree pt;
	stringstream ss; ss << json;
	read_json(ss, pt);

	// Get back the data from the ptree
	mName = pt.get<std::string>("Name");
	BOOST_FOREACH(ptree::value_type &v,pt.get_child("Attributes"))
	{
		VCNINodeAttributeKey key;
		key.name = v.second.get<std::string>("Name");
		key.type = (VCNAttributeType) v.second.get<int>("Type");

		VCNINodeAttributeValue newVal = VCNINodeAttributeValue::BuildVCNINodeAttributeFromType(key.type, v.second);

		mAttributes[key] = newVal;
	}

}

VCNINodeComponent::VCNINodeComponent( const VCNINodeComponent& other )
{
	mName = other.GetName();
	mNodeWrapper = other.mNodeWrapper;
	for (auto it = other.mAttributes.begin(); it != other.mAttributes.end(); it++)
	{
		mAttributes[it->first] = it->second;
	}
}

VCNINodeComponent::~VCNINodeComponent()
{

}



std::string VCNINodeComponent::BuildJsonRepresentation() const
{
	ptree pt;
	ptree attributesTree;
	std::vector<ptree> aTreeList;

	// Component's attributes
	pt.put ("Name", mName);
	int c = 0;
	// Build attributes list
	for (std::map<VCNINodeAttributeKey, VCNINodeAttributeValue>::const_iterator it = mAttributes.begin(); it != mAttributes.end(); it++)
	{

		ptree aTree;
		aTree.put("Name", it->first.name);
		aTree.put("Type", it->first.type);
		it->second.AddVCNINodeAttributeFromType(aTree);
		aTreeList.push_back(aTree);
		
		attributesTree.push_back(std::make_pair("", aTreeList[c]));
		c++;
	}

	// Add attributes to the main tree
	pt.add_child("Attributes", attributesTree);

	// Output buffer
	std::ostringstream buf; 
	write_json (buf, pt, false);
	std::string json = buf.str();
	const char* test = json.c_str();
	return json;
}



bool VCNINodeComponent::AddOrEditAttribute( VCNAttributeType type, const std::string& name, const VCNINodeAttributeValue& value )
{
	VCNINodeAttributeKey key;
	key.name = name;
	key.type = type;

	auto it = mAttributes.find(key);
	if (it == mAttributes.end())
	{
		mAttributes[key] = value;
		return true;
	}
	else
	{
		if ((it->second) == value)
		{
			MakeNodeWrapperDirty();
		}
		return false;
	}
}


VCNINodeComponent& VCNINodeComponent::operator=( const VCNINodeComponent & other )
{
	mName = other.GetName();
	mNodeWrapper = other.mNodeWrapper;
	for (auto it = other.mAttributes.begin(); it != other.mAttributes.end(); it++)
	{
		mAttributes[it->first] = it->second;
	}
	return *this;
}











VCNINodeWrapper::VCNINodeWrapper( INode& inode )
	:	mNode(inode),
		mIsDirty(false),
		mName("Unknown node")
{


	LoadFromINode();
}

VCNINodeWrapper::~VCNINodeWrapper()
{

}





void VCNINodeWrapper::LoadFromINode()
{
	// Load object from the AppData contained in the INode
	AppDataChunk* iNodeWrapperData = mNode.GetAppDataChunk(VCN3dsExporter_CLASS_ID, UTILITY_CLASS_ID, 0);
	if (iNodeWrapperData)
	{
		int componentCount = 0;
		if (iNodeWrapperData->data)
		{
			VCNINodeWrapperDataChunk* d = (VCNINodeWrapperDataChunk*)iNodeWrapperData->data;
			componentCount = d->componentCount;
			if (componentCount > 0)
			{
				mComponents.reserve(componentCount);
			}
			mName = std::string(d->name);
		}

		for (int i=1; i<=componentCount; i++)
		{
			AppDataChunk* componentJsonStr = mNode.GetAppDataChunk(VCN3dsExporter_CLASS_ID, UTILITY_CLASS_ID, i);
			if (componentJsonStr)
			{
				std::string b((char*)componentJsonStr->data);
				VCNINodeComponent component(this, b);
				AddCompnent(component);
			}
		}
	}
	mIsDirty = false;
}


void VCNINodeWrapper::SaveFromINode()
{
	// Empty the previous data before
	int removeCounter = 0;
	BOOL foundSomething = TRUE;
	while (foundSomething)
	{
		foundSomething = mNode.RemoveAppDataChunk(VCN3dsExporter_CLASS_ID, UTILITY_CLASS_ID, removeCounter);
		removeCounter++;
	}

	VCNINodeWrapperDataChunk* d = (VCNINodeWrapperDataChunk*) MAX_malloc(sizeof(VCNINodeWrapperDataChunk));
	strcpy(d->name, mName.c_str());
 	d->componentCount = mComponents.size();
	int sizeS = sizeof(VCNINodeWrapperDataChunk);
	// Set the first data chunk. Will call free on the data passed
	mNode.AddAppDataChunk(VCN3dsExporter_CLASS_ID, UTILITY_CLASS_ID, 0, sizeS, d);

	// Set components dataChunks
	for (unsigned int i=1; i<=mComponents.size(); i++)
	{
		std::string buf = mComponents[i-1].BuildJsonRepresentation();
		int len = buf.length() + 10;
		char* b = (char*) MAX_malloc(len);
		strcpy_s(b, len, buf.c_str());
		// Will call free on the data
		mNode.AddAppDataChunk(VCN3dsExporter_CLASS_ID, UTILITY_CLASS_ID, i, len, b);
	}

}


bool VCNINodeWrapper::AddCompnent( const VCNINodeComponent& component )
{
	VCNINodeComponent testC = component;
	mComponents.push_back(testC);
	MakeDirty();
	return true;
}




//////////////////////////////////////////////////


VCNINodeAttributeValue::VCNINodeAttributeValue()
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
}


VCNINodeAttributeValue::VCNINodeAttributeValue( int val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( unsigned int val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( float val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( double val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const std::string& val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( char val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const Vector2& val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const Vector3& val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const Vector4& val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const LuaTrigger& val )
{
	mData = NULL;
	mType = ATTR_UNDEFINED;
	SetValue(val);
}

VCNINodeAttributeValue::~VCNINodeAttributeValue()
{
	DestroyData();
}





void VCNINodeAttributeValue::SetValue( int val )
{
	DestroyData();
	mData = new int(val);
	mType = ATTR_INTEGER;
}

void VCNINodeAttributeValue::SetValue( unsigned int val )
{
	DestroyData();
	mData = new unsigned int(val);
	mType = ATTR_UNSIGNED_INTEGER;
}

void VCNINodeAttributeValue::SetValue( float val )
{
	DestroyData();
	mData = new float(val);
	mType = ATTR_FLOAT;
}

void VCNINodeAttributeValue::SetValue( double val )
{
	DestroyData();
	mData = new double(val);
	mType = ATTR_DOUBLE;
}

void VCNINodeAttributeValue::SetValue( const std::string& val )
{
	DestroyData();
	std::string* s = new string();
	s->append(val);
	mData = s;
	mType = ATTR_STRING;
}

void VCNINodeAttributeValue::SetValue( const LuaTrigger& val )
{
	DestroyData();
	LuaTrigger* trigger = new LuaTrigger();
	*trigger = val;
	mData = trigger;
	mType = ATTR_LUA_TRIGGER;
}

void VCNINodeAttributeValue::SetValue( char val )
{
	DestroyData();
	mData = new char(val);
	mType = ATTR_CHAR;
}

void VCNINodeAttributeValue::SetValue( const Vector2& val )
{
	DestroyData();
	mData = new Vector2(val);
	mType = ATTR_VECTOR2;
}

void VCNINodeAttributeValue::SetValue( const Vector3& val )
{
	DestroyData();
	mData = new Vector3(val);
	mType = ATTR_VECTOR3;
}

void VCNINodeAttributeValue::SetValue( const Vector4& val )
{
	DestroyData();
	mData = new Vector4(val);
	mType = ATTR_VECTOR4;
}

void VCNINodeAttributeValue::SetValue( bool val )
{
	DestroyData();
	mData = new bool(val);
	mType = ATTR_BOOL;
}


void VCNINodeAttributeValue::DestroyData()
{
	if (mData == NULL)
	{
		return;
	}

	VCNID* pID          = (VCNID*) mData;
	int* pInt           = (int*) mData;
	unsigned int* pUInt = (unsigned int*) mData;
	float* pFloat       = (float*) mData;
	double* pDouble     = (double*) mData;
	string* pString     = (string*) mData;
	char* pChar         = (char*) mData;
	Vector2* pVector2   = (Vector2*) mData;
	Vector3* pVector3   = (Vector3*) mData;
	Vector4* pVector4   = (Vector4*) mData;
	bool* pBool         = (bool*) mData;
	LuaTrigger* pLuaTrigger = (LuaTrigger*) mData;

	switch (mType)
	{
	case ATTR_ID:
		delete pID;
		break;
	case ATTR_INTEGER:
		delete pInt;
		break;
	case ATTR_UNSIGNED_INTEGER:
		delete pUInt;
		break;
	case ATTR_FLOAT:
		delete pFloat;
		break;
	case ATTR_DOUBLE:
		delete pDouble;
		break;
	case ATTR_STRING:
		delete pString;
		break;
	case ATTR_CHAR:
		delete pChar;
		break;
	case ATTR_VECTOR2:
		delete pVector2;
		break;
	case ATTR_VECTOR3:
		delete pVector3;
		break;
	case ATTR_VECTOR4:
		delete pVector4;
		break;
	case ATTR_BOOL:
		delete pBool;
		break;
	case ATTR_LUA_TRIGGER:
		delete pLuaTrigger;
		break;
	default:
		break;
	}

	mData = NULL;
}





VCNINodeAttributeValue VCNINodeAttributeValue::BuildVCNINodeAttributeFromType( VCNAttributeType type, const boost::property_tree::basic_ptree<std::string, std::string, std::less<std::string>>& tree )
{
	switch (type)
	{
	case ATTR_ID:
		return VCNINodeAttributeValue(tree.get<int>("Value"));
	case ATTR_INTEGER:
		return VCNINodeAttributeValue(tree.get<int>("Value"));
	case ATTR_UNSIGNED_INTEGER:
		return VCNINodeAttributeValue(tree.get<unsigned int>("Value"));
	case ATTR_FLOAT:
		return VCNINodeAttributeValue(tree.get<float>("Value"));
	case ATTR_DOUBLE:
		return VCNINodeAttributeValue(tree.get<double>("Value"));
	case ATTR_STRING:
		return VCNINodeAttributeValue(tree.get<std::string>("Value"));
	case ATTR_CHAR:
		return VCNINodeAttributeValue(tree.get<char>("Value"));
	case ATTR_VECTOR2:
		return VCNINodeAttributeValue(tree.get<Vector2>("Value"));
	case ATTR_VECTOR3:
		return VCNINodeAttributeValue(tree.get<Vector3>("Value"));
	case ATTR_VECTOR4:
		return VCNINodeAttributeValue(tree.get<Vector4>("Value"));
	case ATTR_BOOL:
		return VCNINodeAttributeValue(tree.get<bool>("Value"));
	case ATTR_LUA_TRIGGER:
		return VCNINodeAttributeValue(tree.get<LuaTrigger>("Value"));
	default:
		break;
	}

	return VCNINodeAttributeValue("");
}



void VCNINodeAttributeValue::AddVCNINodeAttributeFromType( boost::property_tree::basic_ptree<std::string, std::string, std::less<std::string>>& tree ) const
{
	switch (mType)
	{
	case ATTR_ID:
		tree.put("Value", *GetValueAsInt());
		break;
	case ATTR_INTEGER:
		tree.put("Value", *GetValueAsInt());
		break;
	case ATTR_UNSIGNED_INTEGER:
		tree.put("Value", *GetValueAsUnsignedInt());
		break;
	case ATTR_FLOAT:
		tree.put("Value", *GetValueAsFloat());
		break;
	case ATTR_DOUBLE:
		tree.put("Value", *GetValueAsDouble());
		break;
	case ATTR_STRING:
		tree.put("Value", *GetValueAsString());
		break;
	case ATTR_CHAR:
		tree.put("Value", *GetValueAsChar());
		break;
	case ATTR_VECTOR2:
		tree.put("Value", *GetValueAsVector2());
		break;
	case ATTR_VECTOR3:
		tree.put("Value", *GetValueAsVector3());
		break;
	case ATTR_VECTOR4:
		tree.put("Value", *GetValueAsVector4());
		break;
	case ATTR_LUA_TRIGGER:
		tree.put("Value", *GetValueAsLuaTrigger());
		break;
	case ATTR_BOOL:
		tree.put("Value", *GetValueAsBool());
		break;
	default:
		tree.put("Value", "");
		break;
	}

}


VCNINodeAttributeValue & VCNINodeAttributeValue::operator=( const VCNINodeAttributeValue & other )
{
	mType = other.GetType();
	switch (mType)
	{
	case ATTR_ID:
		SetValue(*other.GetValueAsInt());
		break;
	case ATTR_INTEGER:
		SetValue(*other.GetValueAsInt());
		break;
	case ATTR_UNSIGNED_INTEGER:
		SetValue(*other.GetValueAsUnsignedInt());
		break;
	case ATTR_FLOAT:
		SetValue(*other.GetValueAsFloat());
		break;
	case ATTR_DOUBLE:
		SetValue(*other.GetValueAsDouble());
		break;
	case ATTR_STRING:
		SetValue(*other.GetValueAsString());
		break;
	case ATTR_CHAR:
		SetValue(*other.GetValueAsChar());
		break;
	case ATTR_VECTOR2:
		SetValue(*other.GetValueAsVector2());
		break;
	case ATTR_VECTOR3:
		SetValue(*other.GetValueAsVector3());
		break;
	case ATTR_VECTOR4:
		SetValue(*other.GetValueAsVector4());
		break;
	case ATTR_LUA_TRIGGER:
		SetValue(*other.GetValueAsLuaTrigger());
		break;
	case ATTR_BOOL:
		SetValue(*other.GetValueAsBool());
		break;
	default:
		SetValue("");
		break;
	}
	return *this;
}


bool VCNINodeAttributeValue::operator==( const VCNINodeAttributeValue& other )
{
	if (other.GetType() != mType)
	{
		return false;
	}
	else
	{
		switch (mType)
		{
		case ATTR_ID:
			return (*GetValueAsInt() == *other.GetValueAsInt());
		case ATTR_INTEGER:
			return (*GetValueAsInt() == *other.GetValueAsInt());
		case ATTR_UNSIGNED_INTEGER:
			return (*GetValueAsUnsignedInt() == *other.GetValueAsUnsignedInt());
		case ATTR_FLOAT:
			return (*GetValueAsFloat() == *other.GetValueAsFloat());
		case ATTR_DOUBLE:
			return (*GetValueAsDouble() == *other.GetValueAsDouble());
		case ATTR_STRING:
			return (*GetValueAsString() == *other.GetValueAsString());
		case ATTR_CHAR:
			return (*GetValueAsChar() == *other.GetValueAsChar());
		case ATTR_VECTOR2:
			return (*GetValueAsVector2() == *other.GetValueAsVector2());
		case ATTR_VECTOR3:
			return (*GetValueAsVector3() == *other.GetValueAsVector3());
		case ATTR_VECTOR4:
			return (*GetValueAsVector4() == *other.GetValueAsVector4());
		case ATTR_LUA_TRIGGER:
			return (*GetValueAsLuaTrigger() == *other.GetValueAsLuaTrigger());
		case ATTR_BOOL:
			return (*GetValueAsBool() == *other.GetValueAsBool());
		default:
			return false;
			break;
		}
	}
}
