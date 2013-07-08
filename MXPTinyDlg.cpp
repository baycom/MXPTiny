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


#include "stdafx.h"
#include "MXPTiny.h"
#include "MXPTinyDlg.h"
#include "exeSetup.h"
#include "EncodingPresetSetup.h"

#include "DeckLinkAPI_h.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMXPTinyDlg dialog



int GetKeyData(HKEY hRootKey, CString subKey, CString value, LPBYTE data, DWORD cbData)
{
	HKEY hKey;
	if(RegOpenKeyEx(hRootKey, subKey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		return 0;

	if(RegQueryValueEx(hKey, value, NULL, NULL, data, &cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	RegCloseKey(hKey);
	return 1;
}

int SetKeyData(HKEY hRootKey, CString subKey, DWORD dwType, CString value, LPBYTE data, DWORD cbData)
{
	HKEY hKey;
	if(RegCreateKey(hRootKey, subKey, &hKey) != ERROR_SUCCESS)
		return 0;

	if(RegSetValueEx(hKey, value, 0, dwType, data, cbData) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	RegCloseKey(hKey);
	return 1;
}

CMXPTinyDlg::CMXPTinyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMXPTinyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_streamingDiscovery = NULL;
	m_streamingDevice = NULL;
	m_streamingDeviceInput = NULL;
	m_fh = NULL;
	m_pipe = NULL;
	m_playing = false;
	m_recording = false;
	m_deviceMode = bmdStreamingDeviceUnknown;
	m_autorec = false;

	TCHAR pf[MAX_PATH];

	if(!GetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), _T("bitrate"), (BYTE *)&m_bitrate, sizeof(m_bitrate))) 
		m_bitrate=20000;

	GetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), _T("autorec"), (BYTE *)&m_autorec, sizeof(m_autorec));
	m_button_autorec.SetCheck(m_autorec);

	if(!GetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), _T("folder"), (BYTE *)m_filename.GetBuffer(MAX_PATH), MAX_PATH))
	{
		m_filename.ReleaseBuffer();
		SHGetSpecialFolderPath( 0, pf, CSIDL_MYDOCUMENTS, TRUE ); 
		m_filename.Format(_T("%s\\DeckLink.ts"), pf);	
	} else {
		m_filename.ReleaseBuffer();
	}
	if(!GetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), _T("previewcmd"), (BYTE *)m_vlcexe.GetBuffer(MAX_PATH), MAX_PATH))
	{
		m_vlcexe.ReleaseBuffer();
		SHGetSpecialFolderPath( 0, pf, CSIDL_PROGRAM_FILESX86, FALSE ); 
		m_vlcexe.Format(_T("%s\\VideoLAN\\VLC\\vlc.exe --fullscreen stream://\\\\\\.\\pipe\\DeckLink.ts"), pf);
	} else 
		m_vlcexe.ReleaseBuffer();

	SHGetSpecialFolderPath( 0, pf, CSIDL_PROGRAM_FILESX86, FALSE ); 
	m_default_exe.Format(_T("%s\\VideoLAN\\VLC\\vlc.exe --fullscreen stream://\\\\\\.\\pipe\\DeckLink.ts"), pf);

	SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_DWORD, _T("bitrate"), (BYTE *)&m_bitrate, sizeof(m_bitrate));
	SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_SZ, _T("folder"), (BYTE *)m_filename.GetBuffer(MAX_PATH), m_filename.GetLength()*2);
	m_filename.ReleaseBuffer();
	SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_SZ, _T("previewcmd"), (BYTE *)m_vlcexe.GetBuffer(MAX_PATH), m_vlcexe.GetLength()*2);
	m_vlcexe.ReleaseBuffer();
}

void CMXPTinyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START_PREVIEW, m_startButton);
	DDX_Control(pDX, IDC_STATIC_CONFIGBOX, m_configBoxStatic);
	DDX_Control(pDX, IDC_COMBO_ENCODING_PRESET, m_videoEncodingCombo);
	DDX_Control(pDX, IDC_COMBO_INPUT_DEVICE, m_videoInputDeviceCombo);
	DDX_Control(pDX, IDC_SLIDER1, m_bitrate_slider);
	DDX_Control(pDX, IDC_STATIC_BITRATE_CURRENT, m_bitrate_static);
	DDX_Control(pDX, IDC_BUTTON_RECORD, m_record_button);
	DDX_Control(pDX, IDC_STATIC_ENCODING_PRESET, m_encoding_static);
	DDX_Control(pDX, IDC_BUTTON_FOLDER, m_folder_button);
	DDX_Control(pDX, IDC_BUTTON_PREVCFG, m_prevcfg_button);
	DDX_Control(pDX, IDC_BUTTON_CUSTOMIZE, m_button_customize);
	DDX_Check(pDX, IDC_AUTOREC, m_autorec);
	DDX_Control(pDX, IDC_AUTOREC, m_button_autorec);
}

BEGIN_MESSAGE_MAP(CMXPTinyDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START_PREVIEW, &CMXPTinyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CMXPTinyDlg::OnBnClickedButtonRecord)
	ON_CBN_SELCHANGE(IDC_COMBO_INPUT_DEVICE, &CMXPTinyDlg::OnCbnSelchangeComboInputDevice)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER, &CMXPTinyDlg::OnBnClickedButtonFolder)
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_ENCODING_PRESET, &CMXPTinyDlg::OnCbnSelchangeComboEncodingPreset)
	ON_BN_CLICKED(IDC_BUTTON_PREVCFG, &CMXPTinyDlg::OnBnClickedButtonPrevcfg)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOMIZE, &CMXPTinyDlg::OnBnClickedButtonCustomize)
	ON_BN_CLICKED(IDC_AUTOREC, &CMXPTinyDlg::OnBnClickedAutorec)
END_MESSAGE_MAP()


// CMXPTinyDlg message handlers

BOOL CMXPTinyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_bitrate_slider.SetRange(1000,28000);
	m_bitrate_slider.SetPos(m_bitrate);
	updBitrate();
	
	// Presume no devices to begin with:
	UpdateUIForNoDevice();

	// Initialise Blackmagic Streaming API
	HRESULT						result;
	
	result = CoCreateInstance(CLSID_CBMDStreamingDiscovery, NULL, CLSCTX_ALL, IID_IBMDStreamingDiscovery, (void**)&m_streamingDiscovery);
	if (FAILED(result))
	{
		MessageBox(_T("This application requires the Blackmagic Streaming drivers installed.\nPlease install the Blackmagic Streaming drivers to use the features of this application."), _T("Error"));
		goto bail;
	}

	// Note: at this point you may get device notification messages!
	result = m_streamingDiscovery->InstallDeviceNotifications(this);
	if (FAILED(result))
	{
		MessageBox(_T("Failed to install device notifications for the Blackmagic Streaming devices"), _T("Error"));
		goto bail;
	}

	// return TRUE unless you set the focus to a control
	return TRUE;

bail:
	if (m_streamingDiscovery != NULL)
	{
		m_streamingDiscovery->Release();
		m_streamingDiscovery = NULL;
	}

	return FALSE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMXPTinyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMXPTinyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// IUnknown

void CMXPTinyDlg::OnBnClickedOk()
{
	OutputDebugString(_T("OnBnClickedOk\n"));

	if (m_playing ||  m_deviceMode == bmdStreamingDeviceEncoding)
		StopPreview();
	else
		StartPreview();
}

