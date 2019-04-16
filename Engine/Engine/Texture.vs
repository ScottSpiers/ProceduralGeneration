////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ScreenBuffer
{
	float2 screenDims;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;

	//neighbours
	float2 n0 : TEXCOORD1;
	float2 n1 : TEXCOORD2;
	float2 n2 : TEXCOORD3;
	float2 n3 : TEXCOORD4;
	float2 n4 : TEXCOORD5;
	float2 n5 : TEXCOORD6;
	float2 n6 : TEXCOORD7;
	float2 n7 : TEXCOORD8;
	float2 n8 : TEXCOORD9;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the pos vector to be 4 units for proper matrix calculations.
    input.pos.w = 1.0f;

	// Calculate the pos of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	float texelWidth = 1 / screenDims.x;
	float texelHeight = 1 / screenDims.y;

	output.n0 = input.tex + float2(-texelWidth, -texelHeight);
	output.n1 = input.tex + float2(0.0f, -texelHeight);
	output.n2 = input.tex + float2(texelWidth, -texelHeight);

	output.n3 = input.tex + float2(-texelWidth, 0.0f);
	output.n4 = input.tex + float2(0.0f, 0.0f); //this is where we are
	output.n5 = input.tex + float2(texelWidth, 0.0f);

	output.n6 = input.tex + float2(-texelWidth, texelHeight);
	output.n7 = input.tex + float2(0.0f, texelHeight);
	output.n8 = input.tex + float2(texelWidth, texelHeight);

    return output;
}