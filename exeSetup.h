//	This file is part of MXPTiny by BayCom GmbH.
//
//	MXPTiny is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	MXPTiny is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with MXPTiny.  If not, see <http://www.gnu.org/licenses/>.
//
//	The developer can be reached at software@baycom.tv

#pragma once
#include "resource.h"		// main symbols


// exeSetup-Dialogfeld


class exeSetup : public CDialogEx
{
	DECLARE_DYNAMIC(exeSetup)

public:
	exeSetup(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~exeSetup();

// Dialogfelddaten
	enum { IDD = IDD_EXESETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_execfg;
	CString m_vlcexe;
	CString m_default;
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_hinttext;
};
