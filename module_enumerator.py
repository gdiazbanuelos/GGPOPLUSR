import ctypes
from ctypes import POINTER, Structure, c_char, pointer
from ctypes.wintypes import BYTE, DWORD, HMODULE
from sys import getsizeof


def getModuleAddressByPIDandName(pid, name):
    # const variable
    # Establish rights and basic options needed for all process declartion / iteration
    TH32CS_SNAPPROCESS = 2
    STANDARD_RIGHTS_REQUIRED = 0x000F0000
    SYNCHRONIZE = 0x00100000
    PROCESS_ALL_ACCESS = STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF
    TH32CS_SNAPMODULE = 0x00000008
    TH32CS_SNAPTHREAD = 0x00000004

    class MODULEENTRY32(Structure):
        _fields_ = [
            ("dwSize", DWORD),
            ("th32ModuleID", DWORD),
            ("th32ProcessID", DWORD),
            ("GlblcntUsage", DWORD),
            ("ProccntUsage", DWORD),
            ("modBaseAddr", POINTER(BYTE)),
            ("modBaseSize", DWORD),
            ("hModule", HMODULE),
            ("szModule", c_char * 256),
            ("szExePath", c_char * 260),
        ]

    CreateToolhelp32Snapshot = ctypes.windll.kernel32.CreateToolhelp32Snapshot
    Process32First = ctypes.windll.kernel32.Process32First
    Process32Next = ctypes.windll.kernel32.Process32Next
    Module32First = ctypes.windll.kernel32.Module32First
    Module32Next = ctypes.windll.kernel32.Module32Next
    GetLastError = ctypes.windll.kernel32.GetLastError
    OpenProcess = ctypes.windll.kernel32.OpenProcess
    GetPriorityClass = ctypes.windll.kernel32.GetPriorityClass
    CloseHandle = ctypes.windll.kernel32.CloseHandle

    try:
        addressToReturn = None

        ProcessID = pid
        hModuleSnap = DWORD
        me32 = MODULEENTRY32()
        me32.dwSize = getsizeof(MODULEENTRY32)
        # me32.dwSize = 5000

        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID)
        if hModuleSnap == -1:
            print("CreateToolhelp32Snapshot Error [%d]" % GetLastError())
            print(
                "Build the code yourself? This is the error for using 32-bit Python. Try the 64-bit version."
            )

        ret = Module32First(hModuleSnap, pointer(me32))
        if ret == 0:
            print("ListProcessModules() Error on Module32First[%d]" % GetLastError())
            CloseHandle(hModuleSnap)
        global PROGMainBase
        PROGMainBase = False
        while ret:
            if name == me32.szModule.decode("utf-8"):
                addressToReturn = me32.hModule

            ret = Module32Next(hModuleSnap, pointer(me32))
        CloseHandle(hModuleSnap)

        return addressToReturn

    except:
        print("Error in ListProcessModules")
        raise
