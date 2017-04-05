#define LIGHT_CNT		1
#define DIRECTIONAL		0.f
#define POINT			1.0f
#define SPOT			2.0f

struct LIGHT
{
	float	fType;
	float3	vPos;
	float4	vDiffuse;
	float4	vAmbient;
	float4	vSpecular;
	float3	vDir;
	float	fRange;
	float3	vAttenuation;
	float	fFallOff;
	float	fPhi;
	float	fTheta;
	float2	vPadding;
};

matrix	g_mtxWinSize : register( b0 );

cbuffer cbCamera : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
}

cbuffer cbLight : register( b2 )
{
	LIGHT	g_tLight[ LIGHT_CNT ];
}

Texture2D g_NormalTexture : register( t0 );
Texture2D g_DepthTexture : register( t1 );


float4 Directional_Specular( LIGHT tLight, float4 vWorldNormal, float3 vLookInv );
float4 Point_Lighting( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal );
float4 Point_Specular( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal, float3 vLookInv );
float4 Spot_Lighting( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal );
float4 Spot_Specular( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal, float3 vLookInv );

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vUV : TEXCOORD0;
	float2 vTexUV : TEXCOORD1;
};

struct PS_OUT
{
	float4	vLight : SV_Target0;
	float4	vSpecular : SV_Target1;
};

VS_OUT VS( uint iVertexNum : SV_VertexID )
{
	VS_OUT Out = ( VS_OUT )0;

	if( iVertexNum == 0 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f );  Out.vTexUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 1 ) { Out.vPos = float4( +1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 0.f ); Out.vTexUV = float2( g_mtxWinSize._11, 0.f ); }
	else if( iVertexNum == 2 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); Out.vTexUV = float2( g_mtxWinSize._11, g_mtxWinSize._21 ); }
	else if( iVertexNum == 3 ) { Out.vPos = float4( -1.f, +1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 0.f ); Out.vTexUV = float2( 0.f, 0.f ); }
	else if( iVertexNum == 4 ) { Out.vPos = float4( +1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 1.f, 1.f ); Out.vTexUV = float2( g_mtxWinSize._11, g_mtxWinSize._21 ); }
	else if( iVertexNum == 5 ) { Out.vPos = float4( -1.f, -1.f, 0.5f, 1.f ); Out.vUV = float2( 0.f, 1.f ); Out.vTexUV = float2( 0.f, g_mtxWinSize._21 ); }

	return Out;
}

PS_OUT	PS( VS_OUT	In )
{
	PS_OUT		Out = ( PS_OUT )0;

	float3		vNormal = g_NormalTexture.Load( float3( In.vTexUV.x, In.vTexUV.y, 0.f ) ).xyz;
	float3		vWorldNormal = mad( vNormal.xyz, 2.f, -1.f );

	float3		vDepth = g_DepthTexture.Load( float3( In.vTexUV.x, In.vTexUV.y, 0.f ) ).xyz;
	float		fViewZ = vDepth.y * 1000.f;

	float4		vPos = ( float4 )0;

	vPos.x = mad( In.vUV.x, 2.f, -1.f ) * fViewZ;
	vPos.y = mad( In.vUV.y, -2.f, 1.f ) * fViewZ;
	vPos.z = vDepth.x * fViewZ;
	vPos.w = fViewZ;

	vPos = mul( g_mtxProj, vPos );
	vPos = mul( g_mtxView, vPos );

	float3 vLookInv = normalize( g_vCameraPos.xyz - vPos.xyz );

	int i = 0;
	float4	vLight = ( float4 )0;
	float4	vSpecular = ( float4 )0;

	for( i = 0; i < LIGHT_CNT; ++i )
	{
		switch( g_tLight[ i ].fType )
		{
		case DIRECTIONAL :
			vLight += saturate( mad( max( dot( -g_tLight[ i ].vDir, vWorldNormal ), 0.f ), g_tLight[ i ].vDiffuse, g_tLight[ i ].vAmbient ) );
			vSpecular += Directional_Specular( g_tLight[ i ], float4( vWorldNormal, 0.f ), vLookInv );
			break;
		case POINT :
			vLight += Point_Lighting( g_tLight[ i ], vPos.xyz, vWorldNormal.xyz );
			vSpecular += Point_Specular( g_tLight[ i ], vPos.xyz, vWorldNormal, vLookInv );
			break;
		case SPOT :
			vLight += Spot_Lighting( g_tLight[ i ], vPos.xyz, vWorldNormal.xyz );
			vSpecular += Spot_Specular( g_tLight[ i ], vPos.xyz, vWorldNormal, vLookInv );
			break;
		}
	}

	Out.vLight = saturate( vLight );
	Out.vSpecular = saturate( vSpecular );

	return Out;
}

