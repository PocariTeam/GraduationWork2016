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

float4 VS( float3 vPos : POSITION ) : SV_POSITION
{
	float4 Out = ( float4 )0;
	matrix	mtxWVP;
	mtxWVP = mul( g_mtxWorld, g_mtxView );
	mtxWVP = mul( mtxWVP, g_mtxProj );

	Out = mul( float4( vPos.xyz, 1.f ), mtxWVP );

	return Out;
}

float4 PS( float4 In : SV_POSITION ) : SV_Target0
{
	float4 vColor = float4( 1.f, 1.f, 0.f, 1.f );

	return vColor;
}