void CMXPTinyDlg::StartPreview()
{
	if (m_playing)
		return;
	int i=m_videoEncodingCombo.GetCurSel();
	int iscustom=0;
	IBMDStreamingVideoEncodingMode* encodingMode = (IBMDStreamingVideoEncodingMode*)m_videoEncodingCombo.GetItemDataPtr(i);
	int64_t rate=m_bitrate_slider.GetPos();
	IBMDStreamingMutableVideoEncodingMode *em;

	CString str;
	m_videoEncodingCombo.GetLBText(i, str); 
	if(!str.Compare(_T("Custom"))){
		iscustom=1;
		em=(IBMDStreamingMutableVideoEncodingMode*)m_videoEncodingCombo.GetItemDataPtr(i);
	} else {
		encodingMode->CreateMutableVideoEncodingMode(&em);
	}
	em->SetInt(bmdStreamingEncodingPropertyVideoBitRateKbps, rate);

	m_streamingDeviceInput->SetVideoEncodingMode(em);
	if(!iscustom) {
		em->Release();
	}
	
	m_streamingDeviceInput->GetVideoEncodingMode(&encodingMode);
	encodingMode->GetInt(bmdStreamingEncodingPropertyVideoBitRateKbps, &rate);
	encodingMode->Release();

	m_bitrate_slider.SetPos((int)rate);
	updBitrate();
	m_bitrate=(DWORD)rate;
	SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_DWORD, _T("bitrate"), (BYTE *)&m_bitrate, sizeof(m_bitrate));
	m_filename.ReleaseBuffer();

	m_pipe=CreateNamedPipe(_T("\\\\.\\pipe\\DeckLink.ts"), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT | PIPE_ACCEPT_REMOTE_CLIENTS, 100, 188*1000, 188*1000, 0, NULL);

	m_playing = true;	
	m_last_tscount.QuadPart = 0;
	m_tscount.QuadPart = 0;
	m_streamingDeviceInput->StartCapture();
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	if(!m_autorec) {
		CreateProcess( NULL, m_vlcexe.GetBuffer(MAX_PATH), NULL, NULL, false, 0, NULL, NULL,  &si, &pi);
		m_vlcexe.ReleaseBuffer();
	} else {
		if(m_recording) 
			OnBnClickedButtonRecord();
		OnBnClickedButtonRecord();
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread); 

}
CString frameRate2String(unsigned int rate) 
{
	CString fmt;
	switch(rate) {
		case bmdStreamingEncodedFrameRate50i: fmt="50i"; break;
		case bmdStreamingEncodedFrameRate5994i: fmt="59.94i"; break;
		case bmdStreamingEncodedFrameRate60i: fmt="60i"; break;
		case bmdStreamingEncodedFrameRate2398p: fmt="23.98p"; break;
		case bmdStreamingEncodedFrameRate24p: fmt="24p"; break;
		case bmdStreamingEncodedFrameRate25p: fmt="25p"; break;
		case bmdStreamingEncodedFrameRate2997p: fmt="29.97p"; break;
		case bmdStreamingEncodedFrameRate30p: fmt="30p"; break;
		case bmdStreamingEncodedFrameRate50p: fmt="50p"; break;
		case bmdStreamingEncodedFrameRate5994p: fmt="59.94p"; break;
		case bmdStreamingEncodedFrameRate60p: fmt="60p"; break;
		default: fmt="N/A";
	}
	return fmt;
}

void CMXPTinyDlg::OnCbnSelchangeComboEncodingPreset()
{
	CString str;
	IBMDStreamingVideoEncodingMode* em = (IBMDStreamingVideoEncodingMode*)m_videoEncodingCombo.GetItemDataPtr(m_videoEncodingCombo.GetCurSel());

	LONGLONG rate=0;
	em->GetInt(bmdStreamingEncodingPropertyVideoFrameRate, &rate);
	CString fmt=frameRate2String(rate);
	str.Format(_T("Input: Source: X:%d,Y:%d->%dx%d Dest: %dx%d Format: %s"), em->GetSourcePositionX(), em->GetSourcePositionY(), em->GetSourceWidth(), em->GetSourceHeight(), em->GetDestWidth(), em->GetDestHeight(), fmt);

	m_encoding_static.SetWindowText(str);
}

