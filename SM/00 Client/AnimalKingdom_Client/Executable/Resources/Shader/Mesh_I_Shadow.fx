cbuffer cbShadow : register( b3 )
{
	matrix g_mtxLightViewProj;
}

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV : TEXCOORD;
	matrix mtxWorld : WORLD;
	float4 vOption : OPTION;
};

float4 VS( VS_IN In ) : SV_POSITION
{
	float4 Out = ( float4 )0;

	matrix mtxWVP = mul( In.mtxWorld, g_mtxLightViewProj );

	Out = mul( float4( In.vPos, 1.f ), mtxWVP );

	return Out;
}