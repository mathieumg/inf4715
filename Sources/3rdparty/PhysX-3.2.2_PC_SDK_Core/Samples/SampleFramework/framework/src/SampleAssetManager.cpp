/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include <SampleAssetManager.h>
#include <SampleAsset.h>
#include <SampleMaterialAsset.h>
#include <SampleTextureAsset.h>
#include <SampleInputAsset.h>

#include <Renderer.h>

#include <stdio.h>
#include "SamplePlatform.h"
#include "ODBlock.h"

// for PsString.h
namespace physx
{
	namespace string
	{}
}
#include <PsString.h>
#include <PsFile.h>

#include "SampleXml.h"
#include "PxFileBuffer.h"

using namespace SampleFramework;

///////////////////////////////////////////////////////////////////////////////

static std::vector<char*>* gSearchPaths = NULL;

void SampleFramework::addSearchPath(const char* path)
{
	if(!path)
		return;

	const PxU32 len = *path ? (PxU32)strlen(path) : 0;
	if(!len)
		return;

	const PxU32 len2 = len+2;
	char* searchPath = new char[len2];
	physx::string::strncpy_s(searchPath, len2, path, len2);
	if(path[len-1] != '/' && path[len-1] != '\\')
	{
		physx::string::strncat_s(searchPath, len2, "/", len2);
	}
	if(!gSearchPaths)
		gSearchPaths = new std::vector<char*>;
	gSearchPaths->push_back(searchPath);
}

void SampleFramework::clearSearchPaths()
{
	if(!gSearchPaths)
		return;
	const PxU32 numSearchPaths = (PxU32)gSearchPaths->size();
	for(PxU32 i=0; i<numSearchPaths; i++)
	{
		delete [] (*gSearchPaths)[i];
	}
	delete gSearchPaths;
	gSearchPaths = NULL;
}

static void FixSeparators(char* path)
{
	for (unsigned i = 0; i < strlen(path); ++i)
		if (path[i] == '\\' || path[i] == '/')
			path[i] = SampleFramework::SamplePlatform::platform()->getPathSeparator()[0];
}

FILE* SampleFramework::findFile(const char* path, bool binary)
{
	if(!gSearchPaths)
		return NULL;

	FILE* file = NULL;
	const PxU32 numSearchPaths = (PxU32)gSearchPaths->size();
	for(PxU32 i=0; i<numSearchPaths; i++)
	{
		const char* prefix = (*gSearchPaths)[i];

		char fullPath[512];
		physx::string::strncpy_s(fullPath, 512, prefix, 512);
		physx::string::strncat_s(fullPath, 512, path,   512);
		FixSeparators(fullPath);
		if(binary)
			Fnd::fopen_s(&file, fullPath, "rb");
		else
		{
			// text files open also for write
			Fnd::fopen_s(&file, fullPath, "r+");
		}
		if(file) break;
	}
	return file;
}

const char* SampleFramework::findPath(const char* path)
{
	if(!gSearchPaths)
		return path;

	static char fullPath[512];

	FILE* file = NULL;
	const PxU32 numSearchPaths = (PxU32)gSearchPaths->size();
	for(PxU32 i=0; i<numSearchPaths; i++)
	{
		const char* prefix = (*gSearchPaths)[i];
		physx::string::strncpy_s(fullPath, 512, prefix, 512);
		physx::string::strncat_s(fullPath, 512, path,   512);
		FixSeparators(fullPath);
		Fnd::fopen_s(&file, fullPath, "rb");
		if(file) break;
	}
	if(file)
	{
		fclose(file);
		return fullPath;
	}
	else
		return path;
}


///////////////////////////////////////////////////////////////////////////////

SampleAssetManager::SampleAssetManager(SampleRenderer::Renderer& renderer) :
	m_renderer(renderer)
{
}

SampleAssetManager::~SampleAssetManager(void)
{
	PX_ASSERT(m_assets.size() == 0);
}

SampleAsset* SampleAssetManager::getAsset(const char* path, SampleAsset::Type type)
{	
	SampleAsset* asset = findAsset(path);
	if(!asset)
	{
		asset = loadAsset(path);
	}
	if(asset && asset->getType() != type)
	{
		releaseAsset(*asset);
		asset = 0;
	}
	if(asset)
	{
		asset->m_numUsers++;
	}
	return asset;
}

void SampleAssetManager::returnAsset(SampleAsset& asset)
{
	PX_ASSERT(asset.m_numUsers > 0);
	if(asset.m_numUsers > 0)
	{
		asset.m_numUsers--;
	}
	if(asset.m_numUsers == 0)
	{
		releaseAsset(asset);
	}
}

SampleAsset* SampleAssetManager::findAsset(const char* path)
{
	SampleAsset* asset = NULL;
	PxU32 numAssets = (PxU32)m_assets.size();
	for(PxU32 i=0; i<numAssets; i++)
	{
		if(!strcmp(m_assets[i]->getPath(), path))
		{
			asset = m_assets[i];
			break;
		}
	}
	return asset;
}

static const char* strext(const char* str)
{
	const char* ext = str;
	while(str)
	{
		str = strchr(str, '.');
		if(str)
		{
			str++;
			ext = str;
		}
	}
	return ext;
}

