

#pragma once
#include <string>
#include <msclr/marshal_cppstd.h>


namespace VCN3dsExporterBridge
{

	public ref class VCN3dsExporterBridgeHelpers
	{
	public:
		static std::string VCN3dsExporterBridgeHelpers::ConvertManagedString( System::String^ str )
		{
			std::string ret = msclr::interop::marshal_as<std::string>(str);
			return ret;
		}
	};

}