void CMXPTinyDlg::StopPreview()
{
	m_encoding_static.SetWindowText(_T(""));
	m_playing = false;
	m_recording = false;

	if (m_streamingDeviceInput)
		m_streamingDeviceInput->StopCapture();
	if(m_fh) {
		m_record_button.SetWindowTextW(_T("Record"));
		CloseHandle(m_fh);
		m_fh=NULL;
	}
	if(m_pipe) {
		CloseHandle(m_pipe);
		m_pipe=NULL;
	}
}

void CMXPTinyDlg::UpdateUIForModeChanges()
{
	CString status = _T(" (unknown)");

	switch (m_deviceMode)
	{
		case bmdStreamingDeviceIdle:
			status = _T(" (idle)");
			break;
		case bmdStreamingDeviceEncoding:
			status = _T(" (encoding)");
			break;
		case bmdStreamingDeviceStopping:
			status = _T(" (stopping)");
			break;
	}
	CString displayName = _T("Device: ") + m_deviceName + status;
	m_configBoxStatic.SetWindowText(displayName);

	bool enablePresets = !m_playing;/*m_deviceMode == bmdStreamingDeviceIdle && m_inputMode != bmdModeUnknown;*/
	m_videoInputDeviceCombo.EnableWindow(enablePresets);
	m_videoEncodingCombo.EnableWindow(enablePresets);
	m_bitrate_slider.EnableWindow(enablePresets);
	m_prevcfg_button.EnableWindow(enablePresets);
	m_folder_button.EnableWindow(!m_recording);
	m_record_button.EnableWindow(!enablePresets);
	m_button_customize.EnableWindow(enablePresets);
	
	bool enableStartStop = (m_deviceMode == bmdStreamingDeviceIdle || m_deviceMode == bmdStreamingDeviceEncoding);
	m_startButton.EnableWindow(enableStartStop);

	bool start = /*!m_playing;*/ m_deviceMode != bmdStreamingDeviceEncoding;
	m_startButton.SetWindowText(start ? _T("Preview") : _T("Stop"));
	if (m_deviceMode == bmdStreamingDeviceEncoding)
	{
//		if (m_inputMode != bmdModeUnknown)
//			StartPreview();
	}
	else
		StopPreview();
}

void CMXPTinyDlg::UpdateUIForNewDevice()
{
	// Add video input modes:
	IDeckLinkDisplayModeIterator* inputModeIterator;
	if (FAILED(m_streamingDeviceInput->GetVideoInputModeIterator(&inputModeIterator)))
	{
		MessageBox(_T("Failed to get input mode iterator"), _T("error"));
		return;
	}

	BMDDisplayMode currentInputModeValue;
	if (FAILED(m_streamingDeviceInput->GetCurrentDetectedVideoInputMode(&currentInputModeValue)))
	{
		MessageBox(_T("Failed to get current detected input mode"), _T("error"));
		return;
	}

	IDeckLinkDisplayMode* inputMode;
	while (inputModeIterator->Next(&inputMode) == S_OK)
	{
		if (inputMode->GetDisplayMode() == currentInputModeValue) {
			BSTR modeName;
			if (inputMode->GetName(&modeName) != S_OK)
			{
				inputMode->Release();
				inputModeIterator->Release();
				return;
			}

			CString modeNameCString(modeName);
			SysFreeString(modeName);

			CString str;
			str.Format(_T("Input Mode: % 26s"), modeNameCString);
			m_encoding_static.SetWindowText(str);
			break;
		}
		inputMode->Release();
	}

	inputModeIterator->Release();

	UpdateEncodingPresetsUIForInputMode();
}

