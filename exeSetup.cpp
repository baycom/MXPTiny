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


// exeSetup.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "exeSetup.h"
#include "afxdialogex.h"


// exeSetup-Dialogfeld

IMPLEMENT_DYNAMIC(exeSetup, CDialogEx)

exeSetup::exeSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(exeSetup::IDD, pParent)
	, m_vlcexe(_T(""))
	, m_hinttext(_T(""))
{

}

exeSetup::~exeSetup()
{
}

void exeSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_execfg);
	DDX_Text(pDX, IDC_EDIT1, m_vlcexe);
	DDX_Text(pDX, IDC_EDIT2, m_hinttext);
}


BEGIN_MESSAGE_MAP(exeSetup, CDialogEx)
	ON_BN_CLICKED(IDRESET, &exeSetup::OnBnClickedReset)
	ON_BN_CLICKED(IDOK, &exeSetup::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &exeSetup::OnBnClickedCancel)
END_MESSAGE_MAP()


// exeSetup-Meldungshandler


void exeSetup::OnBnClickedReset()
{
	m_vlcexe = m_default;
	m_execfg.SetWindowText(m_vlcexe);
}


void exeSetup::OnBnClickedOk()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	CDialogEx::OnOK();
}


void exeSetup::OnBnClickedCancel()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	CDialogEx::OnCancel();
}
