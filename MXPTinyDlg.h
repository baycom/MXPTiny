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

#include "DeckLinkAPI_h.h"
#include "afxwin.h"
#include "afxcmn.h"

// CMXPTinyDlg dialog
class CMXPTinyDlg : public CDialog,
	public IBMDStreamingDeviceNotificationCallback,
	public IBMDStreamingH264InputCallback
{
// Construction
public:
	CMXPTinyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MXPTINY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON							m_hIcon;
	//
	CButton							m_startButton;
	CStatic							m_configBoxStatic;
	CComboBox						m_videoEncodingCombo;
	CComboBox						m_videoInputDeviceCombo;
	BOOL						    m_autorec;
	CSliderCtrl m_bitrate_slider;
	CStatic m_bitrate_static;
	CString m_default_exe;

	class dev {
		public:
			IDeckLink *device;
			CString name;
			IBMDStreamingDeviceInput *input;
			BMDStreamingDeviceMode mode;
	};

	std::vector <dev> m_devs;
	HANDLE m_fh;
	HANDLE m_pipe;
	CString m_filename;
	CString m_vlcexe;
	LARGE_INTEGER m_tscount;
	LARGE_INTEGER m_last_tscount;
	DWORD m_bitrate;

	// Streaming API:
	IBMDStreamingDiscovery*			m_streamingDiscovery;
	IDeckLink*						m_streamingDevice;
	IBMDStreamingDeviceInput*		m_streamingDeviceInput;
	bool							m_playing;
	bool							m_recording;
	BMDStreamingDeviceMode			m_deviceMode;
	BMDVideoConnection				m_inputConnector;
	BMDDisplayMode					m_inputMode;
	CString							m_deviceName;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnCbnSelchangeComboInputDevice();
	afx_msg void OnBnClickedButtonFolder();

private:
	void							StartPreview();
	void							StopPreview();
	void							UpdateUIForNewDevice();
	void							UpdateUIForNoDevice();
	void							UpdateUIForModeChanges();
	void							UpdateEncodingPresetsUIForInputMode();
	void							EncodingPresetsRemoveItems();
	void updBitrate();
	void activate_device(int i);
	int browseFolder(char *pszBuffer);

public:
	// IUknown
	// We need to correctly implement QueryInterface, but not the AddRef/Release
	virtual HRESULT STDMETHODCALLTYPE	QueryInterface (REFIID iid, LPVOID* ppv);
	virtual ULONG STDMETHODCALLTYPE		AddRef ()	{return 1;}
	virtual ULONG STDMETHODCALLTYPE		Release ()	{return 1;}

public:
	// IBMDStreamingDeviceNotificationCallback
	virtual HRESULT STDMETHODCALLTYPE StreamingDeviceArrived(IDeckLink* device);
	virtual HRESULT STDMETHODCALLTYPE StreamingDeviceRemoved(IDeckLink* device);
	virtual HRESULT STDMETHODCALLTYPE StreamingDeviceModeChanged(IDeckLink* device, BMDStreamingDeviceMode mode);
	virtual HRESULT STDMETHODCALLTYPE StreamingDeviceFirmwareUpdateProgress(IDeckLink* device, unsigned char percent);

public:
	virtual HRESULT STDMETHODCALLTYPE H264NALPacketArrived(IBMDStreamingH264NALPacket* nalPacket);
	virtual HRESULT STDMETHODCALLTYPE H264AudioPacketArrived(IBMDStreamingAudioPacket* audioPacket);
	virtual HRESULT STDMETHODCALLTYPE MPEG2TSPacketArrived(IBMDStreamingMPEG2TSPacket* mpeg2TSPacket);
	virtual HRESULT STDMETHODCALLTYPE H264VideoInputConnectorScanningChanged(void);
	virtual HRESULT STDMETHODCALLTYPE H264VideoInputConnectorChanged(void);
	virtual HRESULT STDMETHODCALLTYPE H264VideoInputModeChanged(void);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CButton m_record_button;
	CStatic m_encoding_static;
	CButton m_folder_button;
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboEncodingPreset();
	afx_msg void OnBnClickedButtonPrevcfg();
	CButton m_prevcfg_button;
	afx_msg void OnBnClickedButtonCustomize();
	CButton m_button_customize;
	CButton m_button_autorec;
	afx_msg void OnBnClickedAutorec();
};
