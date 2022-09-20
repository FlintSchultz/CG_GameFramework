struct VS_IN
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float4 normal : NORMAL;
};

struct LightData
{
	float4 direction;
	float4 color;
	float4 viewerPos;
};

cbuffer LightsBuf:register(b1)
{
	LightData lightData;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float4 worldPos : TEXCOORD1;
	float4 normal :NORMAL;
};

struct ConstantData
{
	float4x4 worldViewProj;
	float4x4 world;
	float4x4 invertedWorldTransform;
};

cbuffer ConstBuf:register(b0)
{
	ConstantData constData;
};

PS_IN VSMain(VS_IN input)
{

	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), constData.worldViewProj);
	output.worldPos = mul(float4(input.pos.xyz, 1.0f), constData.world);
	output.col = input.col;
	output.normal = mul(float4(input.normal.xyz, 1.0f), constData.invertedWorldTransform);
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{	 

	float4 color = input.col;

	float3 LightDir = lightData.direction.xyz;
	float3 normal = normalize(input.normal.xyz);

	float3 viewDir = normalize(lightData.viewerPos.xyz - input.worldPos.xyz);
	float3 refVec = normalize(reflect(LightDir, normal));

	float3 ambient = color.xyz * 0.5;
	float3 diffuse = saturate(dot(LightDir, normal)) * color.xyz;
	float3 specular = pow(saturate(dot(-viewDir, refVec)), 0.7) * 0.3;

	return float4(lightData.color.xyz * (ambient + diffuse + specular), 1.0f);
}