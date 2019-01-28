#pragma once


#include "Font.h"
#include "FontShader.h"

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
		D3DXVECTOR3 pos;
		D3DXVECTOR2 texture;
	};

public:
	Text();
	Text(const Text&);
	~Text();

	bool Initialise(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);

	bool setText(const char*, ID3D11DeviceContext*);

private:
	bool InitialiseSentence(SentenceType**, int, ID3D11Device*);
	bool updateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);

private:
	Font* m_font;
	FontShader* m_fontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_viewMatrix;
	SentenceType* m_fpsSentence;
};