Texture2D gTexture : register( t0 );
SamplerState gSamplerState : register( s0 );

matrix g_mtxWorld : register( b0 );

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float3 vWorldPos : POSITION;
	float2 vUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vNormal : SV_Target0;
	float4 vDepth : SV_Target1;
	float4 vAlbedo : SV_Target2;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;

	matrix	mtxWV, mtxWVP;
	mtxWV = mul( g_mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWV, g_mtxProj );

	Out.vPos = mul( float4( In.vPos, 1.f ), mtxWVP );
	Out.vWorldPos = mul( float4( In.vPos, 1.f ), g_mtxWorld ).xyz;
	Out.vUV = In.vUV;

	Out.vNormal = normalize( mul( float4( In.vNormal, 0.f ), g_mtxWorld ) ).xyz;
	Out.vTangent = normalize( mul( float4( In.vTangent, 0.f ), g_mtxWorld ) ).xyz;

	return Out;
}

PS_OUT PS( VS_OUT In )
{
	PS_OUT	Out = ( PS_OUT )0;

	Out.vAlbedo = gTexture.Sample( gSamplerState, In.vUV );
	Out.vNormal = float4( In.vNormal.xyz * 0.5f + 0.5f, 0.f );
	Out.vDepth = float4( In.vPos.z / In.vPos.w, In.vPos.w / 1000.f, 0.f, 0.f );

	return Out;
}