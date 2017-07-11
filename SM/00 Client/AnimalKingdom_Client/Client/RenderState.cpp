#include "stdafx.h"
#include "RenderState.h"
#include "Function.h"

ID3D11BlendState*			CRenderState::m_pBlendState[ BL_END ];
ID3D11DepthStencilState*	CRenderState::m_pDepthStencilState[ DS_END ];
ID3D11RasterizerState*		CRenderState::m_pRasterizerState[ RS_END ];

void CRenderState::Set_DepthStencilState( ID3D11DeviceContext* pContext, DS_STATE eDS_State )
{
	pContext->OMSetDepthStencilState( m_pDepthStencilState[ eDS_State ], 0 );
}

void CRenderState::Set_Rasterize( ID3D11DeviceContext* pContext, RS_STATE eRS_State )
{
	pContext->RSSetState( m_pRasterizerState[ eRS_State ] );
}

void CRenderState::Set_BlendState( ID3D11DeviceContext* pContext, BL_STATE eBL_State )
{
	pContext->OMSetBlendState( m_pBlendState[ eBL_State ], nullptr, 0xffffffff );
}

HRESULT CRenderState::Initialize( ID3D11Device* pDevice )
{
	m_pBlendState[ BL_NULL ] = nullptr;
	m_pDepthStencilState[ DS_NULL ] = nullptr;
	m_pRasterizerState[ RS_NULL ] = nullptr;

	/* BL_ALPHA */
	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory( &BlendStateDesc, sizeof( D3D11_BLEND_DESC ) );
	BlendStateDesc.IndependentBlendEnable = false;
	ZeroMemory( &BlendStateDesc.RenderTarget[ 0 ], sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
	BlendStateDesc.RenderTarget[ 0 ].BlendEnable = true;
	BlendStateDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState( &BlendStateDesc, &m_pBlendState[ BL_ALPHA ] );
	printf( "RefCnt : %d\n", pDevice->Release() );

	/* DS_NO_WRITE */
	D3D11_DEPTH_STENCIL_DESC DepthDesc;
	ZeroMemory( &DepthDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	DepthDesc.DepthEnable = true;
	DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	DepthDesc.StencilEnable = false;
	DepthDesc.StencilReadMask = 0xff;
	DepthDesc.StencilWriteMask = 0xff;
	DepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	DepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	pDevice->CreateDepthStencilState( &DepthDesc, &m_pDepthStencilState[ DS_NO_WRITE ] );
	printf( "RefCnt : %d\n", pDevice->Release() );

	/* DS_NO_TEST */
	DepthDesc.DepthEnable = false;
	pDevice->CreateDepthStencilState( &DepthDesc, &m_pDepthStencilState[ DS_NO_TEST ] );
	printf( "RefCnt : %d\n", pDevice->Release() );

	/* RS_NULL */
	//D3D11_RASTERIZER_DESC RSNullDesc;
	//ZeroMemory( &RSNullDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	//RSNullDesc.MultisampleEnable = TRUE;
	//RSNullDesc.AntialiasedLineEnable = TRUE;
	//pDevice->CreateRasterizerState( &RSNullDesc, &m_pRasterizerState[ RS_NULL ] );
	// pDevice->Release();
	/* RS_WIREFRAME */
	D3D11_RASTERIZER_DESC RSWireDesc;
	ZeroMemory( &RSWireDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	RSWireDesc.FillMode = D3D11_FILL_WIREFRAME;
	//RSWireDesc.MultisampleEnable = TRUE;
	RSWireDesc.CullMode = D3D11_CULL_NONE;
	RSWireDesc.FrontCounterClockwise = false;
	RSWireDesc.DepthClipEnable = true;
	//RSWireDesc.AntialiasedLineEnable = TRUE;
	pDevice->CreateRasterizerState( &RSWireDesc, &m_pRasterizerState[ RS_WIREFRAME ] );
	printf( "RefCnt : %d\n", pDevice->Release() );
	/* RS_NO_CULL */
	D3D11_RASTERIZER_DESC RSNoCull;
	ZeroMemory( &RSNoCull, sizeof( D3D11_RASTERIZER_DESC ) );
	RSNoCull.FillMode = D3D11_FILL_SOLID;
	RSNoCull.CullMode = D3D11_CULL_NONE;
	//RSNoCull.AntialiasedLineEnable = TRUE;
	//RSNoCull.MultisampleEnable = TRUE;
	RSNoCull.FrontCounterClockwise = false;
	RSNoCull.DepthClipEnable = true;
	pDevice->CreateRasterizerState( &RSNoCull, &m_pRasterizerState[ RS_NO_CULL ] );
	printf( "RefCnt : %d\n", pDevice->Release() );

	return S_OK;
}

void CRenderState::Release()
{
	for( int i = 0; i < ( int )BL_END; ++i )
		::Safe_Release( m_pBlendState[ i ] );

	for( int i = 0; i < ( int )DS_END; ++i )
		::Safe_Release( m_pDepthStencilState[ i ] );

	for( int i = 0; i < ( int )RS_END; ++i )
		::Safe_Release( m_pRasterizerState[ i ] );
}
