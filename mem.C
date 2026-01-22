#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
HANDLE pHandle = NULL;
int procs = 0;
typedef struct { char name[MAX_PATH]; uintptr_t baseAddr; } ModEnt;
ModEnt modulesArr[512];
int modulesCount = 0;
uintptr_t mainModule = 0;
int OpenGameProcess(int procID)
{
    if(procID==0) return 0;
    procs = procID;
    HANDLE h = OpenProcess(2035711u, 1, (DWORD)procID);
    if(!h) return 0;
    pHandle = h;
    modulesCount = 0;
    HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)procID);
    if(hs!=INVALID_HANDLE_VALUE){
        MODULEENTRY32 me; me.dwSize = sizeof(me);
        if(Module32First(hs, &me)){
            mainModule = (uintptr_t)me.modBaseAddr;
        }
        Module32First(hs, &me);
        do{
            if(me.szModule[0]){
                int found = 0;
                for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, me.szModule)==0){found=1;break;}
                if(!found && modulesCount < 512){
                    strncpy(modulesArr[modulesCount].name, me.szModule, MAX_PATH-1);
                    modulesArr[modulesCount].baseAddr = (uintptr_t)me.modBaseAddr;
                    modulesCount++;
                }
            }
        }while(Module32Next(hs, &me));
        CloseHandle(hs);
    }
    return 1;
}
void getModules()
{
    if(procs==0) return;
    modulesCount = 0;
    HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)procs);
    if(hs==INVALID_HANDLE_VALUE) return;
    MODULEENTRY32 me; me.dwSize = sizeof(me);
    if(Module32First(hs, &me)){
        mainModule = (uintptr_t)me.modBaseAddr;
        do{
            if(me.szModule[0]){
                int found = 0;
                for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, me.szModule)==0){found=1;break;}
                if(!found && modulesCount < 512){
                    strncpy(modulesArr[modulesCount].name, me.szModule, MAX_PATH-1);
                    modulesArr[modulesCount].baseAddr = (uintptr_t)me.modBaseAddr;
                    modulesCount++;
                }
            }
        }while(Module32Next(hs, &me));
    }
    CloseHandle(hs);
}
int getProcIDFromName(const char *name)
{
    HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hs==INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32 pe; pe.dwSize = sizeof(pe);
    if(Process32First(hs, &pe)){
        do{
            char procNameNoExt[MAX_PATH];
            strcpy(procNameNoExt, pe.szExeFile);
            char *dot = strrchr(procNameNoExt, '.');
            if(dot) *dot = 0;
            if(_stricmp(procNameNoExt, name)==0 || _stricmp(pe.szExeFile, name)==0){
                CloseHandle(hs);
                return (int)pe.th32ProcessID;
            }
        }while(Process32Next(hs, &pe));
    }
    CloseHandle(hs);
    return 0;
}
char* LoadCode(const char *name, const char *file)
{
    char *buf = (char*)malloc(1024);
    buf[0]=0;
    if(file && file[0]){
        GetPrivateProfileStringA("codes", name, "", buf, 1024, file);
        if(buf[0]==0){
            free(buf);
            return _strdup("");
        }
        return buf;
    } else {
        free(buf);
        return _strdup(name);
    }
}
uintptr_t LoadUIntPtrCode(const char *name, const char *path)
{
    char *txt = LoadCode(name, path?path:"");
    if(!txt) return (uintptr_t)0;
    char *plus = strchr(txt, '+');
    char *value = plus ? plus+1 : "";
    uintptr_t res = 0;
    if(!value || value[0]==0){
        free(txt);
        return (uintptr_t)0;
    } else {
        long num = 0;
        num = (long)strtol(value, NULL, 16);
        if(strstr(txt, "base") || strstr(txt, "main")){
            res = (uintptr_t)((intptr_t)mainModule + num);
        } else if(strchr(txt, '+')){
            char copy[1024]; strncpy(copy, txt, 1023); copy[1023]=0;
            char *tok = strtok(copy, "+");
            if(tok){
                int found = 0; uintptr_t modbase = 0;
                for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, tok)==0){ modbase = modulesArr[i].baseAddr; found=1; break; }
                if(!found) getModules();
                for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, tok)==0){ modbase = modulesArr[i].baseAddr; break; }
                res = (uintptr_t)((intptr_t)modbase + num);
            } else res = (uintptr_t)num;
        } else {
            res = (uintptr_t)num;
        }
    }
    free(txt);
    return res;
}
uintptr_t getCode(const char *name, const char *path, int size)
{
    char *text = LoadCode(name, path?path:"");
    if(!text || text[0]==0){
        if(text) free(text);
        return (uintptr_t)0;
    }
    char *text2 = _strdup(text);
    if(strchr(text, '+')){
        char *p = strchr(text, '+');
        if(p) { strncpy(text2, p+1, 1023); text2[1023]=0; }
    }
    uintptr_t result = 0;
    if(strchr(text2, ',')){
        char *arr[64]; int arrc=0;
        char copy[1024]; strncpy(copy, text2, 1023); copy[1023]=0;
        char *t = strtok(copy, ",");
        while(t && arrc<64){ arr[arrc++]=t; t=strtok(NULL, ","); }
        unsigned int arr4[64];
        for(int i=0;i<arrc;i++) arr4[i] = (unsigned int)strtoul(arr[i], NULL, 16);
        unsigned char buf[8];
        uintptr_t baseAddr = 0;
        if(strstr(text, "base") || strstr(text, "main")){
            uintptr_t raddr = (uintptr_t)((intptr_t)mainModule + arr4[0]);
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)raddr, buf, size, &br);
            unsigned int num = 0; memcpy(&num, buf, sizeof(unsigned int));
            uintptr_t up = 0;
            for(int j=1;j<arrc;j++){
                up = (uintptr_t)(num + arr4[j]);
                ReadProcessMemory(pHandle, (LPCVOID)up, buf, size, &br);
                memcpy(&num, buf, sizeof(unsigned int));
            }
            result = up;
        } else if(!strstr(text, "base") && !strstr(text, "main") && strchr(text, '+')){
            char cp2[1024]; strncpy(cp2, text, 1023); cp2[1023]=0;
            char *mod = strtok(cp2, "+");
            uintptr_t mb=0;
            for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, mod)==0){ mb = modulesArr[i].baseAddr; break; }
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)((uintptr_t)((intptr_t)mb + arr4[0])), buf, size, &br);
            unsigned int num=0; memcpy(&num, buf, sizeof(unsigned int));
            uintptr_t up=0;
            for(int j=1;j<arrc;j++){
                up = (uintptr_t)(num + arr4[j]);
                ReadProcessMemory(pHandle, (LPCVOID)up, buf, size, &br);
                memcpy(&num, buf, sizeof(unsigned int));
            }
            result = up;
        } else {
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)((uintptr_t)arr4[0]), buf, size, &br);
            unsigned int num=0; memcpy(&num, buf, sizeof(unsigned int));
            uintptr_t up=0;
            for(int j=1;j<arrc;j++){
                up = (uintptr_t)(num + arr4[j]);
                ReadProcessMemory(pHandle, (LPCVOID)up, buf, size, &br);
                memcpy(&num, buf, sizeof(unsigned int));
            }
            result = up;
        }
    } else {
        int num2 = (int)strtol(text2, NULL, 16);
        unsigned char buf[8];
        if(strstr(text, "base") || strstr(text, "main")){
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)((uintptr_t)((intptr_t)mainModule + num2)), buf, size, &br);
        } else if(!strstr(text, "base") && !strstr(text, "main") && strchr(text, '+')){
            char cp2[1024]; strncpy(cp2, text, 1023); cp2[1023]=0;
            char *mod = strtok(cp2, "+");
            uintptr_t mb=0;
            for(int i=0;i<modulesCount;i++) if(_stricmp(modulesArr[i].name, mod)==0){ mb = modulesArr[i].baseAddr; break; }
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)((uintptr_t)((intptr_t)mb + num2)), buf, size, &br);
        } else {
            SIZE_T br=0;
            ReadProcessMemory(pHandle, (LPCVOID)((uintptr_t)num2), buf, size, &br);
        }
        unsigned int v=0; memcpy(&v, buf, sizeof(unsigned int));
        uintptr_t u = (uintptr_t)v;
        result = u;
    }
    free(text2);
    free(text);
    return result;
}
char* readString(const char *code, const char *file)
{
    unsigned char tmp[11]; tmp[10]=0;
    uintptr_t lpBaseAddress;
    char *lcode = LoadCode(code, file?file:"");
    if(!lcode) return _strdup("");
    if(!strstr(lcode, ",")){
        lpBaseAddress = LoadUIntPtrCode(code, file?file:"");
    } else {
        lpBaseAddress = getCode(code, file?file:"", 4);
    }
    free(lcode);
    SIZE_T br=0;
    if(ReadProcessMemory(pHandle, (LPCVOID)lpBaseAddress, tmp, 10, &br)){
        tmp[10]=0;
        char *out = (char*)malloc(11);
        memcpy(out, tmp, 11);
        return out;
    } else {
        return _strdup("");
    }
}
int writeMemory(const char *code, const char *type, const char *write, const char *file)
{
    unsigned char buf[1024];
    int num = 4;
    uintptr_t lpBaseAddress;
    char *lcode = LoadCode(code, file?file:"");
    if(!lcode) return 0;
    if(!strstr(lcode, ",")){
        lpBaseAddress = LoadUIntPtrCode(code, file?file:"");
    } else {
        lpBaseAddress = getCode(code, file?file:"", 4);
    }
    if(strcmp(type, "float")==0){
        float f = (float)atof(write);
        memcpy(buf, &f, 4); num=4;
    } else if(strcmp(type, "int")==0){
        int v = atoi(write);
        memcpy(buf, &v, 4); num=4;
    } else if(strcmp(type, "byte")==0){
        int v = atoi(write);
        buf[0] = (unsigned char)v; num=1;
    } else if(strcmp(type, "string")==0){
        int l = (int)strlen(write);
        if(l>1023) l=1023;
        memcpy(buf, write, l); num=l;
    } else {
        free(lcode);
        return 0;
    }
    SIZE_T bw=0;
    int ok = WriteProcessMemory(pHandle, (LPVOID)lpBaseAddress, buf, (SIZE_T)num, &bw);
    free(lcode);
    return ok;
}
void closeProcess()
{
    if(pHandle) CloseHandle(pHandle);
    pHandle = NULL;
    procs = 0;
    modulesCount = 0;
    mainModule = 0;
}
