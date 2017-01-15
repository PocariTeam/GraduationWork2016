#pragma once

#include "Tool_Header.h"
// CTool_View 뷰입니다.

class CGraphicDev;
class CImporter;
class CShader;
class CCamera;
class CLight;
class CTool_View : public CView
{
	DECLARE_DYNCREATE(CTool_View)

protected:
	CTool_View();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CTool_View();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	void	CreateRasterizerState();

public:
	CGraphicDev*			m_pGraphicDev;
	vector< pair<string, CImporter*> > m_vecImporter;
	CImporter*				m_pActiveImporter;
	CCamera*				m_pCamera;
	CLight*					m_pLight;
	ID3D11RasterizerState*	m_pRSState;

	bool					m_bWirefream;
	bool					m_bPause;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
};


