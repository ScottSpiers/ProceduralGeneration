
#include "ProceduralScene.h"

ProceduralScene::ProceduralScene(ResourceManager::ManagerScene sceneResources) : Scene(sceneResources)
{
	m_terrain = 0;
	m_lsystem = 0;

	for (int i = 0; i < m_trees.size(); ++i)
	{
		m_trees[i] = new LTree();
	}
}

ProceduralScene::~ProceduralScene()
{
	/*for (LTree* l : m_trees)
	{
		if (l)
		{
			delete l;
			l = 0;
		}
	}*/

	if (m_lsystem)
	{
		delete m_lsystem;
		m_lsystem = 0;
	}

	if (m_terrain)
	{
		delete m_terrain;
		m_terrain = 0;
	}
}

bool ProceduralScene::Initialise(ID3D11Device* device , ID3D11DeviceContext* context)
{
	bool result;
	float terrainSize = 513.0f;

	result = Scene::Initialise(device, context);  
	if (!result)
		return false;

	m_terrain = new Terrain(terrainSize,terrainSize);
	m_terrain->SetTexture(m_resources->GetTexture(ResourceManager::TERRAIN_TEXTURE));
	m_terrain->GenRandom();
	//m_terrain->GenSinWave();

	m_Light->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecColour(0.1f, .1f, .1f, 1.f);
	m_Light->SetSpecIntensity(1.f);
	m_Light->SetDirection(1.0f, -.5f, 1.0f);
	
	m_Camera->SetPosition(terrainSize/2.0f, 2.0f, terrainSize/2.0f);

	result = m_terrain->Initialise(device);
	if (!result)
		return false;

	//The cube!
	/*m_lsystem = new LSystem("A");
	m_lsystem->AddRule('A', "B-F+CFC+F-D&F^D-F+&&CFC+F+B//");
	m_lsystem->AddRule('B', "A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//");
	m_lsystem->AddRule('C', "|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//");
	m_lsystem->AddRule('D', "|CFB-F+B|FA&F^A&&FB-F+B|FC//");*/

	//a=90, n = 3 (or 5 if you want to be stupid)

	/*m_lsystem = new LSystem("F[X][Y][Z]");
	m_lsystem->AddRule('F', "F[+[X][Y][Z]][-[X][Y][Z]][^[X][Y][Z]][&[X][Y][Z]][/[X][Y][Z]][[X][Y][Z]]");
	m_lsystem->AddRule('Z', "FZ[+FZ][/FX][^FY]F", 0.5f);
	m_lsystem->AddRule('Z', "FZ[-FZ][\FX][&FY]F", 0.5f);
	m_lsystem->AddRule('X', "FX[+FZ][/FX][^FY]F", 0.5f);
	m_lsystem->AddRule('X', "FX[-FZ][\FX][&FY]F", 0.5f);
	m_lsystem->AddRule('Y', "FY[+FZ][/FX][^FY]F", 0.5f);
	m_lsystem->AddRule('Y', "FY[-FZ][\FX][&FY]F", 0.5f);*/
	//this is a mess


	/*m_lsystem = new LSystem("A");
	m_lsystem->AddRule('A', "[&FL!A]/////'[&FL!A]////////'[&FL!A]");
	m_lsystem->AddRule('F', "S/////F");
	m_lsystem->AddRule('S', "FL");
	m_lsystem->AddRule('L', "['''^^{-f+f+f-|-f+f+f}]");*/
	//n = 7, a = 22.5

	/*m_lsystem = new LSystem("P");
	m_lsystem->AddRule('P', "I+[P+H]--//[--L]I[++L]-[PH]++PH");
	m_lsystem->AddRule('I', "FS[//&&L][//^^L]FS");
	m_lsystem->AddRule('S', "S[//&&L][//^^L]FS", 0.33f);
	m_lsystem->AddRule('S', "SFS", 0.33f);
	m_lsystem->AddRule('S', "S", 0.34f);
	m_lsystem->AddRule('L', "['{+f-ff-f+|+f-ff-f}]");
	m_lsystem->AddRule('H', "[&&&D'/W////W/////W////W////W]");
	m_lsystem->AddRule('D', "FF");
	m_lsystem->AddRule('W', "['^F][{&&&&-f+f|-f+f}]");*/
	//n = 5, a = 18.0
	   
	m_lsystem = new LSystem("X");
	m_lsystem->AddRule('X', "F[+X][-X][^X][&X]FX");
	m_lsystem->AddRule('F', "FF");
	//n = 6, a = 25.7

	/*m_lsystem = new LSystem("F");
	m_lsystem->AddRule('F', "F[+F]F[-F]F", 0.33f);
	m_lsystem->AddRule('F', "F[+F]F", 0.33f);
	m_lsystem->AddRule('F', "F[-F]F", 0.33f);*/
	//n = 5, a = 20.0

	int numIts = 6;

	float stepSize = .75f;
	float angleDelta = (25.7f * XM_PI) / 180;
	//float terrainSize = 513.0f;

	std::string testLSystem = m_lsystem->RunSystem(numIts);
	//XMMATRIX newPos = XMMatrixTranslation(terrainSize, 0.0f, 0.0f);
	//m_trees[0]->SetWorldMatrix(newPos);
	m_trees[0]->InterpretSystem(testLSystem, stepSize, angleDelta);
	m_trees[0]->SetTexture(m_resources->GetTexture(ResourceManager::TREE_TEXTURE));
	m_trees[0]->Initialise(device);

	/*XMMATRIX newPos = XMMatrixTranslation(terrainSize, 0.0f, 0.0f);
	m_trees[1]->SetWorldMatrix(newPos);
	m_trees[1]->InterpretSystem(m_lsystem->RunSystem(numIts), stepSize, angleDelta);
	m_trees[1]->Initialise(device);

	newPos = XMMatrixTranslation(0.0f, 0.0f, terrainSize);
	m_trees[2]->SetWorldMatrix(newPos);
	m_trees[2]->InterpretSystem(m_lsystem->RunSystem(numIts), stepSize, angleDelta);
	m_trees[2]->Initialise(device);

	newPos = XMMatrixTranslation(terrainSize, 0.0f, terrainSize);
	m_trees[3]->SetWorldMatrix(newPos);
	m_trees[3]->InterpretSystem(m_lsystem->RunSystem(numIts), stepSize, angleDelta);
	m_trees[3]->Initialise(device);

	newPos = XMMatrixTranslation(terrainSize / 2, 0.0f, terrainSize / 2);
	m_trees[4]->SetWorldMatrix(newPos);
	m_trees[4]->InterpretSystem(m_lsystem->RunSystem(numIts), stepSize, angleDelta);
	m_trees[4]->Initialise(device);*/

	return true;	
}

