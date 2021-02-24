#include <iostream>
#include <windows.h>

#define BUFSIZE 512
#define GMT 3

typedef void (*MenuProcessingFunctionPointer)();

struct MenuOption
{
    int choice;
    char const * p_selection_text;
    MenuProcessingFunctionPointer p_processing_function;
};

void OptGetLogicalDrives();
void OptGetLogicalDriveStrings();
void OptGetDriveType();
void OptGetVolumeInformation();
void OptGetDiskFreeSpace();
void OptCreateDirectory();
void OptRemoveDirectory();
void OptCreateFile();
void OptCopyFile();
void OptMoveFile();
void OptMoveFileEx();
void OptGetFileAttributes();
void OptSetFileAttributes();
void OptGetFileInformationByHandle();
void OptGetFileTime();
void OptSetFileTime();

static const MenuOption MAIN_MENU[] =
        {
                {1, "GetLogicalDrives", OptGetLogicalDrives},
                {2, "GetLogicalDriveStrings", OptGetLogicalDriveStrings},
                {3, "GetDriveType", OptGetDriveType},
                {4, "GetVolumeInformation", OptGetVolumeInformation},
                {5, "GetDiskFreeSpace", OptGetDiskFreeSpace},
                {6, "CreateDirectory", OptCreateDirectory},
                {7, "RemoveDirectory", OptRemoveDirectory},
                {8, "CreateFile", OptCreateFile},
                {9, "CopyFile", OptCopyFile},
                {10, "MoveFile", OptMoveFile},
                {11, "MoveFileEx", OptMoveFileEx},
                {12, "GetFileAttributes", OptGetFileAttributes},
                {13, "SetFileAttributes", OptSetFileAttributes},
                {14, "GetFileInformationByHandle", OptGetFileInformationByHandle},
                {15, "GetFileTime", OptGetFileTime},
                {16, "SetFileTime", OptSetFileTime},

        };

int main()
{
    static const char *const menu_title =
            "\n"
            "------------------------------\n"
            "         Main Menu\n"
            "------------------------------\n"
    ;

    while(true) {
        std::cout.write(menu_title, strlen(menu_title));
        for (const auto &opt : MAIN_MENU) {
            std::cout << opt.choice << " - "<< opt.p_selection_text << "\n";
        }
        std::cout << "Enter selection, 0 to quit: ";
        int choice;
        std::cin >> choice;
        if (choice == 0) break;
        for (const auto &opt : MAIN_MENU) {
            if (choice == opt.choice) {
                auto p_function = opt.p_processing_function;
                (p_function)();
                break;
            }
        }
    }
    return EXIT_SUCCESS;
}

void OptGetLogicalDrives() {
    DWORD drives = GetLogicalDrives();
    char d_char = 'A';
    std::cout << "Logical drives: ";
    while (drives > 0) {
        if ((drives & (unsigned long)1) != 0) {
            std::cout << d_char << " ";
        }
        drives >>= (unsigned long)1;
        d_char++;
    }
    std::cout << std::endl;
}

void OptGetLogicalDriveStrings() {
    TCHAR drive_strings[BUFSIZE];
    std::cout << "Drive strings: ";
    if (auto ds_size = GetLogicalDriveStrings(BUFSIZE-1, drive_strings)) {
        for (auto i = 0; i < ds_size; ++i) {
            if (drive_strings[i] != '\0') {
                std::cout << drive_strings[i];
            }
            else {
                std::cout << ' ';
            }
        }
    }
}

