#pragma once

#include <all.hpp>

class VolumeCommand : public liteshell::BaseCommand
{
public:
    VolumeCommand()
        : liteshell::BaseCommand(
              "volume",
              "Display volume information",
              "",
              liteshell::CommandConstraint(
                  "root",
                  "The root directory of the volume to be described, a trailing backslash is required. (e.g. \"C:\\\")",
                  true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto root = utils::utf_convert(context.get("root"));

        WCHAR volume_name[MAX_PATH], volume_fs_name[MAX_PATH];
        DWORD volume_serial_number, volume_max_component_length, volume_fs_flags;

        if (!GetVolumeInformationW(
                root.c_str(),                 // lpRootPathName
                volume_name,                  // lpVolumeNameBuffer
                MAX_PATH,                     // nVolumeNameSize
                &volume_serial_number,        // nVolumeSerialNumber
                &volume_max_component_length, // lpMaximumComponentLength
                &volume_fs_flags,             // lpFileSystemFlags
                volume_fs_name,               // lpFileSystemNameBuffer
                MAX_PATH                      // nFileSystemNameSize
                ))
        {
            throw std::runtime_error(utils::last_error("GetVolumeInformationW ERROR"));
        }

        auto display_bool = [](bool value)
        {
            return value ? "Yes" : "No";
        };

        auto display = utils::Table("Attribute", "Value");
        display.add_row("Volume name", utils::utf_convert(std::wstring(volume_name)));
        display.add_row("Serial number", utils::to_hex_string(volume_serial_number));
        display.add_row("Max component length", std::to_string(volume_max_component_length));
        display.add_row("Case-sensitive file names", display_bool(volume_fs_flags & FILE_CASE_SENSITIVE_SEARCH));
        display.add_row("Preserved case of file names", display_bool(volume_fs_flags & FILE_CASE_PRESERVED_NAMES));
        display.add_row("Unicode file names on disk", display_bool(volume_fs_flags & FILE_UNICODE_ON_DISK));
        display.add_row("File-based compression", display_bool(volume_fs_flags & FILE_FILE_COMPRESSION));
        display.add_row("Disk quotas", display_bool(volume_fs_flags & FILE_VOLUME_QUOTAS));
        display.add_row("Sparse files", display_bool(volume_fs_flags & FILE_SUPPORTS_SPARSE_FILES));
        display.add_row("Reparse points", display_bool(volume_fs_flags & FILE_SUPPORTS_REPARSE_POINTS));
        display.add_row("Remote storage", display_bool(volume_fs_flags & FILE_SUPPORTS_REMOTE_STORAGE));
        display.add_row("POSIX-style delete/rename", display_bool(volume_fs_flags & FILE_SUPPORTS_POSIX_UNLINK_RENAME));
        display.add_row("Compressed volume", display_bool(volume_fs_flags & FILE_VOLUME_IS_COMPRESSED));
        display.add_row("Object identifiers", display_bool(volume_fs_flags & FILE_SUPPORTS_OBJECT_IDS));
        display.add_row("Encrypted File System (EFS)", display_bool(volume_fs_flags & FILE_SUPPORTS_ENCRYPTION));
        display.add_row("Named streams", display_bool(volume_fs_flags & FILE_NAMED_STREAMS));
        display.add_row("Read-only", display_bool(volume_fs_flags & FILE_READ_ONLY_VOLUME));
        display.add_row("Filesystem", utils::utf_convert(volume_fs_name));

        std::cout << display.display() << std::endl;

        return 0;
    }
};
