/**************************************************************************
**
** This code is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "processlist.h"

#include <QLibrary>

// Enable Win API of XP SP1 and later
#ifdef Q_OS_WIN
#    define _WIN32_WINNT 0x0502
#    include <windows.h>
#    if !defined(PROCESS_SUSPEND_RESUME) // Check flag for MinGW
#        define PROCESS_SUSPEND_RESUME (0x0800)
#    endif // PROCESS_SUSPEND_RESUME
#endif // Q_OS_WIN

#include <tlhelp32.h>
#include <psapi.h>
// Resolve QueryFullProcessImageNameW out of kernel32.dll due
// to incomplete MinGW import libs and it not being present
// on Windows XP.
static inline BOOL queryFullProcessImageName(HANDLE h,
                                                   DWORD flags,
                                                   LPWSTR buffer,
                                                   DWORD *size)
{
    // Resolve required symbols from the kernel32.dll
    typedef BOOL (WINAPI *QueryFullProcessImageNameWProtoType)
                 (HANDLE, DWORD, LPWSTR, PDWORD);
    static QueryFullProcessImageNameWProtoType queryFullProcessImageNameW = 0;
    if (!queryFullProcessImageNameW) {
        QLibrary kernel32Lib(QLatin1String("kernel32.dll"), 0);
        if (kernel32Lib.isLoaded() || kernel32Lib.load())
            queryFullProcessImageNameW = (QueryFullProcessImageNameWProtoType)kernel32Lib.resolve("QueryFullProcessImageNameW");
    }
    if (!queryFullProcessImageNameW)
        return FALSE;
    // Read out process
    return (*queryFullProcessImageNameW)(h, flags, buffer, size);
}

struct ProcessInfo {
    QString imageName;
    QString processOwner;
};

static inline ProcessInfo processInfo(DWORD processId)
{
    ProcessInfo pi;
    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION , TOKEN_READ, processId);
    if (handle == INVALID_HANDLE_VALUE)
        return pi;
    WCHAR buffer[MAX_PATH];
    DWORD bufSize = MAX_PATH;
    if (queryFullProcessImageName(handle, 0, buffer, &bufSize))
        pi.imageName = QString::fromUtf16(reinterpret_cast<const ushort*>(buffer));

    HANDLE processTokenHandle = NULL;
    if ( !OpenProcessToken( handle, TOKEN_READ, &processTokenHandle ) || !processTokenHandle )
        return pi;

    DWORD size = 0;
    GetTokenInformation(processTokenHandle, TokenUser, NULL, 0, &size);

    if( GetLastError() == ERROR_INSUFFICIENT_BUFFER ) {
        QByteArray buf;
        buf.resize(size);
        PTOKEN_USER userToken = reinterpret_cast<PTOKEN_USER>( buf.data() );
        if (userToken && GetTokenInformation( processTokenHandle, TokenUser, userToken, size, &size )) {
            SID_NAME_USE   sidNameUse;
            TCHAR          user[MAX_PATH] = { 0 };
            DWORD          userNameLength = MAX_PATH;
            TCHAR          domain[MAX_PATH] = { 0 };
            DWORD          domainNameLength = MAX_PATH;

            if (LookupAccountSid( NULL,
                                   userToken->User.Sid,
                                   user,
                                   &userNameLength,
                                   domain,
                                   &domainNameLength,
                                   &sidNameUse )) {
                pi.processOwner = QString::fromUtf16(reinterpret_cast<const ushort*>(user));
            }
        }
    }

    CloseHandle(processTokenHandle);
    CloseHandle(handle);
    return pi;
}

static inline bool isQtApp(DWORD processId)
{
    MODULEENTRY32 me;
    me.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    for (bool hasNext = Module32First(snapshot, &me); hasNext; hasNext = Module32Next(snapshot, &me)) {
        const QString module = QString::fromUtf16(reinterpret_cast<ushort*>(me.szModule));
//TODO: Do this check properly, ptobe does not need to have the same type
#ifdef NDEBUG
        if (module == QLatin1String("QtCore4.dll")) {
#else
        if (module == QLatin1String("QtCored4.dll")) {
#endif
            CloseHandle(snapshot);
            return true;
        }
    }
    CloseHandle(snapshot);
    return false;
}

ProcDataList processList(const ProcessDataList&)
{
    ProcDataList rc;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return rc;

    for (bool hasNext = Process32First(snapshot, &pe); hasNext; hasNext = Process32Next(snapshot, &pe)) {
        ProcData procData;
        procData.ppid = QString::number(pe.th32ProcessID);
        procData.name = QString::fromUtf16(reinterpret_cast<ushort*>(pe.szExeFile));
        const ProcessInfo processInf = processInfo(pe.th32ProcessID);
        procData.image = processInf.imageName;
        procData.user = processInf.processOwner;
        if (isQtApp(pe.th32ProcessID))
            procData.type = ProcData::QtApp;
        else
            procData.type = ProcData::NoQtApp;
        rc.push_back(procData);
    }
    CloseHandle(snapshot);
    return rc;
}
