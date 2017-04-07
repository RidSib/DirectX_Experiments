#include <d3d10.h>

// Header guard - prevents this file being included more than once
#pragma once

// Effects / techniques
extern ID3D10Effect*          Effect;
extern ID3D10EffectTechnique* PlainColourTechnique;
extern ID3D10EffectTechnique* VertexTexTechnique;
extern ID3D10EffectTechnique* VertexChangingTexTechnique;
extern ID3D10EffectTechnique* VertexLitTexTechnique;
extern ID3D10EffectTechnique* NormalMappingTechnique;
extern ID3D10EffectTechnique* NormalMappingParaTechnique;
// Light Effect variables
extern ID3D10EffectVectorVariable* g_pCameraPosVar;
extern ID3D10EffectVectorVariable* g_pLightPosVar;
extern ID3D10EffectVectorVariable* g_pLightColourVar;
extern ID3D10EffectVectorVariable* g_pLight2PosVar;
extern ID3D10EffectVectorVariable* g_pLight2ColourVar;
extern ID3D10EffectVectorVariable* g_pAmbientColourVar;
extern ID3D10EffectScalarVariable* g_pSpecularPowerVar;

// Matrices
extern ID3D10EffectMatrixVariable* WorldMatrixVar;
extern ID3D10EffectMatrixVariable* ViewMatrixVar;
extern ID3D10EffectMatrixVariable* ProjMatrixVar;
extern ID3D10EffectMatrixVariable* ViewProjMatrixVar;

// Variables
extern ID3D10EffectScalarVariable* colourMultiVar;
extern ID3D10EffectScalarVariable* ParallaxDepthVar;

// Textures - no texture class yet so using DirectX variables
extern ID3D10ShaderResourceView* CubeDiffuseMap;
extern ID3D10ShaderResourceView* FloorDiffuseMap;
extern ID3D10ShaderResourceView* SphereDiffuseMap;
extern ID3D10ShaderResourceView* TeapotDiffuseMap;
extern ID3D10ShaderResourceView* TrollDiffuseMap;
extern ID3D10ShaderResourceView* Cube2DiffuseMap;
extern ID3D10ShaderResourceView* Cube2NormalMap;
extern ID3D10ShaderResourceView* Teapot2DiffuseMap;
extern ID3D10ShaderResourceView* Teapot2NormalMap;

// Miscellaneous
extern ID3D10EffectVectorVariable* ModelColourVar;

extern ID3D10EffectShaderResourceVariable* DiffuseMapVar;
extern ID3D10EffectShaderResourceVariable* NormalMapVar;

// Initialise shaders - load an effect file (.fx file containing shaders)
bool LoadEffectFile();

// Release shader objects to free memory when quitting
void ReleaseShaders();



