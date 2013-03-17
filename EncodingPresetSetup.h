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


// CEncodingPresetSetup-Dialogfeld

class CEncodingPresetSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CEncodingPresetSetup)

public:
	CEncodingPresetSetup(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CEncodingPresetSetup();

// Dialogfelddaten
	enum { IDD = IDD_ENCODING_PRESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	CString m_srcx;
	CString m_srcy;
	CString m_srcw;
	CString m_srch;
	CString m_dstw;
	CString m_dsth;
	CComboBox m_combo_framerate;
	CComboBox m_combo_profile;
	CComboBox m_combo_level;
	CComboBox m_combo_entropy;
	BOOL m_has_bframes;
	LONGLONG m_framerate;
	LONGLONG m_profile;
	LONGLONG m_level;
	LONGLONG m_entropy;
	IBMDStreamingDeviceInput* m_streamingDeviceInput;
	IBMDStreamingVideoEncodingMode *m_encoding_mode_in;
	IBMDStreamingVideoEncodingMode* m_em;
	IBMDStreamingMutableVideoEncodingMode *m_mem;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedVerify();
	void dlg2encoding(bool verify);
	void updateSettings();
};