void OptGetDriveType() {
    TCHAR drive_path[MAX_PATH+1];
    std::cout << "Enter the root directory for the drive" << std::endl;
    std::cout << "(a trailing backslash is required): " << std::endl;
    std::cin >> drive_path;

    UINT drive_type = GetDriveTypeA(drive_path);
    std::string drive_type_str;
    switch (drive_type) {
        case 0:
            drive_type_str = "UNDEFINABLE_TYPE";
            break;
        case 1:
            drive_type_str = "NOT_EXIST";
            break;
        case DRIVE_REMOVABLE:
            drive_type_str = "DRIVE_REMOVABLE";
            break;
        case DRIVE_FIXED:
            drive_type_str = "DRIVE_FIXED";
            break;
        case DRIVE_REMOTE:
            drive_type_str = "DRIVE_REMOTE";
            break;
        case DRIVE_CDROM:
            drive_type_str = "DRIVE_CDROM";
            break;
        case DRIVE_RAMDISK:
            drive_type_str = "DRIVE_RAMDISK";
            break;
        default:
            drive_type_str = "UNKNOWN_TYPE";
            break;
    }

    std::cout << "Drive type: " << drive_type_str << std::endl;
}

void OptGetVolumeInformation() {
    TCHAR drive_path[MAX_PATH+1];
    std::cout << "Enter the root directory for the drive" << std::endl;
    std::cout << "(a trailing backslash is required): " << std::endl;
    std::cin >> drive_path;

    TCHAR volume_name[MAX_PATH+1];
    DWORD volume_serial;
    DWORD component_len_max;
    DWORD file_system_flags;
    TCHAR file_system_name[MAX_PATH+1];

    GetVolumeInformationA(drive_path, volume_name, MAX_PATH+1,
                          &volume_serial, &component_len_max,
                          &file_system_flags, file_system_name,
                          MAX_PATH+1);

    std::string file_system_flags_str;
    if (FILE_CASE_PRESERVED_NAMES & file_system_flags)
        file_system_flags_str += "FILE_CASE_PRESERVED_NAMES;";
    if (FILE_CASE_SENSITIVE_SEARCH & file_system_flags)
        file_system_flags_str += "FILE_CASE_SENSITIVE_SEARCH;";
    if (FILE_DAX_VOLUME & file_system_flags)
        file_system_flags_str += "FILE_DAX_VOLUME;";
    if (FILE_FILE_COMPRESSION & file_system_flags)
        file_system_flags_str += "FILE_FILE_COMPRESSION;";
    if (FILE_NAMED_STREAMS & file_system_flags)
        file_system_flags_str += "FILE_NAMED_STREAMS;";
    if (FILE_PERSISTENT_ACLS & file_system_flags)
        file_system_flags_str += "FILE_PERSISTENT_ACLS;";
    if (FILE_READ_ONLY_VOLUME & file_system_flags)
        file_system_flags_str += "FILE_READ_ONLY_VOLUME;";
    if (FILE_SEQUENTIAL_WRITE_ONCE & file_system_flags)
        file_system_flags_str += "FILE_SEQUENTIAL_WRITE_ONCE;";
    if (FILE_SUPPORTS_ENCRYPTION & file_system_flags)
        file_system_flags_str += "FILE_SUPPORTS_ENCRYPTION;";
    if (FILE_SUPPORTS_EXTENDED_ATTRIBUTES & file_system_flags)
        file_system_flags_str += "FILE_SUPPORTS_EXTENDED_ATTRIBUTES;";
    if (FILE_SUPPORTS_HARD_LINKS & file_system_flags)
        file_system_flags_str += "FILE_SUPPORTS_HARD_LINKS;";
    if (FILE_SUPPORTS_OBJECT_IDS & file_system_flags)
        file_system_flags_str += "FILE_SUPPORTS_OBJECT_IDS;";
    if (FILE_SUPPORTS_OPEN_BY_FILE_ID & file_system_flags)
        file_system_flags_str += "FILE_SUPPORTS_OPEN_BY_FILE_ID;";



    std::cout << "Volume name: " << volume_name << std::endl;
    std::cout << "Volume serial: " << volume_serial << std::endl;
    std::cout << "Component len max: " << component_len_max << std::endl;
    std::cout << "File system flags: " << file_system_flags_str << std::endl;
    std::cout << "File system name: " << file_system_name << std::endl;
}

