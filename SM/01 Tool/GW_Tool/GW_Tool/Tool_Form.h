#pragma once
#include "afxwin.h"
#include "Tool_Header.h"
#include "afxcmn.h"
#pragma warning( disable : 4800 )

// CTool_Form 폼 뷰입니다.

class CTool_Form : public CFormView
{
	DECLARE_DYNCREATE(CTool_Form)

protected:
	CTool_Form();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CTool_Form();

public:
	enum LISTBOX_TYPE { LISTBOX_MESH, LISTBOX_ANIMATION, LISTBOX_END };
	enum CHECK_TYPE { CHECK_WIREFRAME, CHECK_END };
	enum EDIT_TYPE { EDIT_START, EDIT_LAST, EDIT_SPEED, EDIT_END	};
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles( HDROP hDropInfo );
private:
	inline wstring ConvertRelativePath( wstring szFullPath );
	void	Print_ListBox_Animation( void );
	void	Active_Slider();
public:
	CListBox m_mfcListBox[ LISTBOX_END ];
	CButton m_mfcCheckBox[ CHECK_END ];
	CSliderCtrl m_mfcSlider;
	CEdit m_mfcEdit[ EDIT_END ];
	CSpinButtonCtrl m_mfcSphine;

	afx_msg void OnLbnListBox_Mesh();
	afx_msg void OnBn_CheckBox_WireFrame();
	afx_msg void OnBnExport();
	afx_msg void OnBnPlay();
	virtual void OnInitialUpdate();
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnLbnListBox_Animation();
};


