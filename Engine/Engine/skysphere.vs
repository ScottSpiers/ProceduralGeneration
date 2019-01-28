
//Globals

cbuffer MatrixBuffer 
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

//Typedefs

struct VertexInputType
{
	float4 pos : POSITION;
	//float2 texCoord : TEXCOORD0;
	//float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float3 texCoord : TEXCOORD0;
};

//Vertex Shader

PixelInputType SkySphereVertexShader(VertexInputType input) 
{
	PixelInputType output;

	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix).xyww;
	
	output.texCoord = input.pos.xyz;
	return output;
}