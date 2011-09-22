
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef USE_DETOURS
#include <detours.h>
#else
#include "windllinjection.h"
#endif
#endif


int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us
  if ( args.isEmpty() ) {
    qWarning( "Nothing to probe. Usage: endoscope <application> <args>" );
    return 1;
  }

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifndef Q_OS_WIN
  if ( env.value( "LD_PRELOAD").isEmpty() ) {
    QStringList pldirs;
    pldirs << "/usr/local/lib64" << "/usr/local/lib"
           << "/opt/lib64" << "/opt/lib"
           << "/usr/lib64" << "/usr/lib";
    QDir::setSearchPaths( "preloads", pldirs );
    QFile plfile( "preloads:libendoscope_probe.so");
    if ( plfile.exists() ) {
      env.insert( "LD_PRELOAD", plfile.fileName() );
    } else {
      qWarning( "Cannot locate libendoscope_probe.so in the typical places.\n"
                "Try setting the $LD_PRELOAD environment variable to the fullpath,\n"
                "For example:\n"
                "  export LD_PRELOAD=/opt/lib64/libendoscope_probe.so\n"
                "Continuing nevertheless, some systems can also preload from just the library name...");
      env.insert("LD_PRELOAD", "libendoscope_probe.so" );
    }
  }
  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );
  return proc.exitCode();
#else
#ifdef USE_DETOURS
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

  QString dllPath = QCoreApplication::applicationDirPath() + "/endoscope_probe.dll";
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
#else
  DWORD dwCreationFlags = CREATE_NO_WINDOW;
  dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
  dwCreationFlags |= CREATE_SUSPENDED;
  STARTUPINFOW startupInfo = { sizeof( STARTUPINFO ), 0, 0, 0,
                               (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
                               (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
                               0, 0, 0, STARTF_USESTDHANDLES, 0, 0, 0, 
                               GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE)
  };
  PROCESS_INFORMATION pid;
  memset(&pid, 0, sizeof(PROCESS_INFORMATION));

  BOOL success = CreateProcess(0, (wchar_t*)args.join(" ").utf16(),
                               0, 0, TRUE, dwCreationFlags,
                               0, 0,
                               &startupInfo, &pid);
  Endoscope::WinDllInjection in;
  in.setDestinationProcess(pid.hProcess, pid.hThread);
  QString dllPath = QCoreApplication::applicationDirPath() + "/endoscope_probe.dll";
  dllPath.replace("/", "\\");
  in.setInjectionDll(dllPath);
  in.inject();
  ResumeThread(pid.hThread);
  WaitForSingleObject(pid.hProcess, INFINITE);
  DWORD exitCode;
  GetExitCodeProcess(pid.hProcess, &exitCode);
  return exitCode;
#endif
#endif
}
