float4 Lighting( float3 vPos, float3 vNormal );
/* VS */
cbuffer CbMtxWorld : register( b0 )
{
	matrix g_mtxWorld;
};

cbuffer CbMtxViewProj : register( b1 )
{
	matrix g_mtxView;
	matrix g_mtxProj;
	float4 g_vCameraPos;
};

cbuffer CbLight : register( b2 )
{
	float4 g_vLightDir;
};

/* PS */
cbuffer CbLight : register( b2 )
{
	float4	 g_vLightDirection;
	float4	 g_vDiffuseLight;
	float4	 g_vAmbientLight;
	float4	 g_vSpecularLight;
};

cbuffer CbTextureInfo : register( b3 )
{
	float4   g_vTextureInfo;
}

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV	   : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float3 vPosW : POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vUV	   : TEXCOORD0;
};

/* PS */
Texture2D gDiffuseTexture : register( t0 );
Texture2D gNormalTexture : register( t1 );
SamplerState gSamplerState : register( s0 );

VS_OUT VS( VS_IN In )
{
	VS_OUT Out = ( VS_OUT )0;
	Out.vPos = mul( float4( In.vPos, 1.f ), g_mtxWorld );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxView );
	Out.vPos = mul( float4( Out.vPos.xyz, 1.f ), g_mtxProj );
	Out.vNormal = mul( In.vNormal, ( float3x3 )g_mtxWorld );
	Out.vTangent = mul( In.vTangent, (float3x3)g_mtxWorld );
	Out.vPosW = (float3)mul( float4( In.vPos.xyz, 1.f ), g_mtxWorld );


	Out.vUV = In.vUV;

	return Out;
}

float4 PS( VS_OUT In ) : SV_Target
{
	float3 N = normalize( In.vNormal );
	
	if( g_vTextureInfo.x >= 2.f )
	{
		float3 T = normalize( In.vTangent );
		float3 B = cross( N, T );

		float3x3 TBN = float3x3( T, B, N );
		float3 vNormal = gNormalTexture.Sample( gSamplerState, In.vUV ).rgb;
		vNormal = 2.f * vNormal - 1.f;
		float3 vNormalW = mul( vNormal, TBN );

		return gDiffuseTexture.Sample( gSamplerState, In.vUV ) * Lighting( In.vPosW, vNormalW );
	}
		

	else
		return gDiffuseTexture.Sample( gSamplerState, In.vUV ) * Lighting( In.vPosW, N );
}

float4 Lighting( float3 vPos, float3 vNormal )
{
	float3 vCameraPos = g_vCameraPos.xyz;
	float3 vToCamera = normalize( vCameraPos - vPos );
	float4 cColor = ( float4 )0;
	float3 vSpecular = ( float3 )0;
	// For.Specular
	float fDiffuseFactor = dot( -g_vLightDirection.xyz, vNormal );
	float3 vDiffuse = ( float3 )0;
	float3 vAmbient = ( float3 )0;

	if( fDiffuseFactor > 0.f ) {
		float3 vReflect = reflect( g_vLightDirection.xyz, vNormal );
		float fSpecularFactor = pow( max( dot( vReflect, vToCamera ), 0.0f ), 0.5f );

		vSpecular = float3( 1.f, 1.f, 1.f ) * ( g_vSpecularLight.xyz * fSpecularFactor );
		vDiffuse = float3( 1.f, 1.f, 1.f ) * ( g_vDiffuseLight.xyz * fDiffuseFactor );
	}
	vAmbient = float3( 0.5f, 0.5f, 0.5f ) * g_vAmbientLight.xyz;

	cColor += float4( vAmbient * 2.f + vDiffuse + vSpecular, 0.f );
	cColor.a = 1.f;

	return( cColor );
}