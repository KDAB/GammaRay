/*
  processtracker_windows.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// A said crossplatform way (using ptrace) is available at
// https://forum.juce.com/t/detecting-if-a-process-is-being-run-under-a-debugger/2098

#include "processtracker_windows.h"
#include "common/commonutils.h"

#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>

#include <qt_windows.h>
// Processthreadsapi.h on Windows 8 and Windows Server 2012
//#include <tlhelp32.h>

#include <type_traits>
#include <memory>

namespace {
// C++ wrappers / helpers

struct LocalHandleDeleter
{
    void operator()(HANDLE handle)
    {
        if (handle) {
            CloseHandle(handle);
            handle = INVALID_HANDLE_VALUE;
        }
    }
};

// Non pointer HANDLE helper
typedef std::remove_pointer<HANDLE>::type LHANDLE;
// HANDLE holder helper
typedef std::unique_ptr<LHANDLE, LocalHandleDeleter> LocalHandlePtr;

class LocalBuffer
{
private:
    BYTE *m_data;
    DWORD m_size;

    LocalBuffer(const LocalBuffer &other)
        : m_data(nullptr)
        , m_size(0)
    {
        Q_UNUSED(other);
        Q_ASSERT(false);
    }

    // cppcheck-suppress operatorEqVarError; m_data and m_size
    LocalBuffer &operator=(const LocalBuffer &other) {
        Q_UNUSED(other);
        Q_ASSERT(false);
        return *this;
    }

    void allocate(DWORD size) {
        m_data = (BYTE*)LocalAlloc(LMEM_FIXED, size);
        if (m_data != nullptr) {
            m_size = size;
        }
    }

    void free() {
        if (m_data != nullptr) {
            LocalFree(m_data);
            m_data = nullptr;
            m_size = 0;
        }
    }

public:
    LocalBuffer(DWORD size)
        : m_data(nullptr)
        , m_size(0)
    {
        allocate(size);
    }

    ~LocalBuffer() {
        free();
    }

    bool isValid() const {
        return m_data != nullptr;
    }

    DWORD size() const {
        return m_size;
    }

    BYTE *data() {
        return m_data;
    }

    template <typename T>
    T *data() {
        return reinterpret_cast<T *>(m_data);
    }

    const BYTE *const data() const {
        return m_data;
    }

    template <typename T>
    const T *const data() const {
        return reinterpret_cast<const T * const>(m_data);
    }

    void resize(DWORD size) {
        free();
        allocate(size);
    }

    void clear() {
        free();
    }
};

// Private windows stuff...
// I used the WINE declarations as they was the only one working on my windows 10 Pro
// https://github.com/wine-mirror/wine/blob/master/include/winternl.h

typedef LONG KPRIORITY;
//typedef LONG NTSTATUS;

#define STATUS_SUCCESS              ((NTSTATUS) 0x00000000)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS) 0xC0000004)

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef NT_ERROR
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#endif

enum SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45,
    SystemPolicyInformation = 134,
};

enum KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    Spare2,
    Spare3,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
};

enum THREAD_STATE
{
    Running = 2,
    Waiting = 5,
};

struct CLIENT_ID
{
    HANDLE UniqueProcess; // Process ID
    HANDLE UniqueThread;  // Thread ID
};

struct SYSTEM_THREAD
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    DWORD WaitTime;
    LPVOID StartAddress;
    CLIENT_ID ClientId;
    DWORD Priority;
    DWORD BasePriority;
    DWORD ContextSwitches;
    DWORD ThreadState;
    DWORD WaitReason;
    DWORD Unknown1;
};

struct VM_COUNTERS // virtual memory of process
{
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
};

struct UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
};

struct SYSTEM_PROCESS
{
    ULONG NextEntryOffset;
    DWORD NumberOfThreads;
    DWORD Unknown1[6];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    DWORD BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    DWORD Unknown2;
    VM_COUNTERS VirtualMemoryCounters;
    IO_COUNTERS IoCounters;
    SYSTEM_THREAD Threads[1];
};

static QString toQString(const UNICODE_STRING &string) {
#if defined(UNICODE)
    return QString::fromWCharArray(string.Buffer, string.Length);
#else
    Q_ASSERT(false);
    return QString();
#endif
}

// Resolve the dll only exported symbol
// Though the api is available with recent Windows version, we prefer to dynamically resolve it
typedef NTSTATUS(*NtQuerySystemInformationFunc)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID                    SystemInformation,
    ULONG                    SystemInformationLength,
    PULONG                   ReturnLength
);

static inline NtQuerySystemInformationFunc qt_NtQuerySystemInformation() {
    static auto symbol = (NtQuerySystemInformationFunc)QLibrary::resolve("NtDll", "NtQuerySystemInformation");
    return symbol;
}
}

using namespace GammaRay;

ProcessTrackerBackendWindows::ProcessTrackerBackendWindows(QObject *parent)
    : GammaRay::ProcessTrackerBackend(parent)
{
}

void ProcessTrackerBackendWindows::checkProcess(qint64 pid)
{
    GammaRay::ProcessTrackerInfo pinfo(pid);

    if (qt_NtQuerySystemInformation()) {
        BOOL traced = false;
        if (pid == QCoreApplication::applicationPid()) {
            traced = IsDebuggerPresent();
        } else {
            LocalHandlePtr processHandle(OpenProcess(READ_CONTROL | PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)); //krazy:exclude=captruefalse
            if (!CheckRemoteDebuggerPresent(processHandle.get(), &traced)) {
                qWarning("%s: Can not check remote debugger presence.");
            }
        }

        pinfo.traced = traced;

        // There is no way getting a thread state using public api as i'm aware of...
        /*{
            // Get threads status
            HANDLE_UNIQUE_PTR threadsHandle(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid));

            if (threadsHandle.get() == INVALID_HANDLE_VALUE) {
                Q_ASSERT(false);
                return false;
            }

            QVector<qint64> threadsId;
            THREADENTRY32 te32;

            // Fill in the size of the structure before using it.
            te32.dwSize = sizeof(THREADENTRY32);

            // Retrieve information about the first thread, and exit if unsuccessful
            if (!Thread32First(threadsHandle.get(), &te32)) {
                qWarning("Thread32First");  // Show cause of failure
                return false;
            }

            // Now walk the thread list of the system, and display information about each thread
            // associated with the specified process
            do {
                if (te32.th32OwnerProcessID == pid) {
                    threadsId << te32.th32ThreadID;
                }
            } while (Thread32Next(threadsHandle.get(), &te32));
        }*/

        // The only way to get threads state is using private stuff...

        // Get SYSTEM_PROCESS array
        // This must run in a loop because in the mean time a new process may have started and we need
        // more buffer than bufferNeededSize !!
        LocalBuffer buffer(1000);
        while (true) {
            WIN_ERROR_ASSERT(buffer.isValid(), break);

            ULONG bufferNeededSize = 0;
            NTSTATUS status = qt_NtQuerySystemInformation()(SystemProcessInformation, buffer.data(), buffer.size(), &bufferNeededSize);

            if (status == STATUS_INFO_LENGTH_MISMATCH) { // The buffer was too small
                buffer.resize(bufferNeededSize);
                continue;
            }
            WIN_ERROR_ASSERT(NT_SUCCESS(status), buffer.clear(); break;);
            break;
        }

        if (buffer.isValid()) {
            // Get the SYSTEM_PROCESS by its pid
            SYSTEM_PROCESS *processes = buffer.data<SYSTEM_PROCESS>();
            SYSTEM_PROCESS *process = nullptr;
            while (true) {
                if ((uintptr_t)processes->UniqueProcessId == pid) {
                    //qWarning() << "Found process:" << pid << toQString(processes->ImageName);
                    process = processes;
                    break;
                } else {
                    //qWarning("No matching process: %lli", (uintptr_t)processes->UniqueProcessId);
                }

                if (!processes->NextEntryOffset) {
                    qWarning("no process found matching pid: %lli", pid);
                    break;
                }

                processes = (SYSTEM_PROCESS *)((BYTE *)processes + processes->NextEntryOffset);
            }

            if (process) {
                // Get all SYSTEM_THREAD state to generate the process state
                // Looks like when debugging a windows process that spawn lots of new threads
                // All are suspended except the last one
                bool suspended = true;
                for (DWORD i = 0; i < process->NumberOfThreads; ++i) {
                    SYSTEM_THREAD *thread = &(process->Threads[i]);
                    if (!thread) {
                        Q_ASSERT(false);
                        continue;
                    }

                    const bool debuggingThread = process->NumberOfThreads > 1 && i == process->NumberOfThreads - 1;
                    if (debuggingThread) {
                        continue;
                    }

                    const bool running = !(thread->ThreadState == Waiting && thread->WaitReason == Suspended);
                    //qWarning() << "Thread#" << i << thread->Priority << thread->BasePriority << thread->ContextSwitches << thread->ThreadState << thread->WaitReason;

                    if (running) {
                        suspended = false;
                        break;
                    }
                }

                pinfo.state = suspended ?
                    GammaRay::ProcessTracker::Suspended : GammaRay::ProcessTracker::Running;
            }
        }
    }

    emit processChecked(pinfo);
}
