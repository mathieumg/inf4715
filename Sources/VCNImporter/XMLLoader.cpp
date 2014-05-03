///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "XMLLoader.h"

#include "XMLLoaderConstants.h"
#include "VCNUtils/Macros.h"
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Constants.h"
#include "VCNResources/Resource.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <atlbase.h>  //for CComVariant type

VCNBool VCNXMLLoader::mMSXMLInstalled = false;
XMLDocPtr VCNXMLLoader::mCurrentDoc = NULL;
XMLDocPtr VCNXMLLoader::pXMLDoc = NULL;


//-------------------------------------------------------------
/// Check if MS XML is installed
//-------------------------------------------------------------
VCNBool VCNXMLLoader::isMSXMLInstalled()
{
  HKEY hKey;
  DWORD retCode;

  _TCHAR   szTemp[_MAX_PATH];         // multipurpose buffer on stack
  DWORD    dwLen;                     // buffer size

  retCode = RegOpenKeyEx(HKEY_CLASSES_ROOT, 
    _T("CLSID\\{88d96a05-f192-11d4-a65f-0040963251e5}\\InProcServer32"), 
    0, 
    KEY_QUERY_VALUE, 
    &hKey);

  if (retCode != ERROR_SUCCESS) return false;

  retCode = RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)szTemp, &(dwLen = sizeof(szTemp)));

  if (retCode != ERROR_SUCCESS) return false;

  RegCloseKey(hKey);

  double dVer;
  int i;

  for( i = (int)_tcslen(szTemp); i >= 0; --i )
  {
    if (szTemp[i] == _T('\\'))
      break;
  }

  if (_stscanf(szTemp + i + 1, _T("msxml%lf"), &dVer) == 0 || dVer < 6.0)
  {
    VCN_ASSERT_FAIL( _T("\nError: MSXML 6.0 is not installed. Exiting\n[%s].\n"), szTemp );
    return false;
  }

  return true;
}



//-------------------------------------------------------------
/// Helper function to create a VT_BSTR variant from a null terminated string. 
//-------------------------------------------------------------
HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
    HRESULT hr = S_OK;
    BSTR bstr = SysAllocString(wszValue);
    VCN_ASSERT( bstr != 0 );
    
    V_VT(&Variant)   = VT_BSTR;
    V_BSTR(&Variant) = bstr;

    return hr;
}

//-------------------------------------------------------------
///  Load the XML document using DOM
//-------------------------------------------------------------
XMLDocPtr VCNXMLLoader::LoadDocument( const VCNString& pDocName )
{
  if( !mMSXMLInstalled )
  {
    mMSXMLInstalled = isMSXMLInstalled();
    VCN_ASSERT( mMSXMLInstalled && "MS XML not installed." );

    // Now that our XML parser is installed, init COM
    CoInitialize(NULL);
  }

  VARIANT_BOOL varLoadResult((bool)FALSE);

  //XMLDocPtr pXMLDoc = NULL;

  //  Create MSXML DOM object
  HRESULT hr = CoCreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pXMLDoc) );
  VCN_ASSERT( hr == S_OK && "Could not load XML file!" );

  //  Load the document synchronously
  pXMLDoc->put_async( VARIANT_FALSE );

  //  Load the XML document
  VARIANT varFileName;
    VariantInit( &varFileName );
  VariantFromString( pDocName.c_str(), varFileName );
  pXMLDoc->load( varFileName, &varLoadResult );

  return pXMLDoc;
}

//-------------------------------------------------------------
///  Opens the document and returns the root.
//-------------------------------------------------------------
XMLElementPtr VCNXMLLoader::LoadDocumentRoot( const VCNString& pDocName )
{
  // Open the XML document
  mCurrentDoc = LoadDocument( pDocName );
  VCN_ASSERT( mCurrentDoc != NULL && "Could not load XML file!" );

  // Lets fetch the root element of the doc
  XMLElementPtr pRootElem = NULL;
  mCurrentDoc->get_documentElement(&pRootElem);

  return pRootElem;
}



//-------------------------------------------------------------
///  Retrieve the node tag name
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetNodeTagName( XMLElementPtr element, VCNString& value )
{
	// First things first, lets take down the version
	BSTR varValue = NULL;
	element->get_tagName(&varValue);
	if( varValue )
	{
		// Store it in the string
		value = VCN_UNICODE_TO_TCHAR(varValue);
		::SysFreeString(varValue);
		return true;
	}
	return false;
}

