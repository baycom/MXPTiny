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


// EncodingPresetSetup.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "DeckLinkAPI_h.h"
#include "EncodingPresetSetup.h"
#include "afxdialogex.h"

// CEncodingPresetSetup-Dialogfeld

IMPLEMENT_DYNAMIC(CEncodingPresetSetup, CDialogEx)

CEncodingPresetSetup::CEncodingPresetSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEncodingPresetSetup::IDD, pParent)
	, m_srcx(_T(""))
	, m_srcy(_T(""))
	, m_srcw(_T(""))
	, m_srch(_T(""))
	, m_dstw(_T(""))
	, m_dsth(_T(""))
	, m_has_bframes(FALSE)
{
}

CEncodingPresetSetup::~CEncodingPresetSetup()
{
}

void CEncodingPresetSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SRCX, m_srcx);
	DDX_Text(pDX, IDC_SRCY, m_srcy);
	DDX_Text(pDX, IDC_SRCW, m_srcw);
	DDX_Text(pDX, IDC_SRCH, m_srch);
	DDX_Text(pDX, IDC_DSTW, m_dstw);
	DDX_Text(pDX, IDC_DSTH, m_dsth);
	DDX_Control(pDX, IDC_COMBO_FRAMERATE, m_combo_framerate);
	DDX_Control(pDX, IDC_COMBO_PROFILE, m_combo_profile);
	DDX_Control(pDX, IDC_COMBO_LEVEL, m_combo_level);
	DDX_Control(pDX, IDC_COMBO_ENTROPY, m_combo_entropy);
	DDX_Check(pDX, IDC_CHECK_HASBFRAMES, m_has_bframes);
}


BEGIN_MESSAGE_MAP(CEncodingPresetSetup, CDialogEx)
	ON_BN_CLICKED(IDVERIFY, &CEncodingPresetSetup::OnBnClickedVerify)
END_MESSAGE_MAP()


// CEncodingPresetSetup-Meldungshandler


