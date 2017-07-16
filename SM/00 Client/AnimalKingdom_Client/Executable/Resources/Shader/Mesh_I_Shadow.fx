cbuffer cbShadow : register( b3 )
{
	matrix g_mtxLightView;
	matrix g_mtxLightProj;
	float4 g_vLightPos;
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

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( In.mtxWorld, g_mtxLightView );
	mtxWVP = mul( mtxWV, g_mtxLightProj );

	Out = mul( float4( In.vPos, 1.f ), mtxWVP );

	return Out;
}