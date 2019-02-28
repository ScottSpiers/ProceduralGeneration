////////////////////////////////////////////////////////////////////////////////
// Filename: Terrain.vs
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

cbuffer CameraBuffer
{
	float3 cameraPos;
	float pad;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 viewDir : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TerrainVertexShader(VertexInputType input)
{
   PixelInputType output;
	float4 worldPos;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	//Calc pos of vertex in world
	worldPos = mul(input.position, worldMatrix);

	//Determine viewing dir based on camera pos and vertex pos in world
	output.viewDir =   cameraPos.xyz - worldPos.xyz;

	//normalise
	output.viewDir = normalize(output.viewDir);

    return output;
}