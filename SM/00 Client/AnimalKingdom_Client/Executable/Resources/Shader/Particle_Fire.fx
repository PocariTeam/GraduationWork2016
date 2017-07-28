#define PARTICLE_TYPE_EMITTER	0

Texture2D gTexture : register( t0 );
SamplerState gSamplerState: register( s0 );

cbuffer CbMtxWorld : register( b1 )
{
	matrix g_mtxWorld;
	float4 g_vOption;
};

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	matrix g_mtxOrtho;
	matrix g_mtxViewInv;
	matrix g_mtxProjInv;
	float4 g_vCameraPos;
};

cbuffer CbParticle : register( b2 )
{
	float4 g_vParticleInfo;
};

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vVelocity : VELOCITY;
	float2 vSize : SIZE;
	float  fAge : AGE;
	uint   iType : TYPE;
};

struct VS_OUT
{
	float3 vPos : POSITION;
	float2 vSize : SIZE;
	float4 vColor : COLOR;
	uint   iType : TYPE;
};

struct GS_OUT
{
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR;
	float2 vUV   : TEXCOORD0;
};

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;
	float fT = In.fAge;
	Out.vPos = ( 0.5f * 3.f * fT * fT ) + ( In.vVelocity * fT ) + In.vPos;
	float fOpacity = 1.f - smoothstep( 0.f, 1.f, fT ) * 0.5f;
	Out.vColor = float4( 1.f, 1.f, 1.f, fOpacity );
	Out.vSize = In.vSize;
	Out.iType = In.iType;

	return Out;
}

VS_IN VS_ParticleStreamOut( VS_IN In )
{
	return In;
}

[maxvertexcount( 4 )]
void GS_ParticleStreamOut( point VS_IN In[ 1 ], inout PointStream<VS_IN> OutStream )
{
	//In[ 0 ].fAge += g_vParticle.x;

	if( In[ 0 ].iType == PARTICLE_TYPE_EMITTER ) {
		if( In[ 0 ].fAge > 0.005f ) {
			// Random

			VS_IN	vParticle;
			vParticle.vPos = float3( -200.f, 40.f, -170.f );
			//vParticle.vVelocity = g_vParticle.w * float3( -0.1f, 1.f, 0.f );
			//vParticle.vSize = float2( 4.f, 4.f ) * g_vParticle.z;
			vParticle.fAge = 0.f;
			vParticle.iType = 1;

			OutStream.Append( vParticle );
			In[ 0 ].fAge = 0.f;
		}
		OutStream.Append( In[ 0 ] );
	}

	else
		if( In[ 0 ].fAge <= 5.f ) OutStream.Append( In[ 0 ] );
}

[maxvertexcount( 4 )]
void GS( point VS_OUT In[ 1 ], inout TriangleStream<GS_OUT> OutStream )
{
	if( In[ 0 ].iType != PARTICLE_TYPE_EMITTER )
	{
		float3 vUp = float3( 0.f, 1.f, 0.f );
		float3 vLook = normalize( g_vCameraPos.xyz - In[ 0 ].vPos.xyz );
		float3 vRight = cross( vUp, vLook );

		float fHalfWidth = 0.5f * In[ 0 ].vSize.x;
		float fHalfHeight = 0.5f * In[ 0 ].vSize.y;

		float4 pVertex[ 4 ];
		pVertex[ 0 ] = float4( In[ 0 ].vPos.xyz + fHalfWidth * vRight - fHalfHeight * vUp, 1.f );
		pVertex[ 1 ] = float4( In[ 0 ].vPos.xyz + fHalfWidth * vRight + fHalfHeight * vUp, 1.f );
		pVertex[ 2 ] = float4( In[ 0 ].vPos.xyz - fHalfWidth * vRight - fHalfHeight * vUp, 1.f );
		pVertex[ 3 ] = float4( In[ 0 ].vPos.xyz - fHalfWidth * vRight + fHalfHeight * vUp, 1.f );

		float2 pUV[ 4 ];
		pUV[ 0 ] = float2( 0.f, 1.f );
		pUV[ 1 ] = float2( 0.f, 0.f );
		pUV[ 2 ] = float2( 1.f, 1.f );
		pUV[ 3 ] = float2( 1.f, 0.f );

		GS_OUT Out;
		for( int i = 0; i < 4; ++i ) {
			Out.vPos = mul( pVertex[ i ], g_mtxView );
			Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );
			Out.vUV = pUV[ i ];
			Out.vColor = In[ 0 ].vColor;
			OutStream.Append( Out );
		}
	}
}

float4 PS( GS_OUT In ) : SV_Target
{
	float4 vColor = gTexture.Sample( gSamplerState, In.vUV );

	if( In.vColor.a < 0.1f )
		discard;

	return vColor * In.vColor;
}