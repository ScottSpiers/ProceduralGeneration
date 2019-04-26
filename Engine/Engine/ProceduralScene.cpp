
#include "ProceduralScene.h"

ProceduralScene::ProceduralScene(ResourceManager::ManagerScene sceneResources) : Scene(sceneResources)
{
	m_terrain = 0;
	m_lsystem = 0;
	m_ppQuad = 0;

	for (int i = 0; i < m_trees.size(); ++i)
	{
		m_trees[i] = new LTree();
	}
}

ProceduralScene::~ProceduralScene()
{
	for (LTree* l : m_trees)
	{
		if (l)
		{
			delete l;
			l = 0;
		}
	}

	if (m_ppQuad)
	{
		delete m_ppQuad;
		m_ppQuad = 0;
	}

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
	m_terrain->GenPerlin();
	//m_terrain->GenRandom();
	//m_terrain->GenSinWave();
	m_terrain->SetTextures(m_resources->GetTextures(ResourceManager::TERRAIN_TEXTURE));

	m_Light->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(.75f, .75f, .75f, 1.0f);
	m_Light->SetSpecColour(0.0f, .0f, .0f, 0.f);
	m_Light->SetSpecIntensity(1.f);
	m_Light->SetDirection(.75f, -.5f, .75f);
	
	m_Camera->SetPosition(terrainSize/2.0f - 10.0f, 2.0f, terrainSize/2.0f - 10.0f);
	m_ppView = GetView();

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
	XMMATRIX newPos = XMMatrixTranslation(terrainSize / 2.0f, 0.0f, terrainSize / 2.0f);
	m_trees[0]->SetWorldMatrix(newPos);
	m_trees[0]->InterpretSystem(testLSystem, stepSize, angleDelta);
	m_trees[0]->SetTextures(m_resources->GetTextures(ResourceManager::TREE_TEXTURE));
	m_trees[0]->Initialise(device);

	//newPos = XMMatrixTranslation(terrainSize, 0.0f, 0.0f);
	//m_trees[1]->SetWorldMatrix(newPos);
	////m_trees[1]->InterpretSystem(testLSystem /*m_lsystem->RunSystem(numIts)*/, stepSize, angleDelta);
	//m_trees[1]->Initialise(device);

	//newPos = XMMatrixTranslation(0.0f, 0.0f, terrainSize);
	//m_trees[2]->SetWorldMatrix(newPos);
	////m_trees[2]->InterpretSystem(testLSystem /*m_lsystem->RunSystem(numIts)*/, stepSize, angleDelta);
	//m_trees[2]->Initialise(device);

	//newPos = XMMatrixTranslation(terrainSize, 0.0f, terrainSize);
	//m_trees[3]->SetWorldMatrix(newPos);
	////m_trees[3]->InterpretSystem(testLSystem /*m_lsystem->RunSystem(numIts)*/, stepSize, angleDelta);
	//m_trees[3]->Initialise(device);

	//newPos = XMMatrixTranslation(terrainSize / 2, 0.0f, terrainSize / 2);
	//m_trees[4]->SetWorldMatrix(newPos);
	////m_trees[4]->InterpretSystem(testLSystem /*m_lsystem->RunSystem(numIts)*/, stepSize, angleDelta);
	//m_trees[4]->Initialise(device);


	m_isSphereAlive = true;
	m_sphere = m_resources->GetModel(ResourceManager::ORBIT_MODEL);
	m_skySphere = m_resources->GetModel(ResourceManager::SKY_DOME_MODEL);
	m_sphere->SetWorldMatrix(XMMatrixMultiply(XMMatrixScaling(2.0f,2.0f,2.0f), XMMatrixTranslation(250.0f, 6.66f, 250.0f)));
	m_sphereCount = 0;
	return true;	
}


bool ProceduralScene::InitialiseRenderTexture(ID3D11Device* device, XMFLOAT2 dimensions)
{
	HRESULT res;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = dimensions.x;
	textureDesc.Height = dimensions.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	res = device->CreateTexture2D(&textureDesc, 0, &m_ppRenderTarget);
	if (FAILED(res))
		return false;

	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	res = device->CreateRenderTargetView(m_ppRenderTarget, &rtvDesc, &m_ppRTV);
	if (FAILED(res))
		return false;

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	res = device->CreateShaderResourceView(m_ppRenderTarget, &srvDesc, &m_ppSRV);
	if(FAILED(res))
		return false;

	if (m_ppQuad)
	{
		delete m_ppQuad;
		m_ppQuad = 0;
	}

	m_ppQuad = new Quad(dimensions.x, dimensions.y);
	if (!m_ppQuad->Initialise(device))
		return false;

	
	m_orthoProj = XMMatrixOrthographicLH(dimensions.x, dimensions.y, 0.1f, 1000.0f);
	return true;
}

