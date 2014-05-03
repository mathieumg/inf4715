///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Base class to read XML files (Windows Specific)
///

#ifndef VICUNA_XML_LOADER
#define VICUNA_XML_LOADER

#pragma once

#include "VCNUtils/Types.h"

// Lets define ourselves a few shortcuts
#define XMLDocPtr MSXML2::IXMLDOMDocument2Ptr
#define XMLDocBasePtr MSXML2::IXMLDOMDocumentPtr
#define XMLNodePtr MSXML2::IXMLDOMNodePtr
#define XMLElementPtr MSXML2::IXMLDOMElementPtr
#define XMLNodeMapPtr MSXML2::IXMLDOMNamedNodeMapPtr
#define XMLNodeListPtr MSXML2::IXMLDOMNodeListPtr

class VCNResource;

class VCNXMLLoader
{
protected:
  static VCNBool isMSXMLInstalled();
  static XMLDocPtr LoadDocument( const VCNString& pDocName );
  static VCNBool   SaveDocument( XMLDocPtr spDocOutput, const VCNString& filename );
  static XMLElementPtr LoadDocumentRoot( const VCNString& pDocName );
  
  static void ReleaseDocument();

  // Load base resource properties
  static VCNBool LoadResourceBaseProperties( XMLElementPtr element, VCNResource* resource );

  static VCNBool GetNodeName(XMLNodePtr element, VCNString& value);
  static VCNBool GetNodeTagName(XMLElementPtr element, VCNString& value);

  // Fetch attributes from elements
  static VCNBool GetAttributeInt( XMLElementPtr element, const VCNTChar* name, VCNInt& value );
  static VCNBool GetAttributeFloat( XMLElementPtr element, const VCNTChar* name, VCNFloat& value );
  static VCNBool GetAttributeString( XMLElementPtr element, const VCNTChar* name, VCNString& value );

  // Fetch attributes from nodes
  static VCNBool GetAttributeBool( XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value );
  static VCNBool GetAttributeUShort( XMLNodePtr nodePtr, const VCNTChar* name, VCNUShort& value );
  static VCNBool GetAttributeInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value );
  static VCNBool GetAttributeUInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNUInt& value );
  static VCNBool GetAttributeFloat( XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value );
  static VCNBool GetAttributeString( XMLNodePtr nodePtr, const VCNTChar* name, VCNString& value );

  // Save node modifications in XML file
  static VCNBool SetAttributeInt( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value );
  static VCNBool SetAttributeFloat( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value );
  static VCNBool SetAttributeBool( const VCNString& filename, XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value );

  // Save node modifications in memory
  static VCNBool SetAttributeInt( XMLNodePtr nodePtr, const VCNTChar* name, VCNInt& value );
  static VCNBool SetAttributeFloat( XMLNodePtr nodePtr, const VCNTChar* name, VCNFloat& value );
  static VCNBool SetAttributeBool( XMLNodePtr nodePtr, const VCNTChar* name, VCNBool& value );

protected:
  // This utility class should not be instanced.
  VCNXMLLoader() {}
  ~VCNXMLLoader() {}

  // Check for XML installation only once
  static VCNBool mMSXMLInstalled;
  static XMLDocPtr mCurrentDoc;

private:
  static XMLDocPtr pXMLDoc;
};


#endif
