
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType FontVertexShader(VertexInputType input) 
{
	PixelInputType output;

	input.pos.w = 1.0f;

	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	output.tex = input.tex;
	return output;
}