void OptGetDiskFreeSpace() {
    TCHAR drive_path[MAX_PATH+1];
    std::cout << "Enter the root directory for the drive" << std::endl;
    std::cout << "(a trailing backslash is required): " << std::endl;
    std::cin >> drive_path;

    DWORD sectors_per_cluster;
    DWORD bytes_per_sector;
    DWORD number_of_free_clusters;
    DWORD total_number_of_clusters;

    GetDiskFreeSpaceA(drive_path, &sectors_per_cluster, &bytes_per_sector,
                      &number_of_free_clusters, &total_number_of_clusters);

    std::cout << "Sectors per cluster: " << sectors_per_cluster << std::endl;
    std::cout << "Bytes per sector: " << bytes_per_sector << std::endl;
    std::cout << "Number of free clusters: " << number_of_free_clusters << std::endl;
    std::cout << "Total number of clusters: " << total_number_of_clusters << std::endl;
}

void OptCreateDirectory() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter the directory path: " << std::endl;
    std::cin >> dir_path;

    CreateDirectoryA(dir_path, nullptr);
}

void OptRemoveDirectory() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter the directory path: " << std::endl;
    std::cin >> dir_path;

    RemoveDirectoryA(dir_path);
}

void OptCreateFile() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter the full file name: " << std::endl;
    std::cin >> dir_path;

    auto file_handle = CreateFileA(dir_path, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   nullptr, OPEN_ALWAYS,
                                   FILE_ATTRIBUTE_READONLY,nullptr);

    std::cout << file_handle << std::endl;
    CloseHandle(file_handle);
}

void OptCopyFile() {
    TCHAR ex_name[MAX_PATH + 1];
    TCHAR new_name[MAX_PATH + 1];
    std::cout << "Enter the existing file name: " << std::endl;
    std::cin >> ex_name;
    std::cout << "Enter the new file name: " << std::endl;
    std::cin >> new_name;

    bool res = CopyFile(ex_name, new_name, false);
    if (!res) {
        if (GetLastError() == ERROR_SHARING_VIOLATION) {
            std::cout << "Error: same files!";
        }
        else {
            std::cout << "Error: " << GetLastError() << std::endl;
        }
    }
}

void OptMoveFile() {
    TCHAR ex_name[MAX_PATH + 1];
    TCHAR new_name[MAX_PATH + 1];
    std::cout << "Enter the existing file name: " << std::endl;
    std::cin >> ex_name;
    std::cout << "Enter the new file name: " << std::endl;
    std::cin >> new_name;

    bool res = MoveFile(ex_name, new_name);
    if (!res) {
        if (GetLastError() == ERROR_SHARING_VIOLATION) {
            std::cout << "Error: same files!";
        }
        else {
            std::cout << "Error: " << GetLastError() << std::endl;
        }
    }
}

void OptMoveFileEx() {
    TCHAR ex_name[MAX_PATH + 1];
    TCHAR new_name[MAX_PATH + 1];
    std::cout << "Enter the existing file name: " << std::endl;
    std::cin >> ex_name;
    std::cout << "Enter the new file name: " << std::endl;
    std::cin >> new_name;

    bool res = MoveFileEx(ex_name, new_name, MOVEFILE_REPLACE_EXISTING);
    if (!res) {
        if (GetLastError() == ERROR_SHARING_VIOLATION) {
            std::cout << "Error: same files!";
        }
        else {
            std::cout << "Error: " << GetLastError() << std::endl;
        }
    }
}

