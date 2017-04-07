//--------------------------------------------------------------------------------------
//	GraphicsAssign1.cpp
//
//	Shaders Graphics Assignment
//	Add further models using different shader techniques
//	See assignment specification for details
//--------------------------------------------------------------------------------------

//***|  INFO  |****************************************************************
// Lights:
//   The initial project shows models for a couple of point lights, but the
//   initial shaders don't actually apply any lighting. Part of the assignment
//   is to add a shader that uses this information to actually light a model.
//   Refer to lab work to determine how best to do this.
// 
// Textures:
//   The models in the initial scene have textures loaded but the initial
//   technique/shaders doesn't show them. Part of the assignment is to add 
//   techniques to show a textured model
//
// Shaders:
//   The initial shaders render plain coloured objects with no lighting:
//   - The vertex shader performs basic transformation of 3D vertices to 2D
//   - The pixel shader colours every pixel the same colour
//   A few shader variables are set up to communicate between C++ and HLSL
//   You will usually need to add further variables to add new techniques
//*****************************************************************************

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <atlbase.h>
#include "resource.h"

#include "Defines.h" // General definitions shared by all source files
#include "Device.h"
#include "Model.h"   // Model class - encapsulates working with vertex/index data and world matrix
#include "Camera.h"  // Camera class - encapsulates the camera's view and projection matrix
#include "Shader.h"
#include "Input.h"   // Input functions - not DirectX


//--------------------------------------------------------------------------------------
// Global Scene Variables
//--------------------------------------------------------------------------------------

// Models and cameras encapsulated in classes for flexibity and convenience
// The CModel class collects together geometry and world matrix, and provides functions to control the model and render it
// The CCamera class handles the view and projections matrice, and provides functions to control the camera
CModel* Cube;
CModel* Cube2;
CModel* Floor;
CCamera* Camera;
CModel* Sphere;
CModel* Teapot;
CModel* Troll;


// Light data - stored manually as there is no light class
D3DXVECTOR3 Light1Colour = D3DXVECTOR3( 1.0f, 0.0f, 0.7f );
D3DXVECTOR3 Light2Colour = D3DXVECTOR3( 1.0f, 0.8f, 0.2f );
D3DXVECTOR3 AmbientColour = D3DXVECTOR3( 0.2f, 0.2f, 0.2f );
float SpecularPower = 256.0f;

// changing lights variables

D3DXVECTOR3 cLight1Colour = D3DXVECTOR3(1.0f, 0.0f, 0.7f) * 10;
D3DXVECTOR3 cLight2Colour = D3DXVECTOR3(1.0f, 0.8f, 0.2f) * 40;


// Display models where the lights are. One of the lights will follow an orbit
CModel* Light1;
CModel* Light2;
const float LightOrbitRadius = 20.0f;
const float LightOrbitSpeed  = 0.5f;

// Variables used to setup the Window
int       g_ViewportWidth;
int       g_ViewportHeight;





//--------------------------------------------------------------------------------------
// Scene Setup / Update / Rendering
//--------------------------------------------------------------------------------------