bool ProceduralScene::RenderToTexture(D3D* d3d)
{
	InitialiseRenderTexture(d3d->GetDevice(), d3d->GetScreenDimensions());
	d3d->GetDeviceContext()->OMSetRenderTargets(1, &m_ppRTV, d3d->getDepthStencilView());

	float colour[4]{ 0.0f, 0.0f, 1.0f, 1.0f };
	d3d->GetDeviceContext()->ClearRenderTargetView(m_ppRTV, colour);
	d3d->GetDeviceContext()->ClearDepthStencilView(d3d->getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	RenderScene(d3d);

	m_ppQuad->SetTexture(m_ppSRV);

	ID3D11RenderTargetView* rtv = d3d->getRenderTargetView();
	d3d->GetDeviceContext()->OMSetRenderTargets(1, &rtv, d3d->getDepthStencilView());

	return true;
}

bool ProceduralScene::RenderScene(D3D* d3d)
{
	XMMATRIX viewMatrix, projMatrix;
	bool result;

	viewMatrix = m_Camera->GetViewMatrix();

	// Set the initial position of the camera.

	d3d->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);
	m_Camera->Render();

	float x = m_Camera->GetPosition().x;
	float z = m_Camera->GetPosition().z;
	float heightOffset = 10.0f;

	XMFLOAT3 cameraPos = m_Camera->GetPosition();
	float terrainHeight = m_terrain->GetTerrainHeight(x, z);
	m_skySphere->SetWorldMatrix(XMMatrixTranslation(cameraPos.x, cameraPos.y , cameraPos.z));
	m_Camera->SetPosition(x, terrainHeight + heightOffset, z);

	XMVECTOR spherePos = m_sphere->GetWorldMatrix().r[3];
	XMVECTOR camPos = XMLoadFloat3(&m_Camera->GetPosition());
	XMVECTOR dist = XMVectorSubtract(spherePos, camPos);
	XMFLOAT3 fDist;
	XMStoreFloat3(&fDist, dist);


	if (abs(fDist.x) < 2.0f && abs(fDist.y) < 10.0f && abs(fDist.z) < 2.0f)
	{
		++m_sphereCount;
		float rx = (double)rand() / (RAND_MAX + 1);
		rx *= 513.0f;

		float rz = (double)rand() / (RAND_MAX + 1);
		rz *= 513.0f;

		if(m_sphereCount >= 7)
			m_isSphereAlive = false;
		else
			m_sphere->SetWorldMatrix(XMMatrixTranslation(rx, terrainHeight, rz));		
	}

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
	if(!m_trees[0]->Render(d3d->GetDeviceContext()))
	{
		return false;
	}
	result = m_shaders->RenderLTree(m_trees[0], m_Camera, m_Light);
	if (!result)
		return false;


	if (m_isSphereAlive)
	{
		
		m_sphere->Render(d3d->GetDeviceContext());
		m_shaders->RenderLight(m_sphere, m_Camera, m_Light);
	}

	d3d->TurnOffCulling();
	d3d->SetDepthLessEqual();


	m_skySphere->Render(d3d->GetDeviceContext());
	result = m_shaders->RenderSkySphere(m_skySphere, m_Camera);
	if (!result)
	{
		return false;
	}

	d3d->TurnOnCulling();
	d3d->SetDepthLess();
	return true;
}

bool ProceduralScene::Render(D3D* d3d)
{
	bool res;
	res = RenderToTexture(d3d);
	if (!res)
		return false;

	d3d->BeginScene(0.0f, 0.0f, .8f, 1.0f);

	d3d->TurnZBufferOff();

	m_ppQuad->Render(d3d->GetDeviceContext());
	m_shaders->RenderTexture(m_ppQuad, m_ppView, m_orthoProj);
	d3d->TurnZBufferOn();

	return true;
}