void CMXPTinyDlg::UpdateUIForNoDevice()
{
	m_deviceMode = bmdStreamingDeviceUnknown;
	m_inputMode = bmdModeUnknown;

	m_configBoxStatic.SetWindowText(_T("No device detected"));

	EncodingPresetsRemoveItems();
	int index = m_videoEncodingCombo.AddString(_T("No Input"));
	m_videoEncodingCombo.SetItemDataPtr(index, NULL);
	m_videoEncodingCombo.EnableWindow(FALSE);

	m_startButton.EnableWindow(FALSE);
}

void CMXPTinyDlg::UpdateEncodingPresetsUIForInputMode()
{
	if (m_streamingDevice == NULL)
		return;

	BMDDisplayMode inputMode = BMDDisplayMode(m_inputMode);
	EncodingPresetsRemoveItems();

	IBMDStreamingVideoEncodingModePresetIterator* presetIterator;
	
	if (SUCCEEDED(m_streamingDeviceInput->GetVideoEncodingModePresetIterator(inputMode, &presetIterator)))
	{
		IBMDStreamingVideoEncodingMode* encodingMode = NULL;
		BSTR encodingModeName;
		
		while (presetIterator->Next(&encodingMode) == S_OK)
		{
			encodingMode->GetName(&encodingModeName);
			CString encodingModeNameCString(encodingModeName);
			SysFreeString(encodingModeName);
			
			// Add this item to the video input poup menu
			int newIndex = m_videoEncodingCombo.AddString(encodingModeNameCString);
			m_videoEncodingCombo.SetItemDataPtr(newIndex, encodingMode);

			// We don't release the object here, as we hold the reference
			// in the combo box.
		}

		presetIterator->Release();
	}

	m_videoEncodingCombo.SetCurSel(0);
}

void CMXPTinyDlg::EncodingPresetsRemoveItems()
{
	int currentCount = m_videoEncodingCombo.GetCount();

	for (int i = 0; i < currentCount; i++)
	{
		IBMDStreamingVideoEncodingMode* encodingMode = (IBMDStreamingVideoEncodingMode*)m_videoEncodingCombo.GetItemDataPtr(i);

		if (encodingMode != NULL)
			encodingMode->Release();
	}

	m_videoEncodingCombo.ResetContent();
}

HRESULT CMXPTinyDlg::QueryInterface(REFIID iid, LPVOID* ppv)
{
	HRESULT result = E_NOINTERFACE;

	if (ppv == NULL)
		return E_POINTER;
	*ppv = NULL;
	
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(static_cast<IBMDStreamingDeviceNotificationCallback*>(this));
		AddRef();
		result = S_OK;
	}
	else if (iid == IID_IBMDStreamingDeviceNotificationCallback)
	{
		*ppv = static_cast<IBMDStreamingDeviceNotificationCallback*>(this);
		AddRef();
		result = S_OK;
	}
	else if (iid == IID_IBMDStreamingH264InputCallback)
	{
		*ppv = static_cast<IBMDStreamingH264InputCallback*>(this);
		AddRef();
		result = S_OK;
	}
	
	return result;	
}

void CMXPTinyDlg::activate_device(int i) 
{
	if(m_streamingDevice != NULL) {
		m_streamingDeviceInput->SetCallback(NULL);
	}
	dev d=m_devs.at(i);
	m_streamingDevice = d.device;
	m_streamingDeviceInput = d.input;
	m_deviceMode = d.mode;
	m_deviceName = d.name;

	if (m_streamingDeviceInput->GetCurrentDetectedVideoInputMode(&m_inputMode) != S_OK)
		MessageBox(_T("Failed to get current detected input mode"), _T("error"));
	m_videoInputDeviceCombo.SetCurSel(i);

	// Now install our callbacks. To do this, we must query our own delegate
	// to get it's IUnknown interface, and pass this to the device input interface.
	// It will then query our interface back to a IBMDStreamingH264InputCallback,
	// if that's what it wants.
	// Note, although you may be tempted to cast directly to an IUnknown, it's
	// not particular safe, and is invalid COM.
	IUnknown* ourCallbackDelegate;
	this->QueryInterface(IID_IUnknown, (void**)&ourCallbackDelegate);
	//
	HRESULT result = d.input->SetCallback(ourCallbackDelegate);
	//
	// Finally release ourCallbackDelegate, since we created a reference to it
	// during QueryInterface. The device will hold its own reference.
	ourCallbackDelegate->Release();
	UpdateUIForNewDevice();
	if (m_deviceMode != bmdStreamingDeviceUnknown) { 
		UpdateUIForModeChanges();
	}
}

