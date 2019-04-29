#pragma once

#include <DirectXMath.h>
#include "Font.h"
#include "FontShader.h"

using namespace DirectX;

class Text
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 texture;
	};

public:
	Text();
	Text(const Text&);
	~Text();

	bool Initialise(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);

	bool setText(const char*, ID3D11DeviceContext*);
	bool setText(const char*, ID3D11DeviceContext*, int posX, int posY);

private:
	bool InitialiseSentence(SentenceType**, int, ID3D11Device*);
	bool updateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);

private:
	Font* m_font;
	FontShader* m_fontShader;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_viewMatrix;
	SentenceType* m_fpsSentence;
};