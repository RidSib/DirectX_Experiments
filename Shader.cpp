#include "Shader.h"
#include "Device.h"
#include <d3dx10.h>
#include <atlbase.h>

// Effects / techniques
ID3D10Effect*          Effect = NULL;
ID3D10EffectTechnique* PlainColourTechnique = NULL;
ID3D10EffectTechnique* VertexTexTechnique = NULL;
ID3D10EffectTechnique* VertexChangingTexTechnique = NULL;
ID3D10EffectTechnique* VertexLitTexTechnique = NULL;
ID3D10EffectTechnique* NormalMappingTechnique = NULL;
ID3D10EffectTechnique* NormalMappingParaTechnique = NULL;

// Light Effect variables
ID3D10EffectVectorVariable* g_pCameraPosVar = NULL;
ID3D10EffectVectorVariable* g_pLightPosVar = NULL;
ID3D10EffectVectorVariable* g_pLightColourVar = NULL;
ID3D10EffectVectorVariable* g_pLight2PosVar = NULL;
ID3D10EffectVectorVariable* g_pLight2ColourVar = NULL;
ID3D10EffectVectorVariable* g_pAmbientColourVar = NULL;
ID3D10EffectScalarVariable* g_pSpecularPowerVar = NULL;

// Matrices
ID3D10EffectMatrixVariable* WorldMatrixVar = NULL;
ID3D10EffectMatrixVariable* ViewMatrixVar = NULL;
ID3D10EffectMatrixVariable* ProjMatrixVar = NULL;
ID3D10EffectMatrixVariable* ViewProjMatrixVar = NULL;

// Variables
ID3D10EffectScalarVariable* colourMultiVar = NULL;
ID3D10EffectScalarVariable* ParallaxDepthVar = NULL; // To set the depth of the parallax mapping effect

// Textures - no texture class yet so using DirectX variables
ID3D10ShaderResourceView* CubeDiffuseMap = NULL;
ID3D10ShaderResourceView* FloorDiffuseMap = NULL;
ID3D10ShaderResourceView* SphereDiffuseMap = NULL;
ID3D10ShaderResourceView* TeapotDiffuseMap = NULL;
ID3D10ShaderResourceView* TrollDiffuseMap = NULL;
ID3D10ShaderResourceView* Cube2DiffuseMap = NULL;
ID3D10ShaderResourceView* Cube2NormalMap = NULL;
ID3D10ShaderResourceView* Teapot2DiffuseMap = NULL;
ID3D10ShaderResourceView* Teapot2NormalMap = NULL;

// Miscellaneous
ID3D10EffectVectorVariable* ModelColourVar = NULL;

ID3D10EffectShaderResourceVariable* DiffuseMapVar = NULL;
ID3D10EffectShaderResourceVariable* NormalMapVar = NULL;

//--------------------------------------------------------------------------------------
// Load and compile Effect file (.fx file containing shaders)
//--------------------------------------------------------------------------------------
// An effect file contains a set of "Techniques". A technique is a combination of vertex, geometry and pixel shaders (and some states) used for
// rendering in a particular way. We load the effect file at runtime (it's written in HLSL and has the extension ".fx"). The effect code is compiled
// *at runtime* into low-level GPU language. When rendering a particular model we specify which technique from the effect file that it will use
//
bool LoadEffectFile()
{
	ID3D10Blob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options

														  // Load and compile the effect file
	HRESULT hr = D3DX10CreateEffectFromFile(L"GraphicsAssign1.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, g_pd3dDevice, NULL, NULL, &Effect, &pErrors, NULL);
	if (FAILED(hr))
	{
		if (pErrors != 0)  MessageBox(NULL, CA2CT(reinterpret_cast<char*>(pErrors->GetBufferPointer())), L"Error", MB_OK); // Compiler error: display error message
		else               MessageBox(NULL, L"Error loading FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK);  // No error message - probably file not found
		return false;
	}

	// Now we can select techniques from the compiled effect file
	PlainColourTechnique = Effect->GetTechniqueByName("PlainColour");
	VertexTexTechnique = Effect->GetTechniqueByName("VertexTex");
	VertexChangingTexTechnique = Effect->GetTechniqueByName("VertexChangingTex");
	VertexLitTexTechnique = Effect->GetTechniqueByName("VertexLitTex");
	NormalMappingTechnique = Effect->GetTechniqueByName("NormalMapping");
	NormalMappingParaTechnique = Effect->GetTechniqueByName("NormalMappingPara");

	// Create special variables to allow us to access global variables in the shaders from C++
	WorldMatrixVar = Effect->GetVariableByName("WorldMatrix")->AsMatrix();
	ViewMatrixVar = Effect->GetVariableByName("ViewMatrix")->AsMatrix();
	ProjMatrixVar = Effect->GetVariableByName("ProjMatrix")->AsMatrix();

	// We access the texture variable in the shader in the same way as we have before for matrices, light data etc.
	// Only difference is that this variable is a "Shader Resource"
	DiffuseMapVar = Effect->GetVariableByName("DiffuseMap")->AsShaderResource();

	// Other shader variables
	ModelColourVar = Effect->GetVariableByName("ModelColour")->AsVector();

	// Other variables
	colourMultiVar = Effect->GetVariableByName("colourMulti")->AsScalar();

	// Light variables
	g_pCameraPosVar = Effect->GetVariableByName("CameraPos")->AsVector();
	g_pLightPosVar = Effect->GetVariableByName("Light1Pos")->AsVector();
	g_pLightColourVar = Effect->GetVariableByName("Light1Colour")->AsVector();
	g_pLight2PosVar = Effect->GetVariableByName("Light2Pos")->AsVector();
	g_pLight2ColourVar = Effect->GetVariableByName("Light2Colour")->AsVector();
	g_pAmbientColourVar = Effect->GetVariableByName("AmbientColour")->AsVector();
	g_pSpecularPowerVar = Effect->GetVariableByName("SpecularPower")->AsScalar();
	NormalMapVar = Effect->GetVariableByName("NormalMap")->AsShaderResource();
	ParallaxDepthVar = Effect->GetVariableByName("ParallaxDepth")->AsScalar();

	return true;
}

// Release the memory held by all objects created
void ReleaseShaders()
{

	if (FloorDiffuseMap)  FloorDiffuseMap->Release();
	if (CubeDiffuseMap)   CubeDiffuseMap->Release();
	if (Cube2DiffuseMap)   Cube2DiffuseMap->Release();
	if (Cube2NormalMap)   Cube2NormalMap->Release();
	if (Teapot2DiffuseMap)   Teapot2DiffuseMap->Release();
	if (Teapot2NormalMap)   Teapot2NormalMap->Release();
	if (SphereDiffuseMap)  SphereDiffuseMap->Release();
	if (TeapotDiffuseMap)  TeapotDiffuseMap->Release();
	if (TrollDiffuseMap)  TrollDiffuseMap->Release();
	if (Effect)           Effect->Release();
	if (DepthStencilView) DepthStencilView->Release();
	if (RenderTargetView) RenderTargetView->Release();
	if (DepthStencil)     DepthStencil->Release();
	if (SwapChain)        SwapChain->Release();

}