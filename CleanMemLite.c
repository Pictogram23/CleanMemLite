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
        if (file = fopen("temp", "r"))
        {
            fclose(file);
        }
        else
        {
            return 0;
        }

        HANDLE process = OpenProcess(
            PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ |
                PROCESS_ALL_ACCESS,
            FALSE, ppid);
        int emp;
        emp = EmptyWorkingSet(process);
        CloseHandle(process);
        printf("%f\n", emp);
        Sleep(50);
    }
}