//-------------------------------------------------------------
///  Retrieve the node name
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetNodeName( XMLNodePtr element, VCNString& value )
{
	// First things first, lets take down the version
	BSTR varValue = NULL;
	element->get_nodeName(&varValue);
	if( varValue )
	{
		// Store it in the string
		value = VCN_UNICODE_TO_TCHAR(varValue);
		::SysFreeString(varValue);
		return true;
	}
	return false;
}

//-------------------------------------------------------------
///  Retrieve an INT attribute from an element
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeInt( XMLElementPtr element, const VCNTChar* name, VCNInt& value )
{
  // First things first, lets take down the version
  VARIANT varValue;
  element->getAttribute( (VCNTChar*)name, &varValue );
  if( varValue.vt != VT_NULL )
  {
    // Convert to int
    value = _wtoi( (wchar_t*)varValue.bstrVal );

    // value has changes
    return true;
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve a FLOAT attribute from an element
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeFloat( XMLElementPtr element, const VCNTChar* name, VCNFloat& value )
{
  // First things first, lets take down the version
  VARIANT varValue;
  element->getAttribute( (VCNTChar*)name, &varValue );

  if( varValue.vt != VT_NULL )
  {
    // Convert to float
    value = (float)_wtof( (wchar_t*)varValue.bstrVal );

    // value has changes
    return true;
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve an STRING attribute from an element
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeString( XMLElementPtr element, const VCNTChar* name, VCNString& value )
{
  // First things first, lets take down the version
  VARIANT varValue;
  element->getAttribute( (VCNTChar*)name, &varValue );
  if( varValue.vt != VT_NULL )
  {
    // Store it in the string
    value = VCN_UNICODE_TO_TCHAR(varValue.bstrVal);

    // value has changes
    return true;
  }

  // value has not changed
  return false;
}


//-------------------------------------------------------------
///  Retrieve an INT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Convert to int
        value = _wtoi( (wchar_t*)varValue.bstrVal );

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve an UINT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeUInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNUInt& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Convert to uint
        int temp = _wtoi( (wchar_t*)varValue.bstrVal );

        // Make sure it's positive!
        VCN_ASSERT( temp >= 0 );

        // Convert it (i know the limitation this implies!)
        value = (VCNUInt)temp;

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve an VCNUShort attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeUShort( XMLNodePtr nodePtr, const VCNTChar* name, VCNUShort& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;

  nodePtr->get_attributes(&nodeMap);

  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Convert to uint
        int temp = _wtoi( (wchar_t*)varValue.bstrVal );

        // Make sure it's positive!
        VCN_ASSERT( temp >= 0 );

        // Make sure it fits in a VCNUShort
        VCN_ASSERT( temp < kMaxUShort && "Value too big for a VCNUShort!" );

        // Convert it (i know the limitation this implies!)
        value = (VCNUShort)temp;

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve a FLOAT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeFloat( XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Convert to float
        value = (float)_wtof( (wchar_t*)varValue.bstrVal );

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;
}

//-------------------------------------------------------------
///  Retrieve an STRING attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeString( XMLNodePtr nodePtr, const VCNTChar* name, VCNString& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Store it in the string
        value = VCN_UNICODE_TO_TCHAR(varValue.bstrVal);

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;

}

//-------------------------------------------------------------
///  Retrieve an STRING attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::GetAttributeBool( XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Read that value
      VARIANT varValue;
      attrNode->get_nodeValue( &varValue );
      if( varValue.vt != VT_NULL )
      {
        // Convert to int
        int i = _wtoi( (wchar_t*)varValue.bstrVal );
        value = (i!=0);

        // value has changes
        return true;
      }
    }
  }

  // value has not changed
  return false;
}


//-------------------------------------------------------------
///  Load the base properties of a resource (name and version)
//-------------------------------------------------------------
VCNBool VCNXMLLoader::LoadResourceBaseProperties( XMLElementPtr element, VCNResource* resource )
{
  // Read the name of the anim and store it
  VCNString tmpString = VCNTXT("NoName");
  if( GetAttributeString(element, kAttrResourceName, tmpString) )
  {
    resource->SetName( tmpString );
  }

  // Read the version of the anims and store it
  VCNFloat tmpFloat = -1.0f;
  if( GetAttributeFloat(element, kAttrResourceVersion, tmpFloat) )
  {
    resource->SetVersion( tmpFloat );
  }

  return true;
}


//-------------------------------------------------------------
///  Set a INT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeInt( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      CComVariant varValue = value;
      attrNode->put_nodeValue( varValue );

      // Save XML document
      VARIANT varFileName;
      VariantInit( &varFileName );
      VariantFromString( filename.c_str(), varFileName );
      
      HRESULT hr = pXMLDoc->save( varFileName );
      VCN_ASSERT( hr == S_OK && "COuld not save XML file!" );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
///  Set a FLOAT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeFloat( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      CComVariant varValue = value;
      attrNode->put_nodeValue( varValue );

      // Save XML document
      VARIANT varFileName;
      VariantInit( &varFileName );
      VariantFromString( filename.c_str(), varFileName );

      HRESULT hr = pXMLDoc->save( varFileName );
      VCN_ASSERT( hr == S_OK && "Could not save XML file!" );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
///  Set a BOOL attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeBool( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      VCNInt temp;
      if(value == true)
        temp = 1;
      else
        temp = 0;

      CComVariant varValue = temp;
      attrNode->put_nodeValue( varValue );

      // Save XML document
      VARIANT varFileName;
      VariantInit( &varFileName );
      VariantFromString( filename.c_str(), varFileName );

      HRESULT hr = pXMLDoc->save( varFileName );
      VCN_ASSERT( hr == S_OK && "COuld not save XML file!" );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
///  Set a INT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      CComVariant varValue = value;
      attrNode->put_nodeValue( varValue );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
///  Set a FLOAT attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeFloat( XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      CComVariant varValue = value;
      attrNode->put_nodeValue( varValue );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
///  Set a BOOL attribute from a node ptr
//-------------------------------------------------------------
VCNBool VCNXMLLoader::SetAttributeBool( XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value )
{
  // Get a map of the attributes
  XMLNodeMapPtr nodeMap;
  nodePtr->get_attributes(&nodeMap);
  if( nodeMap != NULL )
  {
    // Get the attribute's node
    XMLNodePtr attrNode = 0;
    nodeMap->getNamedItem( (VCNTChar*)name, &attrNode );
    if( attrNode != NULL )
    {
      // Set that value
      VCNInt temp;
      if(value == true)
        temp = 1;
      else
        temp = 0;

      CComVariant varValue = temp;
      attrNode->put_nodeValue( varValue );

      // Value has changed
      return true;
    }
  }

  // Value has not changed
  return false;
}

//-------------------------------------------------------------
void VCNXMLLoader::ReleaseDocument()
{
  mCurrentDoc.Release();
}

VCNBool VCNXMLLoader::SaveDocument( XMLDocPtr spDocOutput, const VCNString& filename )
{
  // Local variables and initializations
  HRESULT hResult = S_OK;
  const TCHAR *name;
  name = filename.c_str();

  MSXML2::IXMLDOMDocument2Ptr loadXML;
  hResult = loadXML.CreateInstance(__uuidof(MSXML2::DOMDocument60));
  if(FAILED(hResult))
  {
    return false;
  }

  //We need to load the style sheet which will be used to indent the XML properly.
  VARIANT_BOOL success;
  loadXML->load(variant_t(_T("StyleSheet.xsl")), &success);
  if( success == VARIANT_FALSE )
  {
    return false;
  }


  //Create the final document which will be indented properly
  MSXML2::IXMLDOMDocument2Ptr pXMLFormattedDoc;
  hResult = pXMLFormattedDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
  if(FAILED(hResult))
  {
    return false;
  }

  CComPtr<IDispatch> pDispatch;
  hResult = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
  if(FAILED(hResult))
  {
    return false;
  }

  //Apply the transformation to format the final document    
  _variant_t    vtOutObject;
  vtOutObject.vt = VT_DISPATCH;
  vtOutObject.pdispVal = pDispatch;
  vtOutObject.pdispVal->AddRef();
  hResult = spDocOutput->transformNodeToObject(loadXML, vtOutObject);

  //By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8

  // <?xml version="1.0" encoding="UTF-8"?>
  MSXML2::IXMLDOMNodePtr pXMLFirstChild;
  pXMLFormattedDoc->get_firstChild(&pXMLFirstChild);
  // A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
  MSXML2::IXMLDOMNamedNodeMapPtr pXMLAttributeMap;
  pXMLFirstChild->get_attributes(&pXMLAttributeMap);
  MSXML2::IXMLDOMNodePtr pXMLEncodNode;
  pXMLAttributeMap->getNamedItem(_T("encoding"), &pXMLEncodNode);    
  pXMLEncodNode->put_nodeValue(_variant_t(("UTF-8")));    //encoding = UTF-8

  //  Save the output XML Document
  hResult = pXMLFormattedDoc->save(_variant_t(name));

  if (FAILED(hResult))
  {
    //cerr << "Failed to save document" << endl;
    return false;
  }

  return true;
}