bool ProceduralScene::Render(D3D* d3d)
{
	XMMATRIX viewMatrix, projMatrix;
	bool result;

	viewMatrix = m_Camera->GetViewMatrix();

	// Set the initial position of the camera.
	
	d3d->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);
	m_Camera->Render();
	

	d3d->BeginScene(0.0f, 0.0f, .8f, 1.0f);
	//m_Camera->Render();

	float x = m_Camera->GetPosition().x;
	float z = m_Camera->GetPosition().z;
	float heightOffset = 10.0f;

	m_Camera->SetPosition(x, m_terrain->GetTerrainHeight(x, z) + heightOffset, z);
	/*d3d->GetDeviceContext()->ClearRenderTargetView(defRTV, colour);
	d3d->GetDeviceContext()->ClearDepthStencilView(defDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);*/


	//TERRAIN TURN BACK ON EVENTUALLY
	result = m_terrain->Render(d3d->GetDeviceContext());
	if (!result)
		return false;
	
	result = m_shaders->RenderTerrain(m_terrain, m_Camera, m_Light);
	if (!result)
		return false;	

	//d3d->TurnOffCulling();
	/*for (LTree* lt : m_trees)
	{
		lt->Render(d3d->GetDeviceContext());
		result = m_shaders->RenderLTree(lt, m_Camera, m_Light);
		if (!result)
			return false;
	}*/
	m_trees[0]->Render(d3d->GetDeviceContext());
	result = m_shaders->RenderLTree(m_trees[0], m_Camera, m_Light);
	if (!result)
		return false;

	//d3d->TurnOffCulling();	

	//d3d->TurnOnCulling();

	//Model* orbitSphere = m_resources->GetModel(ResourceManager::ORBIT_MODEL);
	////orbitSphere->SetWorldMatrix(XMMatrixTranslation(0.f, 0.f, -.f));

	//orbitSphere->Render(d3d->GetDeviceContext());

	//result = m_shaders->RenderLight(orbitSphere, m_Camera, m_Light);
	//if (!result)
	//{
	//	return false;
	//}

	return true;
}