// Create / load the camera, models and textures for the scene
bool InitScene()
{
	//////////////////
	// Create camera

	Camera = new CCamera();
	Camera->SetPosition( D3DXVECTOR3(-15, 20,-40) );
	Camera->SetRotation( D3DXVECTOR3(ToRadians(13.0f), ToRadians(18.0f), 0.0f) ); // ToRadians is a new helper function to convert degrees to radians


	///////////////////////
	// Load/Create models

	Cube = new CModel;
	Cube2 = new CModel;
	Sphere = new CModel;
	Floor = new CModel;
	Teapot = new CModel;
	Troll = new CModel;
	Light1 = new CModel;
	Light2 = new CModel;

	// The model class can load ".X" files. It encapsulates (i.e. hides away from this code) the file loading/parsing and creation of vertex/index buffers
	// We must pass an example technique used for each model. We can then only render models with techniques that uses matching vertex input data
	if (!Cube->  Load( "Cube.x", VertexChangingTexTechnique)) return false;
	if (!Cube2->Load("Cube.x", NormalMappingTechnique, true)) return false;
	if (!Sphere->Load("Sphere.x", VertexChangingTexTechnique)) return false;
	if (!Teapot->Load("Teapot.x", VertexLitTexTechnique)) return false;
	if (!Troll->Load("Troll.x", VertexLitTexTechnique)) return false;
	if (!Floor-> Load( "Floor.x", VertexTexTechnique)) return false;
	if (!Light1->Load( "Sphere.x", PlainColourTechnique )) return false;
	if (!Light2->Load( "Sphere.x", PlainColourTechnique )) return false;

	// Initial positions
	Cube->SetPosition( D3DXVECTOR3(0, 10, 0) );
	Cube2->SetPosition(D3DXVECTOR3(-20, 10, 50));
	Sphere->SetPosition(D3DXVECTOR3(30, 20, 50));
	Sphere->SetScale(0.5f);
	Teapot->SetPosition(D3DXVECTOR3(0, 10, 40));
	Troll->SetPosition(D3DXVECTOR3(-10, 10, 50));
	Troll->SetScale(5.0f);
	Light1->SetPosition( D3DXVECTOR3(30, 10, 0) );
	Light1->SetScale( 0.1f ); // Nice if size of light reflects its brightness
	Light2->SetPosition( D3DXVECTOR3(-20, 30, 50) );
	Light2->SetScale( 0.2f );


	//////////////////
	// Load textures

	if (FAILED( D3DX10CreateShaderResourceViewFromFile( g_pd3dDevice, L"StoneDiffuseSpecular.dds", NULL, NULL, &CubeDiffuseMap,  NULL ) ))
		return false;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, L"WoodDiffuseSpecular.dds", NULL, NULL, &FloorDiffuseMap, NULL)))
		return false;
	if (FAILED( D3DX10CreateShaderResourceViewFromFile( g_pd3dDevice, L"BushDiffuseSpecularAlpha.dds",  NULL, NULL, &SphereDiffuseMap, NULL ) ))
		return false;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, L"PalletA.dds", NULL, NULL, &TeapotDiffuseMap, NULL)))
		return false;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, L"Troll1DiffuseSpecular.dds", NULL, NULL, &TrollDiffuseMap, NULL)))
		return false;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, L"PatternDiffuseSpecular.dds", NULL, NULL, &Cube2DiffuseMap, NULL)))
		return false;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice, L"PatternNormal.dds", NULL, NULL, &Cube2NormalMap, NULL))) 
		return false;

	return true;
}


// Update the scene - move/rotate each model and the camera, then update their matrices
void UpdateScene( float frameTime )
{
	// Control camera position and update its matrices (view matrix, projection matrix) each frame
	// Don't be deceived into thinking that this is a new method to control models - the same code we used previously is in the camera class
	Camera->Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );
	Camera->UpdateMatrices();
	
	// Control cube position and update its world matrix each frame
	Cube->Control( frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );
	Cube->UpdateMatrix();
	//Cube2->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
	Cube2->UpdateMatrix();

	// Update the orbiting light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float Rotate = 0.0f;
	Light1->SetPosition( Cube->GetPosition() + D3DXVECTOR3(cos(Rotate)*LightOrbitRadius, 0, sin(Rotate)*LightOrbitRadius) );
	Rotate -= LightOrbitSpeed * frameTime;
	Light1->UpdateMatrix();

	// Second light doesn't move, but do need to make sure its matrix has been calculated - could do this in InitScene instead
	Light2->UpdateMatrix();

	// Sphere brightness/colour calculation
	float static runtimeFloat = 0.0f;
	runtimeFloat += frameTime;
	colourMultiVar->SetFloat(fmod(runtimeFloat, 5.0f)*0.2f);
	Sphere->UpdateMatrix();

	Teapot->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
	Teapot->UpdateMatrix();
	Troll->UpdateMatrix();
	int runtimeInt = static_cast<int>(runtimeFloat);

	// Light 2, gradualy changing blue value in Light 2
	float belowTwoSec = fmod(runtimeFloat, 2.0f);
	cLight1Colour = Light1Colour * (runtimeInt % 2);
	cLight2Colour = D3DXVECTOR3(Light2Colour.x, Light2Colour.y, (belowTwoSec > 1.0f ? 2.0f - belowTwoSec : belowTwoSec) + Light2Colour.z);
	g_pLightPosVar->SetRawValue(Light1->GetPosition(), 0, 12);  // Send 3 floats (12 bytes) from C++ LightPos variable (x,y,z) to shader counterpart (middle parameter is unused) 
	g_pLightColourVar->SetRawValue(cLight1Colour, 0, 12);
	g_pLight2PosVar->SetRawValue(Light2->GetPosition(), 0, 12);
	g_pLight2ColourVar->SetRawValue(cLight2Colour, 0, 12);
	g_pAmbientColourVar->SetRawValue(AmbientColour, 0, 12);
	g_pSpecularPowerVar->SetFloat(SpecularPower);
	g_pCameraPosVar->SetRawValue(Camera->GetPosition(), 0, 12);
}


