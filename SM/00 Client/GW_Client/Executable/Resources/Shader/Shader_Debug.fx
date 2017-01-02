/* VS */
cbuffer CbMtxViewProj : register( b0 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

cbuffer CbMtxWorld : register( b1 )
{
	matrix g_mtxWorld;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;
	Out.vPos = mul( float4( In.vPos.xyz, 1.f ), g_mtxWorld );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxView );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	 float4 vColor = float4( 1.f, 1.f, 1.f, 1.f );

	return vColor;
}