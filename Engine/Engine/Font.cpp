
#include "Font.h"

Font::Font()
{
	m_font = 0;
	m_texture = 0;
}

Font::Font(const Font&)
{
}

Font::~Font()
{
}

bool Font::Initialise(ID3D11Device* device, char* fontFile, WCHAR* textureFile)
{
	if (!loadFont(fontFile))
		return false;

	if (!loadTexture(device, textureFile))
		return false;

	return true;
}

void Font::Shutdown()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_font)
	{
		delete m_font;
		m_font = 0;
	}
}

bool Font::loadFont(char* filename)
{
	std::ifstream fin;
	char temp;

	m_font = new FontType[95];
	if (!m_font)
		return false;

	fin.open(filename);
	if (fin.fail())
		return false;

	for (int i = 0; i < 95; ++i)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_font[i].left;
		fin >> m_font[i].right;
		fin >> m_font[i].size;
	}
}

bool Font::loadTexture(ID3D11Device* device, WCHAR* filename)
{
	
	//D3DX11CreateShaderResourceViewFromFile(m_device, filename, NULL, NULL, &m_texture, NULL);
	/*m_texture = new Texture;
	if (!m_texture)
		return false;

	if (!m_texture->Initialize(device, filename))
		return false;*/

	return true;
}

ID3D11ShaderResourceView* Font::getTexture()
{
	return m_texture;
}

void Font::buildVertexArray(void* vertices, const char* sentence, float x, float y)
{
	VertexType* ptr_vertex;
	int numLetters;
	int index;
	int letter;

	ptr_vertex = (VertexType*)vertices;
	numLetters = strlen(sentence);
	index = 0;

	for (int i = 0; i < numLetters; ++i)
	{
		letter = sentence[i] - 32;
		if (letter == 0)
		{
			x += 3.0f;
		}
		else
		{
			//first triangle
			ptr_vertex[index].pos = XMFLOAT3(x, y, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			++index;

			ptr_vertex[index].pos = XMFLOAT3(x + m_font[letter].size, y - 16, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			++index;

			ptr_vertex[index].pos = XMFLOAT3(x, y - 16, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			++index;

			//second triangle
			ptr_vertex[index].pos = XMFLOAT3(x, y, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			++index;

			ptr_vertex[index].pos = XMFLOAT3(x + m_font[letter].size, y, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			++index;

			ptr_vertex[index].pos = XMFLOAT3(x + m_font[letter].size, y - 16, 0.0f);
			ptr_vertex[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			++index;

			x += m_font[letter].size + 1.0f;
		}
	}
}