
#include "Text.h"

Text::Text()
{
	m_font = 0;
	m_fontShader = 0;
	m_fpsSentence = 0;
}

Text::Text(const Text&)
{
}

Text::~Text()
{
}

bool Text::Initialise(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX view)
{
	bool res;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_viewMatrix = view;

	m_font = new Font;
	if (!m_font)
		return false;

	res = m_font->Initialise(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialise the font object", L"Error", MB_OK);
		return false;
	}

	m_fontShader = new FontShader;
	if(!m_fontShader)
		return false;

	res = m_fontShader->Initialise(device, hwnd);
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialise the font shader object", L"Error", MB_OK);
		return false;
	}

	res = InitialiseSentence(&m_fpsSentence, 16, device);
	if (!res)
		return false;

	res = updateSentence(m_fpsSentence, "Test", 10, 10, .0f, 1.0f, .0f, deviceContext);
	if (!res)
		return false;

	return true;
}

void Text::Shutdown()
{
	if (m_fpsSentence)
	{
		// Release the sentence vertex buffer.
		if (m_fpsSentence->vertexBuffer)
		{
			m_fpsSentence->vertexBuffer->Release();
			m_fpsSentence->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if (m_fpsSentence->indexBuffer)
		{
			m_fpsSentence->indexBuffer->Release();
			m_fpsSentence->indexBuffer = 0;
		}

		// Release the sentence.
		delete m_fpsSentence;
		m_fpsSentence = 0;
	}

	if (m_fontShader)
	{
		m_fontShader->Shutdown();
		delete m_fontShader;
		m_fontShader = 0;
	}

	if (m_font)
	{
		m_font->Shutdown();
		delete m_font;
		m_font = 0;
	}
}

bool Text::setText(const char* sentence, ID3D11DeviceContext* deviceContext)
{
	return updateSentence(m_fpsSentence, sentence, 10, 10, 0.01f, 0.01f, 0.01f, deviceContext);
}



bool Text::Render(ID3D11DeviceContext* deviceContext, XMMATRIX world, XMMATRIX ortho)
{
	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool res;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_fpsSentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_fpsSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = XMFLOAT4(m_fpsSentence->red, m_fpsSentence->green, m_fpsSentence->blue, 1.0f);

	// Render the text using the font shader.
	res = m_fontShader->Render(deviceContext, m_fpsSentence->indexCount, world, m_viewMatrix, ortho, m_font->getTexture(), pixelColor);
	if (!res)
		return false;

	return true;
}

bool Text::InitialiseSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT res;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence)
		return false;

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (int i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	res = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(res))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	res = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(res))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

bool Text::updateSentence(SentenceType* sentence, const char* text, int posX, int posY, float r, float g, float b, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	

	// Store the color of the sentence.
	sentence->red = r;
	sentence->green = g;
	sentence->blue = b;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));
	
	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + posX);
	drawY = (float)((m_screenHeight / 2) - posY);

		// Use the font class to build the vertex array from the sentence text and sentence draw location.
		m_font->buildVertexArray((void*)vertices, text, drawX, drawY);
	

		// Lock the vertex buffer so it can be written to.
	res = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

