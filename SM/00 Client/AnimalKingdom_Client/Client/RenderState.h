#pragma once

#ifndef RenderState_h__
#define RenderState_h__

class CRenderState
{
public:
	enum BL_STATE { BL_NULL, BL_ALPHA, BL_END };
	enum DS_STATE { DS_NULL, DS_NO_WRITE, DS_NO_TEST, DS_STENCIL_WRITE, DS_STENCIL_COMPARE, DS_END };
	enum RS_STATE { RS_NULL, RS_SHADOW, RS_CCW, RS_WIREFRAME, RS_NO_CULL, RS_END };

private:
	CRenderState() = default;
	~CRenderState() = default;
public:
	static void Set_DepthStencilState( ID3D11DeviceContext* pContext, DS_STATE eDS_State );
	static void Set_Rasterize( ID3D11DeviceContext* pContext, RS_STATE eRS_State );
	static void Set_BlendState( ID3D11DeviceContext* pContext, BL_STATE eBL_State );
public:
	static HRESULT Initialize( ID3D11Device* pDevice );
	static void	Release();
public:
	static ID3D11BlendState*		m_pBlendState[ BL_END ];
	static ID3D11DepthStencilState*	m_pDepthStencilState[ DS_END ];
	static ID3D11RasterizerState*	m_pRasterizerState[ RS_END ];
};

#endif // RenderState_h__



