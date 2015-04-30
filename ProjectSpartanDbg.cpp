#include "stdafx.h"
typedef HRESULT(NTAPI *tNtSuspendProcess)(IN HANDLE);
tNtSuspendProcess NtSuspendProcess;

const LPCTSTR sSpartanAUMID = _T("Microsoft.Windows.Spartan_cw5n1h2txyewy!Microsoft.Spartan.Spartan");

HRESULT fGetSnapshot(DWORD dwFlags, DWORD dwProcessId, HANDLE &hSnapshot) {
  HRESULT hResult;
  hSnapshot = CreateToolhelp32Snapshot(dwFlags, dwProcessId);
  if (hSnapshot == INVALID_HANDLE_VALUE) {
    _tprintf(_T("Cannot create snapshot\r\n"));
    hResult = HRESULT_FROM_WIN32(GetLastError());
  } else {
    hResult = S_OK;
  }
  return hResult;
}
BOOL fCloseHandleAndUpdateResult(HANDLE hHandle, HRESULT &hResult) {
  if (!CloseHandle(hHandle)) {
    if (SUCCEEDED(hResult)) {
      hResult = HRESULT_FROM_WIN32(GetLastError());
    }
    return FALSE;
  }
  return TRUE;
}
HRESULT fSuspendThreadsInProcessById(DWORD dwProcessId) {
  HANDLE hThreadSnapshot;
  HRESULT hResult = fGetSnapshot(TH32CS_SNAPTHREAD, 0, hThreadSnapshot);
  if (SUCCEEDED(hResult)) {
    THREADENTRY32 oThreadEntry32;
    oThreadEntry32.dwSize = sizeof(oThreadEntry32);
    if (!Thread32First(hThreadSnapshot, &oThreadEntry32)) {
      _tprintf(_T("Cannot get first thread from snapshot\r\n"));
      hResult = HRESULT_FROM_WIN32(GetLastError());
    } else do {
      if (oThreadEntry32.th32OwnerProcessID == dwProcessId) {
        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, oThreadEntry32.th32ThreadID);
        if (!hThread) {
          _tprintf(_T("Cannot open thread %d of process %d"), oThreadEntry32.th32ThreadID, oThreadEntry32.th32OwnerProcessID);
          hResult = HRESULT_FROM_WIN32(GetLastError());
        } else {
          if (SuspendThread(hThread) == -1) {
            _tprintf(_T("Cannot suspend thread %d of process %d"), oThreadEntry32.th32ThreadID, oThreadEntry32.th32OwnerProcessID);
            hResult = HRESULT_FROM_WIN32(GetLastError());
          }
          if (!fCloseHandleAndUpdateResult(hThread, hResult)) {
            _tprintf(_T("Cannot close thread %d of process %d\r\n"), oThreadEntry32.th32ThreadID, oThreadEntry32.th32OwnerProcessID);
          }
        }
      }
    } while (SUCCEEDED(hResult) && Thread32Next(hThreadSnapshot, &oThreadEntry32));
    if (!fCloseHandleAndUpdateResult(hThreadSnapshot, hResult)) {
      _tprintf(_T("Cannot close snapshot\r\n"));
    }
  }
  return hResult;
}
HRESULT fStartDebugger(DWORD dwTargetProcessId, UINT uCommandLineCount, _TCHAR* asCommandLine[]) {
  std::basic_string<TCHAR> sCommandLine = _T("");
  #ifdef UNICODE
    std::basic_string<TCHAR> sProcessId = std::to_wstring(dwTargetProcessId);
  #else
    std::basic_string<TCHAR> sProcessId = std::to_string(dwTargetProcessId);
  #endif
  for (UINT uIndex = 0; uIndex < uCommandLineCount; uIndex++) {
    if (uIndex > 0) sCommandLine += _T(" ");
    if (_tcscmp(asCommandLine[uIndex], _T("@pid@")) == 0) {
      sCommandLine += sProcessId;
    } else {
      sCommandLine += asCommandLine[uIndex];
    }
  }
  HRESULT hResult;
  STARTUPINFO oStartupInfo = {};
  oStartupInfo.cb = sizeof(oStartupInfo);
  oStartupInfo.dwFlags = STARTF_USESTDHANDLES;
  oStartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  oStartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  oStartupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  PROCESS_INFORMATION oProcessInformation = {};
  if (!CreateProcess(NULL, (LPWSTR)sCommandLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &oStartupInfo, &oProcessInformation)) {
    hResult = HRESULT_FROM_WIN32(GetLastError());
  } else {
    if (WaitForSingleObject( oProcessInformation.hProcess, INFINITE ) != WAIT_OBJECT_0) {
      hResult = HRESULT_FROM_WIN32(GetLastError());
    } else {
      hResult = S_OK;
    }
    if (!fCloseHandleAndUpdateResult(oProcessInformation.hProcess, hResult)) {
      _tprintf(_T("Cannot close debugger process"));
    }
    if (!fCloseHandleAndUpdateResult(oProcessInformation.hThread, hResult)) {
      _tprintf(_T("Cannot close debugger thread"));
    }
  }
  return hResult;
}
int _tmain(UINT uArgumentsCount, _TCHAR* asArguments[]) {
  HRESULT hResult;
  NtSuspendProcess = (tNtSuspendProcess)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtSuspendProcess");
  if (!NtSuspendProcess) {
    _tprintf(_T("Cannot open process\r\n"));
    hResult = E_NOTIMPL;
  } else if (uArgumentsCount < 2) {
    _tprintf(_T("Usage: ProjectSpartanDbg <url> <debugger command line>\r\n"));
    hResult = E_INVALIDARG;
  } else  {
    hResult = CoInitialize(NULL);
    if (!SUCCEEDED(hResult)) {
      _tprintf(L"Failed to initialize\r\n");
    } else {
      IApplicationActivationManager* pAAM;
      hResult = CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAAM));
      if (!SUCCEEDED(hResult)) {
        _tprintf(_T("Failed to create Application Activation Manager\r\n"));
      } else {
        DWORD dwProcessId;
        hResult = pAAM->ActivateApplication(sSpartanAUMID, asArguments[1], AO_NONE, &dwProcessId);
        if (!SUCCEEDED(hResult)) {
          _tprintf(_T("Failed to launch Project Spartan\r\n"));
        } else {
          _tprintf(_T("Project Spartan process id = %d\r\n"), dwProcessId);
          hResult = fSuspendThreadsInProcessById(dwProcessId);
          if (SUCCEEDED(hResult) && uArgumentsCount > 2) {
            hResult = fStartDebugger(dwProcessId, uArgumentsCount - 2, asArguments + 2);
          }
        }
      }
      pAAM->Release();
    }
    CoUninitialize();
  }
  return hResult;
}
