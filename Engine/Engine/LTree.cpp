
#include "LTree.h"

LTree::LTree()
{
	m_worldMatrix = XMMatrixIdentity();
	m_isModel = true;
}

LTree::~LTree()
{
}

bool LTree::Initialise(ID3D11Device* device)
{
	if (m_isModel)
	{
		for (Model* m : m_models)
		{
			m->SetWorldMatrix(m->GetWorldMatrix() * m_worldMatrix);
			m->InitializeBuffers(device);
		}
	}
	else
	{
		D3D11_BUFFER_DESC vBufferDesc;
		D3D11_BUFFER_DESC iBufferDesc;

		D3D11_SUBRESOURCE_DATA vData;
		D3D11_SUBRESOURCE_DATA iData;

		HRESULT result;

		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;
		vBufferDesc.StructureByteStride = 0;

		vData.pSysMem = m_vertices.data();
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&vBufferDesc, &vData, &m_vBuffer);
		if (FAILED(result))
			return false;

		iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		iBufferDesc.ByteWidth = sizeof(int) * m_indices.size();
		iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iBufferDesc.CPUAccessFlags = 0;
		iBufferDesc.MiscFlags = 0;
		iBufferDesc.StructureByteStride = 0;

		iData.pSysMem = m_indices.data();
		iData.SysMemPitch = 0;
		iData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&iBufferDesc, &iData, &m_iBuffer);
		if (FAILED(result))
			return false;
	}


	return true;
}

bool LTree::Render(ID3D11DeviceContext* context)
{
	if (m_isModel)
	{
		for (Model* m : m_models)
		{
			m->Render(context);
		}
	}
	else
	{
		unsigned int stride;
		unsigned int offset;

		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType);
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		context->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		context->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		return true;
	}
}

void LTree::InterpretSystem(std::string lResult, float stepSize, float angleDelta)
{
	TurtleState curState;
	TurtleState nextState;
	curState.pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	curState.rotation = XMMatrixIdentity();
	curState.stepSize = stepSize;
	curState.radius = 1.0f;

	std::stack<int> indexStack;
	std::stack<TurtleState> turtleStack;

	XMMATRIX rotMatrix = XMMatrixIdentity();
	int index = 0;

	float origRad = curState.radius;
	float origStepSize = curState.stepSize;

	Cylinder cyl;
	cyl.GenCylinder(origRad, origStepSize, 24);

	//Simple Math implementation of stuff
	/*Vector3 Up() const { return Vector3(_21, _22, _23); }
	void Up(const Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

	Vector3 Down() const { return Vector3(-_21, -_22, -_23); }
	void Down(const Vector3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

	Vector3 Right() const { return Vector3(_11, _12, _13); }
	void Right(const Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

	Vector3 Left() const { return Vector3(-_11, -_12, -_13); }
	void Left(const Vector3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

	Vector3 Forward() const { return Vector3(-_31, -_32, -_33); }
	void Forward(const Vector3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

	Vector3 Backward() const { return Vector3(_31, _32, _33); }
	void Backward(const Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }*/

	/************************************EXPERIMENT******************************************
		Experiment Success!
		XMFLOAT4X4 rotation;
		XMStoreFloat4x4(&rotation, rotMatrix);
		XMVECTOR rot = XMVectorSet(rotation._21, rotation._22, rotation._23, rotation._24);
		if (XMComparisonAllTrue(XMVector3EqualR(rot, rotMatrix.r[1])))
		{
			//want to see something
			return;
		}
	***************************************SUCCESS!******************************************/
	//Cylinder cyl;

	for (char c : lResult)
	{
		nextState = curState;
		XMVECTOR rotated = XMVector3Transform(curState.dir, rotMatrix);

		switch (c)
		{
			case 'F':
			{
				nextState.pos = XMVectorAdd(nextState.pos, XMVectorScale(rotated, curState.stepSize));

				if (!m_isModel)
				{
					XMFLOAT3 vertPos;
					XMStoreFloat3(&vertPos, curState.pos);
					m_vertices.push_back(vertPos);

					XMStoreFloat3(&vertPos, nextState.pos);
					m_vertices.push_back(vertPos);

					m_indices.push_back(index);
					index = m_indices.size(); //can't add 1, what if we branched
					m_indices.push_back(index);
				}
				else
				{
					//cyl.GenCylinder(curState.radius, curState.stepSize, 24);
					m_models.push_back(new Model(cyl));
					//m_models.back()->SetWorldMatrix(XMMatrixScaling(curState.radius / origRad, curState.stepSize / origStepSize, curState.radius / origRad));
					m_models.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(curState.radius / origRad, curState.stepSize / origStepSize, curState.radius / origRad), rotMatrix), XMMatrixTranslationFromVector(curState.pos)));
					
				}
				break;
			}
			case 'f':
			{
				nextState.pos += curState.stepSize * rotated;
				index = m_indices.size(); //need to increase the index, in case we  go back or just so we don't draw where we shouldn't 
				break;
			}
			case '+':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], angleDelta);
				break;
			}
			case '-':
			{
				//change these back to 2!
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], -angleDelta);
				break;
			}
			case '&':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], angleDelta);
				break;
			}
			case '^':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], -angleDelta);
				break;
			}
			case '\\':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], angleDelta);
				break;
			}
			case '/':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], -angleDelta);
				break;
			}
			case '|':
			{
				rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], (180.0f * XM_PI) / 180);
				break;
			}
			case '[':
			{
				//nextState.stepSize = curState.stepSize + 1.0f;
				nextState.radius = curState.radius - 0.3f;
				curState.rotation = rotMatrix;
				turtleStack.push(curState);
				indexStack.push(index);
				break;
			}
			case ']':
			{
				nextState = turtleStack.top();
				rotMatrix = nextState.rotation;
				turtleStack.pop();
				index = indexStack.top();
				indexStack.pop();
				break;
			}
			default:
				break;
		}
		curState = nextState;
	}
}

std::vector<Model*> LTree::GetModels()
{
	return m_models;
}

void LTree::SetWorldMatrix(XMMATRIX world)
{
	m_worldMatrix = world;
}

XMMATRIX LTree::GetWorldMatrix()
{
	return m_worldMatrix;
}

int LTree::GetIndexCount()
{
	return m_indices.size();
}

bool LTree::IsModel()
{
	return m_isModel;
}