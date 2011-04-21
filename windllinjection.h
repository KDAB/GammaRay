#ifndef WINDLLINJECTION_H
#define WINDLLINJECTION_H

/**
* @brief This class is used to abstract a dll injection
*/

#include <QtCore/QString>
#include <windows.h>

namespace Endoscope {

class WinDllInjection {
    public:
        /**
        * Returns an empty injection object.
        */
        WinDllInjection();

        /**
        * This function sets the destination process and its main thread.
        */
        void setDestinationProcess(HANDLE proc, HANDLE thread);

        /**
        * This functions sets the path to the dll that should be injected
        */
        void setInjectionDll(QString dllPath);

        /**
        * this function injects the dll
        */
        bool inject();
    private:
        HANDLE m_destProcess;
        HANDLE m_destThread;
        QString m_dllPath;

};
}


#endif // WINDLLINJECTION_H