HRESULT CMXPTinyDlg::StreamingDeviceArrived(IDeckLink* device)
{
	dev d;
	HRESULT			result;
	// These messages will happen on the main loop as a result
	// of the message pump.

	// See if it can do input:
	result = device->QueryInterface(IID_IBMDStreamingDeviceInput, (void**)&d.input);
	if (FAILED(result))
	{
		// This device doesn't support input. We can ignore this device.
		return S_OK;
	}

	// Ok, we're happy with this device, hold a reference to the device (we
	// also have a reference held from the QueryInterface, too).
	d.device = device;
	device->AddRef();

	if (FAILED(result))
	{
		d.device->Release();
		d.input->Release();
		return S_OK;
	}

	BSTR modelName;
	if (device->GetModelName(&modelName) != S_OK)
		return S_OK;

	CString modelNameCString(modelName);
	SysFreeString(modelName);

	d.name = modelNameCString;
	d.mode = bmdStreamingDeviceUnknown;

	m_devs.push_back(d);

	int newIndex = m_videoInputDeviceCombo.AddString(d.name);
	m_videoInputDeviceCombo.SetItemDataPtr(newIndex, &d);

	// Check we don't already have a device.
	if (m_streamingDevice != NULL)
	{
		return S_OK;
	} 

	activate_device(newIndex);

	return S_OK;
}

HRESULT CMXPTinyDlg::StreamingDeviceRemoved(IDeckLink* device)
{
	int shutdownactive=0;
	// We only care about removal of the device we are using
	if (device == m_streamingDevice) {
		m_streamingDeviceInput = NULL;
		m_streamingDevice = NULL;
		StopPreview();
		shutdownactive=1;
	}
	for (std::vector <dev>::iterator d = m_devs.begin(); d != m_devs.end(); ++d )
	{
		if(d->device == device) {
			d->input->SetCallback(NULL);
			d->input->Release();
			d->device->Release();
			m_devs.erase(d);
			break;
		} 
	}
	int cursel=m_videoInputDeviceCombo.GetCurSel();
	m_videoInputDeviceCombo.ResetContent();	
	for (std::vector <dev>::iterator d = m_devs.begin(); d != m_devs.end(); ++d )
	{
		int newIndex = m_videoInputDeviceCombo.AddString(d->name);
		m_videoInputDeviceCombo.SetItemDataPtr(newIndex, &d);
	}
	m_videoInputDeviceCombo.SetCurSel(cursel);
	if(shutdownactive) {
		if(m_devs.size() == 0 ) {
			UpdateUIForNoDevice();
		} else {
			activate_device(0);
		}
	}

	return S_OK;
}

HRESULT CMXPTinyDlg::StreamingDeviceModeChanged(IDeckLink* device, BMDStreamingDeviceMode mode)
{
	for (std::vector <dev>::iterator d = m_devs.begin(); d != m_devs.end(); ++d )
	{
		if(d->device == device) {
			d->mode=mode;
			break;
		} 
	}
	if(device != m_streamingDevice)
		return S_OK;

	if (mode == m_deviceMode)
		return S_OK;

	m_deviceMode = mode;

	UpdateUIForModeChanges();

	return S_OK;
}

HRESULT CMXPTinyDlg::StreamingDeviceFirmwareUpdateProgress(IDeckLink* device, unsigned char percent)
{
	return S_OK;
}