void CEncodingPresetSetup::updateSettings()
{
	LONGLONG val=0;
	m_em->GetInt(bmdStreamingEncodingPropertyVideoFrameRate, &val);
	m_framerate=val;
	m_em->GetInt(bmdStreamingEncodingPropertyH264Profile, &val);
	m_profile=val;
	m_em->GetInt(bmdStreamingEncodingPropertyH264Level, &val);
	m_level=val;
	m_em->GetInt(bmdStreamingEncodingPropertyH264EntropyCoding, &val);
	m_entropy=val;
	BOOL bval;
	m_em->GetFlag(bmdStreamingEncodingPropertyH264HasBFrames, &bval);
	m_has_bframes=bval;

	m_srcx.Format(_T("%d"), m_em->GetSourcePositionX());
	m_srcy.Format(_T("%d"), m_em->GetSourcePositionY());
	m_srcw.Format(_T("%d"), m_em->GetSourceWidth());
	m_srch.Format(_T("%d"), m_em->GetSourceHeight());
	m_dstw.Format(_T("%d"), m_em->GetDestWidth());
	m_dsth.Format(_T("%d"), m_em->GetDestHeight());

	int index;
	m_combo_framerate.ResetContent();
	index=m_combo_framerate.AddString(_T("50i"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate50i);
	index=m_combo_framerate.AddString(_T("59.94i"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate5994i);
	index=m_combo_framerate.AddString(_T("60i"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate60i);
	index=m_combo_framerate.AddString(_T("23.98p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate2398p);
	index=m_combo_framerate.AddString(_T("24p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate24p);
	index=m_combo_framerate.AddString(_T("25p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate25p);
	index=m_combo_framerate.AddString(_T("29.97p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate2997p);
	index=m_combo_framerate.AddString(_T("30p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate30p);
	index=m_combo_framerate.AddString(_T("50p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate50p);
	index=m_combo_framerate.AddString(_T("59.94p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate5994p);
	index=m_combo_framerate.AddString(_T("60p"));
	m_combo_framerate.SetItemData(index, bmdStreamingEncodedFrameRate60p);

	m_combo_profile.ResetContent();
	index=m_combo_profile.AddString(_T("High"));
	m_combo_profile.SetItemData(index, bmdStreamingH264ProfileHigh);
	index=m_combo_profile.AddString(_T("Main"));
	m_combo_profile.SetItemData(index, bmdStreamingH264ProfileMain);
	index=m_combo_profile.AddString(_T("Baseline"));
	m_combo_profile.SetItemData(index, bmdStreamingH264ProfileBaseline);

	m_combo_level.ResetContent();
	index=m_combo_level.AddString(_T("1.2"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level12);
	index=m_combo_level.AddString(_T("1.3"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level13);
	index=m_combo_level.AddString(_T("2"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level2);
	index=m_combo_level.AddString(_T("2.1"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level21);
	index=m_combo_level.AddString(_T("2.2"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level22);
	index=m_combo_level.AddString(_T("3"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level3);
	index=m_combo_level.AddString(_T("3.1"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level31);
	index=m_combo_level.AddString(_T("3.2"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level32);
	index=m_combo_level.AddString(_T("4"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level4);
	index=m_combo_level.AddString(_T("4.1"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level41);
	index=m_combo_level.AddString(_T("4.2"));
	m_combo_level.SetItemData(index, bmdStreamingH264Level42);
	
	m_combo_entropy.ResetContent();
	index=m_combo_entropy.AddString(_T("CAVLC"));
	m_combo_entropy.SetItemData(index, bmdStreamingH264EntropyCodingCAVLC);
	index=m_combo_entropy.AddString(_T("CABAC"));
	m_combo_entropy.SetItemData(index, bmdStreamingH264EntropyCodingCABAC);

	for(int i=0;i < m_combo_framerate.GetCount(); i++) {
		if(m_combo_framerate.GetItemData(i) == m_framerate) {
			m_combo_framerate.SetCurSel(i);
			break;
		}
	}
	for(int i=0;i < m_combo_profile.GetCount(); i++) {
		if(m_combo_profile.GetItemData(i) == m_profile) {
			m_combo_profile.SetCurSel(i);
			break;
		}
	}
	for(int i=0;i < m_combo_level.GetCount(); i++) {
		if(m_combo_level.GetItemData(i) == m_level) {
			m_combo_level.SetCurSel(i);
			break;
		}
	}
	for(int i=0;i < m_combo_entropy.GetCount(); i++) {
		if(m_combo_entropy.GetItemData(i) == m_entropy) {
			m_combo_entropy.SetCurSel(i);
			break;
		}
	}
	UpdateData(FALSE);
}

BOOL CEncodingPresetSetup::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_em = m_encoding_mode_in;
	updateSettings();
	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CEncodingPresetSetup::dlg2encoding(bool verify) 
{
	UpdateData();
	m_em->CreateMutableVideoEncodingMode(&m_mem);

	LONGLONG val=m_combo_framerate.GetItemData(m_combo_framerate.GetCurSel());
	m_mem->SetInt(bmdStreamingEncodingPropertyVideoFrameRate, val);
	
	val=m_combo_profile.GetItemData(m_combo_profile.GetCurSel());
	m_mem->SetInt(bmdStreamingEncodingPropertyH264Profile, val);
	
	val=m_combo_level.GetItemData(m_combo_level.GetCurSel());
	m_mem->SetInt(bmdStreamingEncodingPropertyH264Level, val);
	
	val=m_combo_entropy.GetItemData(m_combo_entropy.GetCurSel());
	m_mem->SetInt(bmdStreamingEncodingPropertyH264EntropyCoding, val);
	
	BOOL bval=m_has_bframes;
	m_mem->SetFlag(bmdStreamingEncodingPropertyH264HasBFrames, bval);

	m_mem->SetSourceRect(_wtoi(m_srcx), _wtoi(m_srcy), _wtoi(m_srcw), _wtoi(m_srch));
	m_mem->SetDestSize(_wtoi(m_dstw), _wtoi(m_dsth));

	BMDStreamingEncodingSupport result;
	IBMDStreamingVideoEncodingMode *em;
	BMDDisplayMode currentInputModeValue;
	
	m_streamingDeviceInput->GetCurrentDetectedVideoInputMode(&currentInputModeValue);
	m_streamingDeviceInput->DoesSupportVideoEncodingMode (currentInputModeValue, m_mem, &result, &em);
	switch(result) {
		case bmdStreamingEncodingModeSupported: 
			if(verify)
				MessageBox(_T("Streaming mode supported."), NULL);
			break;
		case bmdStreamingEncodingModeSupportedWithChanges: 
			if(m_em != m_encoding_mode_in)
				m_em->Release();
			m_mem->Release();
			em->CreateMutableVideoEncodingMode(&m_mem);
			
			updateSettings();
			if(verify)
				MessageBox(_T("Streaming supported with changes!"), _T("error"));
			break;
		case bmdStreamingEncodingModeNotSupported: 
			if(verify)
				MessageBox(_T("Streaming mode not supported!"), _T("error"));
			break;
	}
}

void CEncodingPresetSetup::OnOK()
{
	CDialogEx::OnOK();
	dlg2encoding(false);
}


void CEncodingPresetSetup::OnBnClickedVerify()
{
	dlg2encoding(true);
}
