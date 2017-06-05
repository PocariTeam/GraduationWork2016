Texture2D Texture : register( t0 );
SamplerState g_SamplerState : register( s0 );

cbuffer cbWorld : register( b0 )
{
	matrix g_mtxWorld;
	float4 g_vOption;
}

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	matrix g_mtxOrtho;
	matrix g_mtxViewInv;
	matrix g_mtxProjInv;
	float4 g_vCameraPos;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;
	
	if( iVertexNum == 0 ) { Out.vPos = float4( -0.5f, 0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.z ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( 0.5f, -0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.w ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( -0.5f, -0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.w ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( -0.5f, 0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.x, g_vOption.z ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( 0.5f, 0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.z ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( 0.5f, -0.5f, 0.f, 1.f ); Out.vUV = float2( g_vOption.y, g_vOption.w ); }

	matrix mtxBill = g_mtxView;
	mtxBill._41 = 0.f;
	mtxBill._42 = 0.f;
	mtxBill._43 = 0.f;

	Out.vPos = mul( mtxBill, Out.vPos );
	Out.vPos = mul( Out.vPos, g_mtxWorld );
	Out.vPos = mul( Out.vPos, g_mtxView );
	Out.vPos = mul( Out.vPos, g_mtxProj );


	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	return Texture.Sample( g_SamplerState, In.vUV );
}