HRESULT CMXPTinyDlg::H264NALPacketArrived(IBMDStreamingH264NALPacket* nalPacket)
{
	return S_OK;
}

HRESULT CMXPTinyDlg::H264AudioPacketArrived(IBMDStreamingAudioPacket* audioPacket)
{
	return S_OK;
}

HRESULT CMXPTinyDlg::MPEG2TSPacketArrived(IBMDStreamingMPEG2TSPacket* mpeg2TSPacket)
{
	int len=mpeg2TSPacket->GetPayloadSize();
	int rec_error=0;
	void *buf;

	mpeg2TSPacket->GetBytes(&buf);
	DWORD dwBytesWritten;
	m_tscount.QuadPart+=len;
	if(m_playing) {
		if(!WriteFile(m_pipe, buf, len, &dwBytesWritten, NULL)) {
			if(GetLastError() == ERROR_NO_DATA ) {
				CloseHandle(m_pipe);
				m_pipe=CreateNamedPipe(_T("\\\\.\\pipe\\DeckLink.ts"), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_NOWAIT | PIPE_ACCEPT_REMOTE_CLIENTS, 100, 188*1000, 188*1000, 0, NULL);
			}
		}
		if(m_fh != NULL && !WriteFile(m_fh, buf, len, &dwBytesWritten, NULL)) {
			rec_error=1;
		}
		if((m_tscount.QuadPart-m_last_tscount.QuadPart)>(1024*10)) {
			CString str;
			m_last_tscount.QuadPart=m_tscount.QuadPart;
			str.Format(_T("Receiving (kB): % 26llu"), m_tscount.QuadPart>>10);
			if(m_recording) {
				LARGE_INTEGER FileSize;
				GetFileSizeEx( m_fh, &FileSize);
				str.Format(_T("%s    -    Recording (kB): % 6llu %s"), str, FileSize.QuadPart>>10, rec_error?_T("- ERROR WRITING !!!"):_T(""));
			}
			m_encoding_static.SetWindowText(str);
		}
	}
	return S_OK;
}

HRESULT CMXPTinyDlg::H264VideoInputConnectorScanningChanged(void)
{
	return E_NOTIMPL;
}

HRESULT CMXPTinyDlg::H264VideoInputConnectorChanged(void)
{
	return E_NOTIMPL;
}

HRESULT CMXPTinyDlg::H264VideoInputModeChanged(void)
{
	if (m_streamingDeviceInput->GetCurrentDetectedVideoInputMode(&m_inputMode) != S_OK)
		MessageBox(_T("Failed to get current detected input mode"), _T("error"));
	else
	{
		UpdateEncodingPresetsUIForInputMode();
	}

	if (m_inputMode == bmdModeUnknown)
	{
	}

	UpdateUIForModeChanges();
	
	if(m_autorec && !m_recording) {
		StartPreview();
	}

	return S_OK;
}


