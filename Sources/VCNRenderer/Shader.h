///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract shader class interface
///

#ifndef VCNSHADER_H
#define VCNSHADER_H

#pragma once

#include "VCNCore/Atom.h"
#include "VCNResources/Mesh.h"
#include "VCNUtils/Matrix.h"
#include "VCNUtils/RenderTypes.h"

class VCNCache;

class VCNShader : public VCNAtom
  // TODO: Make shader be a resource
{
  VCN_CLASS;

public:

  virtual ~VCNShader() = 0;

  /// Bind the vertex to the current context.
  virtual void Bind() = 0;

  /// Binds an index buffer to the current state
  virtual void BindIndexCache(VCNCache* indexBuffer) = 0;

  /// Binds an vertex buffer to the current state
  virtual void BindVertexCache(VCNUInt streamSource, VCNCache* vertexBuffer, VCNUInt offsetInBytes, VCNUInt stride) = 0;

  /// Begins the effect
  virtual void Begin() = 0;

  /// Begin a pass
  virtual void BeginPass(int pass) = 0;

  /// Ends a pass
  virtual void EndPass() = 0;

  /// Ends the effect
  virtual void End() = 0;

  /// Commit parameter changes before rendering
  virtual void CommitChanges() = 0;

  /// Create a vertex declaration and attach it to the shader for rendering.
  virtual void CreateVertexDeclaration(const VCNDeclarationElementList& list) = 0;

  /// Sets the shader technique name
  virtual void SetTechnique(const char* name) = 0;

  /// Returns a token to a shader parameter. This token can be used to set a value later.
  virtual VCNShaderParameter GetParameterByName(const char* name) const = 0;

	/// Sets a integer value
	virtual void SetInt(const VCNShaderParameter& param, int i) = 0;

  /// Sets a floating value
  virtual void SetFloat(const VCNShaderParameter& param, float f) = 0;

  /// Sets a matrix parameter
  virtual void SetMatrix(const VCNShaderParameter& param, const Matrix4& mat) = 0;

	/// Sets an array of matrices
	virtual void SetMatrixArray(const VCNShaderParameter& param, const std::vector<Matrix4>& matrices, std::size_t count) = 0;

  /// Sets a color parameter
  virtual void SetColor(const VCNShaderParameter& param, const VCNColor& color) = 0;

  /// Sets a texture value
  virtual void SetTexture(const VCNShaderParameter& param, const VCNResID textureID) = 0;

  /// Renders primitives
  virtual void RenderPrimitive(VCNUInt faceCount, VCNInt primitiveType = PT_TRIANGLELIST) = 0;

  /// Renders indexed primitives
  virtual void RenderIndexedPrimitive(VCNUInt faceCount, VCNUInt vertexCount, VCNInt primitiveType = PT_TRIANGLELIST) = 0;

  /// Renders indexed primitives
  virtual void RenderIndexedPrimitive(
    VCNPrimitiveType primitiveType, VCNInt baseVertexIndex, 
    VCNUInt minVertexIndex, VCNUInt numVertices, VCNUInt startIndex, VCNUInt primCount) = 0;

protected:

  VCNShader();
  
};

#endif // VCNSHADER_H
