import ctypes as c
import struct
from ctypes import wintypes as w

from module_enumerator import getModuleAddressByPIDandName
from pid_searcher import getPIDByName

k32 = c.windll.kernel32

OpenProcess = k32.OpenProcess
OpenProcess.argtypes = [w.DWORD, w.BOOL, w.DWORD]
OpenProcess.restype = w.HANDLE

GetLastError = k32.GetLastError
GetLastError.argtypes = None
GetLastError.restype = w.DWORD

ReadProcessMemory = k32.ReadProcessMemory
ReadProcessMemory.argtypes = [
    w.HANDLE,
    w.LPCVOID,
    w.LPVOID,
    c.c_size_t,
    c.POINTER(c.c_size_t),
]
ReadProcessMemory.restype = w.BOOL

CloseHandle = k32.CloseHandle
CloseHandle.argtypes = [w.HANDLE]
CloseHandle.restype = w.BOOL

process_name = "GGXXACPR_Win.exe"

PROCESS_VM_READ = 0x10
PROCESS_VM_WRITE = 0x20

OFFSET_CONFIG = 0x51A080
OFFSET_RECORDING = 0x4FDD28


class Input(c.Structure):
    _fields_ = [
        ("direction", c.c_ubyte),
        ("button", c.c_ubyte),
        ("pad", c.c_ushort)
    ]

class Recording(c.Structure):
    _fields_ = [
        ("player", c.c_ubyte),
        ("unknown", c.c_ubyte * 3),
        ("inputs", Input * 899)
    ]

class Config(c.Structure):
    _fields_ = [
        ("buttons", c.c_ushort * 18),
    ]

class MemoryReader:

    def __init__(self):
        self.pid = -1
        self.needReaquireGameState = True
        self.needReacquireModule = True
        self.module = 0
        self.pHandle = None

    def getValueFromAddress(
        self, processHandle, address, isFloat=False, is64bit=False, isString=False,
    ):
        if isString:
            data = c.create_string_buffer(16)
            bytesRead = c.c_ulonglong(16)
        elif is64bit:
            data = c.c_ulonglong()
            bytesRead = c.c_ulonglong()
        else:
            data = c.c_ulong(4)
            bytesRead = c.c_ulonglong(4)

        successful = ReadProcessMemory(
            processHandle, address, c.byref(data), c.sizeof(data), c.byref(bytesRead)
        )
        if not successful:
            e = GetLastError()
            print("ReadProcessMemory Error: Code " + str(e))
            self.reacquireEverything()

        value = data.value

        if isFloat:
            return struct.unpack("<f", value)[0]
        elif isString:
            try:
                return value.decode("utf-8")
            except:
                print("ERROR: Couldn't decode string from memory")
                return "ERROR"
        else:
            return int(value)

    def getBlockOfData(self, processHandle, address, size_of_block):
        data = c.create_string_buffer(size_of_block)
        bytesRead = c.c_ulonglong(size_of_block)
        successful = ReadProcessMemory(
            processHandle, address, c.byref(data), c.sizeof(data), c.byref(bytesRead)
        )
        if not successful:
            e = GetLastError()
            print("Getting Block of Data Error: Code " + str(e))
        return data

    def reacquireEverything(self):
        self.needReacquireModule = True
        self.needReaquireGameState = True
        self.flagToReacquireNames = True
        self.pid = -1

    def getPid(self):
        return getPIDByName(process_name)

    def get_pid(self):
        return getPIDByName(process_name)

    def hasWorkingPID(self):
        return self.pid > -1

    def getProcess(self):
        if not self.hasWorkingPID():
            self.pid = self.get_pid()
            if not self.hasWorkingPID():
                return False
            self.module = getModuleAddressByPIDandName(self.pid, process_name)
            self.pHandle = OpenProcess(PROCESS_VM_READ, False, self.pid)
            return True

    def readConfig(self):
        if not self.hasWorkingPID():
            if not self.getProcess():
                return None
        conf = Config()
        bytes_read = c.c_ulonglong(c.sizeof(conf))
        successful = ReadProcessMemory(
            self.pHandle, OFFSET_CONFIG + self.module, c.byref(conf), c.sizeof(conf), c.byref(bytes_read)
        )
        if not successful:
            e = GetLastError()
            print("Reading Configuration Error: Code " + str(e))
        return conf

    def readRecording(self, slot):
        if not self.hasWorkingPID():
            if not self.getProcess():
                return None
        rec = Recording()
        addr = OFFSET_RECORDING + (slot - 1) * c.sizeof(Recording) + self.module
        bytes_read = c.c_ulonglong(c.sizeof(Recording))
        successful = ReadProcessMemory(
            self.pHandle, addr, c.byref(rec), c.sizeof(rec), c.byref(bytes_read)
        )
        if not successful:
            e = GetLastError()
            print("Reading Recording {} Error: Code {}".format(slot, e))
            return None
        return rec
