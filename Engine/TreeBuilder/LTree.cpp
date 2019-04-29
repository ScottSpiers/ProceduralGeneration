#include "pch.h"
#include "LTree.h"

LTree::LTree()
{
	m_worldMatrix = XMMatrixIdentity();
	m_isModel = true;
	m_bigModel = 0;
	m_radius = 1.0f;
}

LTree::LTree(float radius) : LTree()
{
	m_radius = radius;
}

LTree::~LTree()
{
	if (m_bigModel)
	{
		delete m_bigModel;
		m_bigModel = 0;
	}
}

Model* LTree::InterpretSystem(std::string lResult, float stepSize, float angleDelta)
{
	TurtleState curState;
	TurtleState nextState;
	curState.pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	curState.rotation = XMMatrixIdentity();
	curState.stepSize = stepSize;
	curState.radius = m_radius;

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
					m_models.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(curState.radius / origRad, curState.stepSize / origStepSize, curState.radius / origRad), rotMatrix), XMMatrixTranslationFromVector(curState.pos)));
					
				}
				nextState.radius = curState.radius - 0.005f;
				if (nextState.radius < 0.05f)
					nextState.radius = 0.05f;
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
				//make this a percentage of the curradius?
				nextState.radius = curState.radius - 0.3f;
				if (nextState.radius < 0.05f)
					nextState.radius = 0.05f;
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

	m_bigModel = new Model();
	m_bigModel->SetWorldMatrix(m_models[0]->GetWorldMatrix());

	int numVerts = 0;
	int numIndices= 0;
	std::vector<Model::VertexType> finalVerts;
	std::vector<unsigned int> finalInds;

	for (Model* m : m_models)
	{
		numVerts += m->GetVertexCount();
		numIndices += m->GetIndexCount();
	}

	finalVerts.reserve(numVerts);
	finalInds.reserve(numIndices);

	for (Model* m : m_models)
	{
		std::vector<unsigned int> tempInds = m->GetIndices();
		std::vector<Model::VertexType> tempVerts = m->GetVertices();

		for (auto& v : tempVerts)
		{
			XMVECTOR pos = XMLoadFloat3(&v.position);
			XMVECTOR newPos = XMVector3TransformCoord(pos, m->GetWorldMatrix());
			XMStoreFloat3(&v.position, newPos);
		}

		int accIndSize = finalVerts.size();
		for (auto& ind : tempInds)
		{
			ind += accIndSize;
		}
		finalInds.insert(finalInds.end(), tempInds.begin(), tempInds.end());
		finalVerts.insert(finalVerts.end(), tempVerts.begin(), tempVerts.end());
	}

	m_bigModel->SetModelData(finalVerts, finalInds);

	m_models.clear();
	return m_bigModel;

}

void LTree::SetRadius(float r)
{
	m_radius = r;
}

float LTree::GetRadius()
{
	return m_radius;
}

Model* LTree::GetModel()
{
	return m_bigModel;
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