// Render everything in the scene
void RenderScene()
{
	// Clear the back buffer - before drawing the geometry clear the entire window to a fixed colour
	float ClearColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f }; // Good idea to match background to ambient colour
	g_pd3dDevice->ClearRenderTargetView( RenderTargetView, ClearColor );
	g_pd3dDevice->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 ); // Clear the depth buffer too


	//---------------------------
	// Common rendering settings

	// Common features for all models, set these once only

	// Pass the camera's matrices to the vertex shader
	ViewMatrixVar->SetMatrix( (float*)&Camera->GetViewMatrix() );
	ProjMatrixVar->SetMatrix( (float*)&Camera->GetProjectionMatrix() );


	//---------------------------
	// Render each model
	
	// Constant colours used for models in initial shaders
	D3DXVECTOR3 Black( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 Blue( 0.0f, 0.0f, 1.0f );

	// Render cube
	WorldMatrixVar->SetMatrix( (float*)Cube->GetWorldMatrix() );  // Send the cube's world matrix to the shader
    DiffuseMapVar->SetResource( CubeDiffuseMap );                 // Send the cube's diffuse/specular map to the shader
	//ModelColourVar->SetRawValue( Blue, 0, 12 );           // Set a single colour to render the model
	Cube->Render(VertexTexTechnique);                         // Pass rendering technique to the model class

	WorldMatrixVar->SetMatrix((float*)Cube2->GetWorldMatrix());
	DiffuseMapVar->SetResource(Cube2DiffuseMap);
	NormalMapVar->SetResource(Cube2NormalMap);               // Send the cube's normal map to the shader
	Cube2->Render(NormalMappingTechnique);

	WorldMatrixVar->SetMatrix((float*)Sphere->GetWorldMatrix());
	DiffuseMapVar->SetResource(SphereDiffuseMap);
	Sphere->Render(VertexChangingTexTechnique);

	WorldMatrixVar->SetMatrix((float*)Teapot->GetWorldMatrix());
	DiffuseMapVar->SetResource(TeapotDiffuseMap);
	Teapot->Render(VertexLitTexTechnique);

	WorldMatrixVar->SetMatrix((float*)Troll->GetWorldMatrix());
	DiffuseMapVar->SetResource(TrollDiffuseMap);
	Troll->Render(VertexLitTexTechnique);

	// Same for the other models in the scene
	WorldMatrixVar->SetMatrix( (float*)Floor->GetWorldMatrix() );
    DiffuseMapVar->SetResource( FloorDiffuseMap );
	ModelColourVar->SetRawValue( Black, 0, 12 );
	Floor->Render(VertexTexTechnique);

	WorldMatrixVar->SetMatrix( (float*)Light1->GetWorldMatrix() );
	//ModelColourVar->SetRawValue( Light1Colour, 0, 12 );
	ModelColourVar->SetRawValue(cLight1Colour, 0, 12);
	Light1->Render( PlainColourTechnique );

	WorldMatrixVar->SetMatrix( (float*)Light2->GetWorldMatrix() );
	//ModelColourVar->SetRawValue( Light2Colour, 0, 12 );
	ModelColourVar->SetRawValue(cLight2Colour, 0, 12);
	Light2->Render( PlainColourTechnique );


	//---------------------------
	// Display the Scene

	// After we've finished drawing to the off-screen back buffer, we "present" it to the front buffer (the screen)
	SwapChain->Present( 0, 0 );
}

void ReleaseResources()
{
	delete Light2;
	delete Light1;
	delete Floor;
	delete Cube;
	delete Cube2;
	delete Sphere;
	delete Teapot;
	delete Troll;
	delete Camera;
}