void CMXPTinyDlg::OnBnClickedButtonRecord()
{
	if (m_streamingDevice == NULL)
	return;

	if(m_recording) {
		if(m_fh != NULL) {
			CloseHandle(m_fh);
			m_fh=NULL;
		}		
		m_record_button.SetWindowTextW(_T("Record"));
		m_recording=false;
	} else {
		if(!m_filename.IsEmpty()) {
			m_fh=CreateFile(m_filename,  FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if(m_fh != INVALID_HANDLE_VALUE) {
				m_record_button.SetWindowTextW(_T("Recording..."));
				m_recording=true;
			}
		}
	}
	m_folder_button.EnableWindow(!m_recording);
}


void CMXPTinyDlg::OnCbnSelchangeComboInputDevice()
{
	activate_device(m_videoInputDeviceCombo.GetCurSel());
}


void CMXPTinyDlg::OnBnClickedButtonFolder()
{
	#define MAX_CFileDialog_FILE_COUNT 99
	#define FILE_LIST_BUFFER_SIZE ((MAX_CFileDialog_FILE_COUNT * (MAX_PATH + 1)) + 1)
	static TCHAR BASED_CODE szFilter[] = _T("Transport stream Files (*.ts)|*.ts|");

	wchar_t* p = m_filename.GetBuffer( FILE_LIST_BUFFER_SIZE );
	CFileDialog dlgFile(TRUE, _T("ts"), _T("DeckLink"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	OPENFILENAME& ofn = dlgFile.GetOFN( );
	ofn.lpstrFile = p;
	ofn.nMaxFile = FILE_LIST_BUFFER_SIZE;
	INT_PTR ret=dlgFile.DoModal();
	if( ret == 1) {		
		SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_SZ, _T("folder"), (BYTE *)m_filename.GetBuffer(MAX_PATH), m_filename.GetLength()*2);
		m_filename.ReleaseBuffer();
		CString str;
		str.Format(_T("Selected file: % 26s"), m_filename);
		m_encoding_static.SetWindowText(str);
	}

	m_filename.ReleaseBuffer();
}

void CMXPTinyDlg::updBitrate() 
{
		CString str;
		str.Format(_T("% 5d kBit/s"), m_bitrate_slider.GetPos());
		m_bitrate_static.SetWindowText(str);
}

void CMXPTinyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);  

    // Check which slider sent the notification  
    if (pSlider == &m_bitrate_slider)  {
		updBitrate();
	}
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMXPTinyDlg::OnClose()
{
	if(m_playing)
		StopPreview();
	__super::OnClose();
}


void CMXPTinyDlg::OnBnClickedButtonPrevcfg()
{
	exeSetup es;
	es.m_vlcexe=m_vlcexe;
	es.m_default=m_default_exe;
	es.m_hinttext=_T("TS data can be read from \\\\.\\pipe\\DeckLink.ts");

	INT_PTR ret=es.DoModal();

	if(ret==1) {
		m_vlcexe=es.m_vlcexe;
		SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_SZ, _T("previewcmd"), (BYTE *)m_vlcexe.GetBuffer(MAX_PATH), m_vlcexe.GetLength()*2);
		m_vlcexe.ReleaseBuffer();
		CString str;
		str.Format(_T("Command line: % 26s"), m_vlcexe);
		m_encoding_static.SetWindowText(str);
	}
}


void CMXPTinyDlg::OnBnClickedButtonCustomize()
{
	if (m_streamingDevice == NULL)
		return;
	CEncodingPresetSetup eps;
	eps.m_encoding_mode_in= (IBMDStreamingVideoEncodingMode*)m_videoEncodingCombo.GetItemDataPtr(m_videoEncodingCombo.GetCurSel());
	eps.m_streamingDeviceInput = m_streamingDeviceInput;
	INT_PTR ret=eps.DoModal();
	CString str;
	if(ret==1) {		
		int i;
		for(i=0;i<m_videoEncodingCombo.GetCount(); i++) {
			m_videoEncodingCombo.GetLBText(i, str); 
			if(!str.Compare(_T("Custom"))){
				IBMDStreamingVideoEncodingMode* em=(IBMDStreamingVideoEncodingMode* )m_videoEncodingCombo.GetItemData(i);
				em->Release();
				break;
			}
		}
		if(i == m_videoEncodingCombo.GetCount()) {
			i=m_videoEncodingCombo.AddString(_T("Custom"));
		}
		m_videoEncodingCombo.SetItemDataPtr(i, eps.m_mem);
		m_videoEncodingCombo.SetCurSel(i);
		OnCbnSelchangeComboEncodingPreset();
	}
}


void CMXPTinyDlg::OnBnClickedAutorec()
{
	UpdateData(TRUE);
	SetKeyData(HKEY_CURRENT_USER, _T("Software\\BayCom\\MXPTiny\\Settings"), REG_DWORD, _T("autorec"), (BYTE *)&m_autorec, sizeof(m_autorec));
}