SampleAsset* SampleAssetManager::loadAsset(const char* path)
{
	const char* extension = strext(path);
	if (extension == NULL || *extension == 0)
		return NULL;

	SampleAsset* asset = NULL;
	
#if defined(RENDERER_ENABLE_PVR_SUPPORT)
	if(!strcmp(extension, "dds"))
	{
		//replace dds with pvr extension and loadAsset.
		char pathPVR[512];
		strcpy(pathPVR, path);	
		strcpy(strstr(pathPVR,".dds"), ".pvr");
		return loadAsset(pathPVR);
	}
#endif
		
	FILE* file = NULL;
		
	if(!strcmp(extension, "ods")) 
		file = findFile(path, false);
	else
		file = findFile(path, true);

	if(!file)
	{
		if(!strcmp(extension, "ods")) 
			Fnd::fopen_s(&file, path, "r+");
		else
			Fnd::fopen_s(&file, path, "rb");
	}

	if(file)
	{
		if(!strcmp(extension, "xml")) 
		{
			asset = loadXMLAsset(*file, path);
		}
		else if (!strcmp(extension, "dds"))
		{
#if defined(RENDERER_ENABLE_GXT_SUPPORT)
			asset = loadTextureAsset(*file, path, SampleTextureAsset::GXT);
#else
			asset = loadTextureAsset(*file, path, SampleTextureAsset::DDS);
#endif
		}		
#if defined(RENDERER_ENABLE_PVR_SUPPORT)
		else if (!strcmp(extension, "pvr"))
		{					
			asset = loadTextureAsset(*file, path, SampleTextureAsset::PVR);
		}
#endif
		else if (!strcmp(extension, "tga")) 
		{
			asset = loadTextureAsset(*file, path, SampleTextureAsset::TGA);
		}
		else if (!strcmp(extension, "bmp")) 
		{
			asset = loadTextureAsset(*file, path, SampleTextureAsset::BMP);
		}
		else if(!strcmp(extension, "ods")) 
		{
			asset = loadODSAsset(*file,path);
		}

		fclose(file);
	}
	else
	{
#define SAM_DEFAULT_MATERIAL "materials/simple_lit.xml"
		
#if defined(RENDERER_ENABLE_PVR_SUPPORT)
	#define SAM_DEFAULT_TEXTURE "textures/test.pvr"
#else
	#define SAM_DEFAULT_TEXTURE "textures/test.dds"
#endif

		// report the missing asset
		char msg[1024];

		if( !strcmp(extension, "xml") && strcmp(path, SAM_DEFAULT_MATERIAL) )  // Avoid infinite recursion
		{
			physx::string::sprintf_s(msg, sizeof(msg), "Could not find material: %s, loading default material: %s", 
				path, SAM_DEFAULT_MATERIAL);
			RENDERER_OUTPUT_MESSAGE(&m_renderer, msg);

			return loadAsset(SAM_DEFAULT_MATERIAL);  // Try to use the default asset
		}
		else if(!strcmp(extension, "dds"))
		{
			physx::string::sprintf_s(msg, sizeof(msg), "Could not find texture: %s, loading default texture: %s", 
				path, SAM_DEFAULT_TEXTURE);
			RENDERER_OUTPUT_MESSAGE(&m_renderer, msg);

			return loadAsset(SAM_DEFAULT_TEXTURE);  // Try to use the default asset
		}
#if defined(RENDERER_ENABLE_PVR_SUPPORT)
		else if(!strcmp(extension, "pvr"))
		{
			physx::string::sprintf_s(msg, sizeof(msg), "Could not find texture: %s, loading default texture: %s", 
									 path, SAM_DEFAULT_TEXTURE);
			RENDERER_OUTPUT_MESSAGE(&m_renderer, msg);
			
			return loadAsset(SAM_DEFAULT_TEXTURE);  // Try to use the default asset
		}
#endif
		else if(!strcmp(extension, "ods"))
		{
			physx::string::sprintf_s(msg, sizeof(msg), "Could not find input definition: %s", path);
			RENDERER_OUTPUT_MESSAGE(&m_renderer, msg);
		}
	}

	if(asset && !asset->isOk())
	{
		delete asset;
		asset = 0;
	}
	if(asset)
	{
		m_assets.push_back(asset);
	}
	return asset;
}

void SampleAssetManager::releaseAsset(SampleAsset& asset)
{
	PxU32 numAssets = (PxU32)m_assets.size();
	PxU32 found     = numAssets;
	for(PxU32 i=0; i<numAssets; i++)
	{
		if(&asset == m_assets[i])
		{
			found = i;
			break;
		}
	}
	PX_ASSERT(found < numAssets);
	if(found < numAssets)
	{
		m_assets[found] = m_assets.back();
		m_assets.pop_back();
		asset.release();
	}
}

SampleAsset* SampleAssetManager::loadXMLAsset(FILE& file, const char* path)
{
	SampleAsset* asset = NULL;

	PxFileBuffer theBuffer(findPath(path), physx::general_PxIOStream2::PxFileBuf::OPEN_READ_ONLY);
	FAST_XML::XmlBuilder builder;
	FAST_XML::FastXml *xml = createFastXml(&builder);
	xml->processXml(theBuffer, false);
	FAST_XML::xml_node* rootnode = builder.rootnode();
	if(!strcmp(rootnode->name(), "material"))
	{
		asset = new SampleMaterialAsset(*this, *rootnode, path);
	}

	return asset;
}

SampleAsset* SampleAssetManager::loadTextureAsset(FILE& file, const char* path, SampleTextureAsset::Type texType)
{
	SampleTextureAsset* asset = new SampleTextureAsset(getRenderer(), file, path, texType);
	return asset;
}

SampleAsset* SampleAssetManager::loadODSAsset(FILE& file, const char* path)
{
	SampleAsset* asset = NULL;

	asset = new SampleInputAsset(&file, path);

	return asset;
}
