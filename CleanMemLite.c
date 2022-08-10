#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

int main()
{
    DWORD my_pid, ppid;
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32;
    BOOL bRet = FALSE;
    HANDLE process;

    my_pid = GetCurrentProcessId();

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap)
    {
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    bRet = Process32First(hProcessSnap, &pe32);

    if (bRet)
    {
        do
        {
            if (pe32.th32ProcessID == my_pid)
            {
                ppid = pe32.th32ParentProcessID;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);

    printf("my_pid: %4u, ppid: %4u\n", my_pid, ppid);

    while (TRUE)
    {
        FILE *file;
        if (file = fopen("cleanmem_temp", "r"))
        {
            fclose(file);
        }
        else
        {
            return 0;
        }

        if ((process = OpenProcess(
                 PROCESS_QUERY_INFORMATION |
                     PROCESS_VM_READ |
                     PROCESS_ALL_ACCESS,
                 FALSE, ppid)) != NULL)
        {
            if (!EmptyWorkingSet(process))
            {
                printf(TEXT("%lu: EmptyWorkingSet()\n"), GetLastError());
            }
            CloseHandle(process);
        }
        else
        {
            printf(TEXT("%lu: OpenProcess()/n"), GetLastError());
        }
        Sleep(100);
    }
}