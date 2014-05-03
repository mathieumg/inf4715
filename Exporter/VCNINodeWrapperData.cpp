
#include "VCNINodeWrapperData.h"
#include "VCNINodeWrapper.h"





void VCNINodeComponent::MakeNodeWrapperDirty()
{
	if (mNodeWrapper != NULL) mNodeWrapper->MakeDirty();
}



std::string VCNINodeAttributeValue::ToString() const
{
	std::stringstream ss;
	switch (mType)
	{
	case ATTR_ID:
		ss << *GetValueAsInt();
		break;
	case ATTR_INTEGER:
		ss <<  *GetValueAsInt();
		break;
	case ATTR_UNSIGNED_INTEGER:
		ss << *GetValueAsUnsignedInt();
		break;
	case ATTR_FLOAT:
		ss << *GetValueAsFloat();
		break;
	case ATTR_DOUBLE:
		ss << *GetValueAsDouble();
		break;
	case ATTR_STRING:
		ss << *GetValueAsString();
		break;
	case ATTR_CHAR:
		ss << *GetValueAsChar();
		break;
	case ATTR_VECTOR2:
		ss << *GetValueAsVector2();
		break;
	case ATTR_VECTOR3:
		ss << *GetValueAsVector3();
		break;
	case ATTR_VECTOR4:
		ss << *GetValueAsVector4();
		break;
	case ATTR_BOOL:
		ss << *GetValueAsBool();
	case ATTR_LUA_TRIGGER:
		ss << *GetValueAsLuaTrigger();
		break;
	}
	return ss.str();
}

VCNINodeAttributeValue::VCNINodeAttributeValue( const VCNINodeAttributeValue& source )
{
	mType = ATTR_UNDEFINED;
	mData = NULL;
	switch (source.GetType())
	{
	case ATTR_ID:
		SetValue(*source.GetValueAsInt());
		break;
	case ATTR_INTEGER:
		SetValue(*source.GetValueAsInt());
		break;
	case ATTR_UNSIGNED_INTEGER:
		SetValue(*source.GetValueAsUnsignedInt());
		break;
	case ATTR_FLOAT:
		SetValue(*source.GetValueAsFloat());
		break;
	case ATTR_DOUBLE:
		SetValue(*source.GetValueAsDouble());
		break;
	case ATTR_STRING:
		SetValue(*source.GetValueAsString());
		break;
	case ATTR_CHAR:
		SetValue(*source.GetValueAsChar());
		break;
	case ATTR_VECTOR2:
		SetValue(*source.GetValueAsVector2());
		break;
	case ATTR_VECTOR3:
		SetValue(*source.GetValueAsVector3());
		break;
	case ATTR_VECTOR4:
		SetValue(*source.GetValueAsVector4());
		break;
	case ATTR_BOOL:
		SetValue(*source.GetValueAsBool());
	case ATTR_LUA_TRIGGER:
		SetValue(*source.GetValueAsLuaTrigger());
		break;
	}
}