float4 Directional_Specular( LIGHT tLight, float4 vWorldNormal, float3 vLookInv )
{
	float4 vSpecular = ( float4 )0;

	float4 vReflect = reflect( float4( normalize( tLight.vDir ), 0.f ), vWorldNormal );
	vSpecular = pow( max( dot( vLookInv, normalize( vReflect ).xyz ), 0.f ), tLight.vSpecular.w );

	return vSpecular;
}

float4 Point_Lighting( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal )
{
	float4 vLight = ( float4 )0;

	float3 vToLight = tLight.vPos - vWorldPos;
	float fDistance = length( vToLight );

	if( fDistance > tLight.fRange ) return vLight;

	vToLight /= fDistance;
	float	fDiffuseFactor = max( dot( vToLight, vWorldNormal ), 0.f );

	if( fDiffuseFactor > 0.f )
		vLight = tLight.vDiffuse * fDiffuseFactor;

	float	fAttenuationFactor = dot( tLight.vAttenuation, float3( 1.f, fDistance, fDistance * fDistance ) );
	vLight += tLight.vAmbient;
	vLight *= fAttenuationFactor;

	return vLight;
}

float4 Point_Specular( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal, float3 vLookInv )
{
	float4 vSpecular = ( float4 )0;

	float3 vToLight = tLight.vPos - vWorldPos;
	float fDistance = length( vToLight );

	if( fDistance > tLight.fRange ) return vSpecular;
	if( tLight.vSpecular.w <= 0.f ) return vSpecular;

	float3	vToLightInv = vWorldPos - tLight.vPos;
	float3	vReflect = reflect( vToLightInv, vWorldNormal );
	float	fSpecularFactor = pow( max( dot( vReflect, vLookInv ), 0.f ), tLight.vSpecular.w );
	vSpecular = tLight.vSpecular * fSpecularFactor;

	float fAttenuationFactor = dot( tLight.vAttenuation, float3( 1.0f, fDistance, fDistance * fDistance ) );
	vSpecular *= fAttenuationFactor;
	vSpecular.w = 0.f;

	return vSpecular;
}

float4 Spot_Lighting( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal )
{
	float4 vLight = ( float4 )0;

	float3 vToLight = tLight.vPos - vWorldPos;
	float fDistance = length( vToLight );

	if( fDistance > tLight.fRange ) return vLight;

	vToLight /= fDistance;
	float	fDiffuseFactor = max( dot( vToLight, vWorldNormal ), 0.f );

	if( fDiffuseFactor > 0.f )
		vLight = tLight.vDiffuse * fDiffuseFactor;

	// Alpha ¶û SpotFactor ¾È¾²´Â Áß

	float fAlpha = max( dot( -vToLight, tLight.vDir ), 0.f );
	float fSpotFactor = pow( max( ( ( fAlpha - tLight.fPhi ) / ( tLight.fTheta - tLight.fPhi ) ), 0.f ), tLight.fFallOff );
	float fAttenuationFactor = 1.f / dot( tLight.vAttenuation, float3( 1.f, fDistance, fDistance * fDistance ) );
	
	vLight += tLight.vAmbient;
	vLight *= fAttenuationFactor;

	return vLight;
}

float4 Spot_Specular( LIGHT tLight, float3 vWorldPos, float3 vWorldNormal, float3 vLookInv )
{
	float4 vSpecular = ( float4 )0;

	float3 vToLight = tLight.vPos - vWorldPos;
	float fDistance = length( vToLight );

	if( fDistance > tLight.fRange ) return vSpecular;
	if( tLight.vSpecular.w <= 0.f ) return vSpecular;

	float3	vReflect = reflect( -vToLight, vWorldNormal );
	float	fSpecularFactor = pow( max( dot( vReflect, vLookInv ), 0.f ), tLight.vSpecular.w );
	vSpecular = tLight.vSpecular * fSpecularFactor;

	float fAttenuationFactor = 1.f / dot( tLight.vAttenuation, float3( 1.0f, fDistance, fDistance * fDistance ) );
	vSpecular *= fAttenuationFactor;
	vSpecular.w = 0.f;
	return vSpecular;
}