

#pragma managed
#pragma once
#include <string>
#include <msclr/marshal_cppstd.h>
#include "VCN3dsExporterBridgeHelpers.h"
#include "..\VCNINodeWrapperData.h"

namespace VCN3dsExporterBridge
{
	// NOTE: THESE ARE NOT REAL VISITORS BECAUSE WE CAN'T MAKE AN "ACCEPT_VISITOR" METHOD FOR UNMANAGED CLASS VCNINodeAttributeValue
	public ref class DataVisitor abstract
	{
	public:
		virtual void Visit(VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd) abstract;
	};


	// Int
	public ref class DataVisitorInt : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((int)objectToAdd);
		}

	};

	// UInt
	public ref class DataVisitorUInt : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((unsigned int)objectToAdd);
		}
	};

	// Float
	public ref class DataVisitorFloat : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((float)objectToAdd);
		}
	};

	// Double
	public ref class DataVisitorDouble : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((double)objectToAdd);
		}
	};

	// Char
	public ref class DataVisitorChar : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((char)objectToAdd);
		}
	};

	// Bool
	public ref class DataVisitorBool : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue((bool)objectToAdd);
		}
	};

	// String
	public ref class DataVisitorString : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			valueToModify.SetValue(VCN3dsExporterBridgeHelpers::ConvertManagedString((System::String^)objectToAdd));
		}
	};

	// Vector2
	public ref class DataVisitorVector2 : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			VCNNodesCSharp::Vector2^ v2m = (VCNNodesCSharp::Vector2^) objectToAdd;
			::Vector2 v2;
			v2.x = v2m->x;
			v2.y = v2m->y;
			valueToModify.SetValue(v2);
		}
	};

	// Vector3
	public ref class DataVisitorVector3 : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			VCNNodesCSharp::Vector3^ v3m = (VCNNodesCSharp::Vector3^) objectToAdd;
			::Vector3 v3;
			v3.x = v3m->x;
			v3.y = v3m->y;
			v3.z = v3m->z;
			valueToModify.SetValue(v3);
		}
	};

	// Vector4
	public ref class DataVisitorVector4 : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			VCNNodesCSharp::Vector4^ v4m = (VCNNodesCSharp::Vector4^) objectToAdd;
			::Vector4 v4;
			v4.x = v4m->x;
			v4.y = v4m->y;
			v4.z = v4m->z;
			v4.w = v4m->w;
			valueToModify.SetValue(v4);
		}
	};

	// LuaTrigger
	public ref class DataVisitorLuaTrigger : public DataVisitor
	{
	public:
		virtual void Visit( VCNINodeAttributeValue& valueToModify, System::Object^ objectToAdd ) override
		{
			VCNNodesCSharp::LuaTrigger^ v4m = (VCNNodesCSharp::LuaTrigger^) objectToAdd;
			::LuaTrigger t;
			t.SetTriggerName(VCN3dsExporterBridgeHelpers::ConvertManagedString((System::String^)v4m->TriggerName));
			t.SetTriggerType(VCN3dsExporterBridgeHelpers::ConvertManagedString((System::String^)v4m->TriggerType));
			valueToModify.SetValue(t);
		}
	};


}
