#pragma once
#include "Model.h"
class Light : public CModel
{
private:
	D3DXVECTOR3 InitColour; // initial colour
	D3DXVECTOR3 CurrentColour; // current colour
public:
	Light();
	~Light();
	D3DXVECTOR3 GetColour()
	{
		return CurrentColour;
	}
	D3DXVECTOR3 GetInitColour()
	{
		return InitColour;
	}


	void SetColour(D3DXVECTOR3 colour)
	{
		CurrentColour = colour;
	}
	void SetInitColour(D3DXVECTOR3 colour)
	{
		InitColour = colour;
	}
};

