Texture2D g_Texture[ 5 ] : register( t0 );
SamplerState g_SamplerState : register( s0 );

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;

	if( iVertexNum == 0 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( +1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 0.f ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( -1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 1.f ); }

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float4 vAlbedo = g_Texture[ 0 ].Load( float3( In.vUV, 0.f ) );
	float4 vLight = g_Texture[ 1 ].Load( float3( In.vUV, 0.f ) );
	float4 vSpecular = g_Texture[ 2 ].Load( float3( In.vUV, 0.f ) );

	float4 vOut = vAlbedo * vLight + vSpecular;
	vOut.a = 1.f;
	return vOut;
}