void OptGetFileAttributes() {
    TCHAR file_name[MAX_PATH + 1];
    std::cout << "Enter the file name: " << std::endl;
    std::cin >> file_name;

    DWORD attributes = GetFileAttributes(file_name);

    if (attributes & FILE_ATTRIBUTE_ARCHIVE) {
        std::cout << "Archive ";
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        std::cout << "Directory ";
    }
    if (attributes & FILE_ATTRIBUTE_READONLY) {
        std::cout << "Read-Only ";
    }
    if (attributes & FILE_ATTRIBUTE_COMPRESSED) {
        std::cout << "Compressed ";
    }
    if (attributes & FILE_ATTRIBUTE_DEVICE) {
        std::cout << "Device ";
    }
    if (attributes & FILE_ATTRIBUTE_ENCRYPTED) {
        std::cout << "Encrypted ";
    }
    if (attributes & FILE_ATTRIBUTE_HIDDEN) {
        std::cout << "Hidden ";
    }
    if (attributes & FILE_ATTRIBUTE_INTEGRITY_STREAM) {
        std::cout << "Integrity-Stream ";
    }
    if (attributes & FILE_ATTRIBUTE_NORMAL) {
        std::cout << "Normal ";
    }
    if (attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) {
        std::cout << "Not-Content-Indexed ";
    }
    if (attributes & FILE_ATTRIBUTE_NO_SCRUB_DATA) {
        std::cout << "No-Scrub-Data ";
    }
    if (attributes & FILE_ATTRIBUTE_OFFLINE) {
        std::cout << "Offline ";
    }
    if (attributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) {
        std::cout << "Recall-On-Data-Access ";
    }
    if (attributes & FILE_ATTRIBUTE_RECALL_ON_OPEN) {
        std::cout << "Recall-On-Open ";
    }
    if (attributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        std::cout << "Reparse-Point ";
    }
    if (attributes & FILE_ATTRIBUTE_SPARSE_FILE) {
        std::cout << "Sparce-File ";
    }
    if (attributes & FILE_ATTRIBUTE_SYSTEM) {
        std::cout << "System ";
    }
    if (attributes & FILE_ATTRIBUTE_TEMPORARY) {
        std::cout << "Temporary ";
    }
    if (attributes & FILE_ATTRIBUTE_VIRTUAL) {
        std::cout << "Virtual ";
    }
    std::cout << std::endl;
}

void OptSetFileAttributes() {
    TCHAR file_name[MAX_PATH + 1];
    std::cout << "Enter the file name: " << std::endl;
    std::cin >> file_name;

    std::cout << "Enter the attribute codes" << std::endl;
    std::cout << "one by one (0 to end): " << std::endl;

    DWORD attributes = 0;
    int input = 1;

    while (input != 0) {
        std::cin >> input;
        attributes |= input;
    }

    bool res = SetFileAttributes(file_name, attributes);
    if (!res)
        std::cout << "Error!" << std::endl;
}

