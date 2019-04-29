#pragma once

#include "Camera.h"
#include "Light.h"
#include "Model.h"

#include "LightShader.h"
#include "LTreeShader.h"
#include "ReflectShader.h"
#include "FontShader.h"
#include "SkySphereShader.h"
#include "TerrainShader.h"
#include "TextureShader.h"

class ShaderManager
{
public:
	ShaderManager(ID3D11Device*, ID3D11DeviceContext*);
	~ShaderManager();

	bool InitialiseShaders();
	bool RenderLight(Model*, Camera*, Light*);
	bool RenderSkySphere(Model*, Camera*, float);
	bool RenderReflection(Model*, Camera*);
	bool RenderTerrain(Terrain*, Camera*, Light*);
	bool RenderLTree(LTree*, Camera*, Light*);
	bool RenderTexture(Quad*, XMMATRIX, XMMATRIX);
	//bool RenderFont();

private:
	LightShader* m_lightShader;
	SkySphereShader* m_skyShader;
	ReflectShader* m_reflectShader;
	TerrainShader* m_terrainShader;
	LTreeShader* m_lTreeShader;
	TextureShader* m_textureShader;
	//FontShader* m_fontShader; Do I want this to be managed here?
	//Font does the shading itself but is that nice?
	//Do I not want it to be here as it is a shader?

};