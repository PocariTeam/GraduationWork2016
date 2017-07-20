cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWorld;
	float4 g_vOption;
}

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
};

float4 VS( VS_IN In ) : SV_POSITION
{
	float4 Out = ( float4 )0;

	matrix mtxWVP = mul( g_mtxWorld, g_mtxLightViewProj );

	Out = mul( float4( In.vPos, 1.f ), mtxWVP );

	return Out;
}