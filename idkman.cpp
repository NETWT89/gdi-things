// was bored and decided to write this feel free to use i guess? (credit tho)
bool GetProcessPath(DWORD pid, std::wstring& path) {
    path.clear();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return false;
    }

    wchar_t buf[MAX_PATH * 4];
    DWORD size = static_cast<DWORD>(std::size(buf));
    BOOL ok = QueryFullProcessImageNameW(hProcess, 0, buf, &size);
    CloseHandle(hProcess);

    if (!ok) {
        return false;
    }

    path.assign(buf, size);
    return true;
}
