#include "detourinjector.h"

#ifdef Q_OS_WIN

#include <windows.h>
#include <detours.h>

using namespace Endoscope;

int DetourInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

  QString dllPath( probeDll );
  dllPath.replace("/", "\\");

  QString detourPath = QCoreApplication::applicationDirPath() + "/detoured.dll";
  detourPath.replace("/", "\\");

  if (!DetourCreateProcessWithDll(0, (wchar_t*)args.join(" ").utf16(),
                                  NULL, NULL, TRUE, dwFlags, NULL, NULL,
                                  &si, &pi, detourPath.toLatin1().data(), dllPath.toLatin1().data(), NULL)) {
      printf("DetourCreateProcessWithDll failed: %d\n", GetLastError());
      ExitProcess(9007);
  }

  ResumeThread(pi.hThread);

  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  return exitCode;
}

#endif