void OptGetFileInformationByHandle() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter existing file name: " << std::endl;
    std::cin >> dir_path;

    auto file_handle = CreateFileA(dir_path, GENERIC_READ,
                                   FILE_SHARE_READ,
                                   nullptr, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY,nullptr);

    BY_HANDLE_FILE_INFORMATION file_info;
    GetFileInformationByHandle(file_handle, &file_info);

    DWORD attributes = file_info.dwFileAttributes;

    std::cout << "File Attributes: ";
    if (attributes & FILE_ATTRIBUTE_ARCHIVE) {
        std::cout << "Archive ";
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        std::cout << "Directory ";
    }
    if (attributes & FILE_ATTRIBUTE_READONLY) {
        std::cout << "Read-Only ";
    }
    if (attributes & FILE_ATTRIBUTE_COMPRESSED) {
        std::cout << "Compressed ";
    }
    if (attributes & FILE_ATTRIBUTE_DEVICE) {
        std::cout << "Device ";
    }
    if (attributes & FILE_ATTRIBUTE_ENCRYPTED) {
        std::cout << "Encrypted ";
    }
    if (attributes & FILE_ATTRIBUTE_HIDDEN) {
        std::cout << "Hidden ";
    }
    if (attributes & FILE_ATTRIBUTE_INTEGRITY_STREAM) {
        std::cout << "Integrity-Stream ";
    }
    if (attributes & FILE_ATTRIBUTE_NORMAL) {
        std::cout << "Normal ";
    }
    if (attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) {
        std::cout << "Not-Content-Indexed ";
    }
    if (attributes & FILE_ATTRIBUTE_NO_SCRUB_DATA) {
        std::cout << "No-Scrub-Data ";
    }
    if (attributes & FILE_ATTRIBUTE_OFFLINE) {
        std::cout << "Offline ";
    }
    if (attributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) {
        std::cout << "Recall-On-Data-Access ";
    }
    if (attributes & FILE_ATTRIBUTE_RECALL_ON_OPEN) {
        std::cout << "Recall-On-Open ";
    }
    if (attributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        std::cout << "Reparse-Point ";
    }
    if (attributes & FILE_ATTRIBUTE_SPARSE_FILE) {
        std::cout << "Sparce-File ";
    }
    if (attributes & FILE_ATTRIBUTE_SYSTEM) {
        std::cout << "System ";
    }
    if (attributes & FILE_ATTRIBUTE_TEMPORARY) {
        std::cout << "Temporary ";
    }
    if (attributes & FILE_ATTRIBUTE_VIRTUAL) {
        std::cout << "Virtual ";
    }
    std::cout << std::endl;

    SYSTEMTIME creation_time, access_time, write_time;
    FileTimeToSystemTime(&file_info.ftCreationTime, &creation_time);
    FileTimeToSystemTime(&file_info.ftLastAccessTime, &access_time);
    FileTimeToSystemTime(&file_info.ftLastWriteTime, &write_time);

    std::cout << "Creation Time " << GMT + creation_time.wHour << " h " <<
              creation_time.wMinute << " m " << std::endl;
    std::cout << "Access Time " << GMT + access_time.wHour << " h " <<
              access_time.wMinute << " m " << std::endl;
    std::cout << "Write Time " << GMT + write_time.wHour << " h " <<
              write_time.wMinute << " m " << std::endl;

    std::cout << "Volume Serial Number: " << file_info.dwVolumeSerialNumber << std::endl;
    std::cout << "File Size High: " << file_info.nFileSizeHigh << std::endl;
    std::cout << "File Size Low: " << file_info.nFileSizeLow << std::endl;
    std::cout << "Number Of Links: " << file_info.nNumberOfLinks << std::endl;
    std::cout << "File Index High: " << file_info.nFileIndexHigh << std::endl;
    std::cout << "File Index Low: " << file_info.nFileIndexLow << std::endl;

    CloseHandle(file_handle);
}

void OptGetFileTime() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter existing file name: " << std::endl;
    std::cin >> dir_path;

    auto file_handle = CreateFileA(dir_path, GENERIC_READ,
                                   FILE_SHARE_READ,
                                   nullptr, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY,nullptr);

    FILETIME f_creation_time, f_access_time, f_write_time;
    SYSTEMTIME creation_time, access_time, write_time;
    GetFileTime(file_handle, &f_creation_time, &f_access_time, &f_write_time);

    FileTimeToSystemTime(&f_creation_time, &creation_time);
    FileTimeToSystemTime(&f_access_time, &access_time);
    FileTimeToSystemTime(&f_write_time, &write_time);

    std::cout << "Creation Time " << GMT + creation_time.wHour << " h " <<
              creation_time.wMinute << " m " << std::endl;
    std::cout << "Access Time " << GMT + access_time.wHour << " h " <<
              access_time.wMinute << " m " << std::endl;
    std::cout << "Write Time " << GMT + write_time.wHour << " h " <<
              write_time.wMinute << " m " << std::endl;

    CloseHandle(file_handle);
}

void OptSetFileTime() {
    TCHAR dir_path[MAX_PATH+1];
    std::cout << "Enter existing file name: " << std::endl;
    std::cin >> dir_path;

    auto file_handle = CreateFileA(dir_path, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   nullptr, OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,nullptr);

    FILETIME create_time;
    create_time.dwHighDateTime = 1;
    create_time.dwLowDateTime = 1;
    SetFileTime(file_handle, &create_time, nullptr, nullptr);
    CloseHandle(file_handle);
}