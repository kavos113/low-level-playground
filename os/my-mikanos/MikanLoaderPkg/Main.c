#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

#include "frame_buffer_config.h"
#include "elf.h"

void Halt();

struct MemoryMap
{
    UINTN buffer_size;
    VOID* buffer;
    UINTN map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};

EFI_STATUS GetMemoryMap(struct MemoryMap* map)
{
    if (map->buffer == NULL)
    {
        return EFI_BUFFER_TOO_SMALL;
    }

    map->map_size = map->buffer_size;
    return gBS->GetMemoryMap(
        &map->map_size,
        (EFI_MEMORY_DESCRIPTOR*)map->buffer,
        &map->map_key,
        &map->descriptor_size,
        &map->descriptor_version
    );
}

const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type)
{
    switch (type)
    {
    case EfiReservedMemoryType:
        return L"EfiReservedMemoryType";
    case EfiLoaderCode:
        return L"EfiLoaderCode";
    case EfiLoaderData:
        return L"EfiLoaderData";
    case EfiBootServicesCode:
        return L"EfiBootServicesCode";
    case EfiBootServicesData:
        return L"EfiBootServicesData";
    case EfiRuntimeServicesCode:
        return L"EfiRuntimeServicesCode";
    case EfiRuntimeServicesData:
        return L"EfiRuntimeServicesData";
    case EfiConventionalMemory:
        return L"EfiConventionalMemory";
    case EfiUnusableMemory:
        return L"EfiUnusableMemory";
    case EfiACPIReclaimMemory:
        return L"EfiACPIReclaimMemory";
    case EfiACPIMemoryNVS:
        return L"EfiACPIMemoryNVS";
    case EfiMemoryMappedIO:
        return L"EfiMemoryMappedIO";
    case EfiMemoryMappedIOPortSpace:
        return L"EfiMemoryMappedIOPortSpace";
    case EfiPalCode:
        return L"EfiPalCode";
    case EfiPersistentMemory:
        return L"EfiPersistentMemory";
    default:
        return L"InvalidMemoryType";
    }
}

EFI_STATUS SaveMemoryMap(struct MemoryMap* map, EFI_FILE_PROTOCOL* file)
{
    CHAR8 buf[256];
    UINTN len;

    CHAR8* header = "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
    len = AsciiStrLen(header);
    file->Write(file, &len, header);

    Print(L"map->buffer = %08lx, map->map_size = %08lx\n", map->buffer, map->map_size);

    EFI_PHYSICAL_ADDRESS iter;
    int i;
    for (
        iter = (EFI_PHYSICAL_ADDRESS)map->buffer, i = 0;
        iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;
        iter += map->descriptor_size, i++
    )
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)iter;

        len = AsciiSPrint(
            buf, sizeof(buf),
            "%u, %x, %-ls, %08lx, %lx, %lx\n",
            i, desc->Type, GetMemoryTypeUnicode(desc->Type), desc->PhysicalStart, desc->NumberOfPages,
            desc->Attribute & 0xffffflu
        );
        file->Write(file, &len, buf);
    }

    return EFI_SUCCESS;
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL** root)
{
    EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;

    gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    fs->OpenVolume(fs, root);

    return EFI_SUCCESS;
}

EFI_STATUS OpenGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL** gop)
{
    UINTN num_gop_handles = 0;
    EFI_HANDLE* gop_handles = NULL;
    EFI_STATUS status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &num_gop_handles,
        &gop_handles
    );
    if (EFI_ERROR(status))
    {
        Print(L"Failed to locate gop handles: %r\n", status);
        return status;
    }

    Print(L"num_gop_handles = %d\n", num_gop_handles);

    for (int i = 0; i < num_gop_handles; ++i)
    {
        status = gBS->OpenProtocol(
            gop_handles[i],
            &gEfiGraphicsOutputProtocolGuid,
            (VOID**)gop,
            image_handle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
        );
        if (EFI_ERROR(status))
        {
            Print(L"Failed to locate gop handles: %r\n", status);
            return status;
        }

        UINTN size_of_info;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
        UINTN target_mode = -1;

        for (int j = 0; j < (*gop)->Mode->MaxMode; ++j)
        {
            status = (*gop)->QueryMode(*gop, j, &size_of_info, &info);
            if (EFI_ERROR(status))
            {
                continue;
            }

            Print(
                L"Mode %d: %u x %u, Format: %d, %u pixels/line\n",
                j,
                info->HorizontalResolution,
                info->VerticalResolution,
                info->PixelFormat,
                info->PixelsPerScanLine
            );

            if (info->HorizontalResolution == 1024 && info->VerticalResolution == 768)
            {
                target_mode = j;
                break;
            }
        }

        if (target_mode == -1)
        {
            Print(L"Failed to find 1024x768 mode\n");
            target_mode = 0;
        }

        Print(L"Set mode %d\n", target_mode);
        status = (*gop)->SetMode(*gop, target_mode);
        if (EFI_ERROR(status))
        {
            Print(L"Failed to set mode %d: %r\n", target_mode, status);
            return status;
        }

        if ((*gop)->Mode->FrameBufferBase != 0)
        {
            break;
        }

        gBS->CloseProtocol(
            gop_handles[i],
            &gEfiGraphicsOutputProtocolGuid,
            image_handle,
            NULL
        );
    }

    FreePool(gop_handles);

    return EFI_SUCCESS;
}

const CHAR16* GetPixelFormatUnicode(EFI_GRAPHICS_PIXEL_FORMAT fmt)
{
    switch (fmt)
    {
    case PixelRedGreenBlueReserved8BitPerColor:
        return L"PixelRedGreenBlueReserved8BitPerColor";
    case PixelBlueGreenRedReserved8BitPerColor:
        return L"PixelBlueGreenRedReserved8BitPerColor";
    case PixelBitMask:
        return L"PixelBitMask";
    case PixelBltOnly:
        return L"PixelBltOnly";
    case PixelFormatMax:
        return L"PixelFormatMax";
    default:
        return L"InvalidPixelFormat";
    }
}

void Halt()
{
    while (1)
    {
        __asm__("hlt");
    }
}

void CalcLoadAddressRange(Elf64_Ehdr* ehdr, UINT64* first, UINT64* last)
{
    Elf64_Phdr* phdr = (Elf64_Phdr*)((UINT64)ehdr + ehdr->e_phoff);
    *first = MAX_UINT64;
    *last = 0;

    for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
        {
            continue;
        }

        *first = MIN(*first, phdr[i].p_vaddr);
        *last = MAX(*last, phdr[i].p_vaddr + phdr[i].p_memsz);
    }
}

void CopyLoadSegments(Elf64_Ehdr* ehdr, UINT64 base_addr)
{
    Elf64_Phdr* phdr = (Elf64_Phdr*)((UINT64)ehdr + ehdr->e_phoff);
    for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
        {
            continue;
        }

        UINT64 segm_in_file = (UINT64)ehdr + phdr[i].p_offset;
        CopyMem((VOID *)(base_addr + phdr[i].p_offset), (VOID*)segm_in_file, phdr[i].p_filesz);

        UINTN remain_bytes = phdr[i].p_memsz - phdr[i].p_filesz;
        SetMem((VOID *)(base_addr + phdr[i].p_offset + phdr[i].p_filesz), remain_bytes, 0);
    }
}

EFI_STATUS EFIAPI UefiMain(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE* system_table
)
{
    Print(L"Hello, Mikan World!\n");

    // memmap
    CHAR8 memmap_buf[4096 * 4];
    struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
    GetMemoryMap(&memmap);

    EFI_FILE_PROTOCOL* root_dir;
    OpenRootDir(image_handle, &root_dir);

    EFI_FILE_PROTOCOL* memmap_file;
    root_dir->Open(
        root_dir, &memmap_file, L"\\memmap", EFI_FILE_MODE_READ | EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE, 0
    );

    SaveMemoryMap(&memmap, memmap_file);
    memmap_file->Close(memmap_file);

    // graphics output protocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status = OpenGOP(image_handle, &gop);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to open gop: %r\n", status);
        Halt();
    }
    Print(
        L"Resolution: %ux%u, Pixel Format: %s, %u pixels/l\n",
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        GetPixelFormatUnicode(gop->Mode->Info->PixelFormat),
        gop->Mode->Info->PixelsPerScanLine
    );
    Print(
        L"Frame Buffer: 0x%0lx - 0x%0lx, Size: %lu bytes\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferBase + gop->Mode->FrameBufferSize,
        gop->Mode->FrameBufferSize
    );
    Print(
        L"GOP MOde: %d\n",
        gop->Mode->Mode
    );

    // open kernel file
    EFI_FILE_PROTOCOL* kernel_file;
    root_dir->Open(root_dir, &kernel_file, L"\\kernel.elf", EFI_FILE_MODE_READ, 0);

    UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 file_info_buffer[file_info_size];
    kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid, &file_info_size, file_info_buffer);

    EFI_FILE_INFO* file_info = (EFI_FILE_INFO*)file_info_buffer;
    UINTN kernel_file_size = file_info->FileSize;

    VOID* kernel_buffer;
    status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, &kernel_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"error: %r\n", status);
        Halt();
    }
    status = kernel_file->Read(kernel_file, &kernel_file_size, kernel_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"error: %r\n", status);
        Halt();
    }

    Elf64_Ehdr* kernel_ehdr = (Elf64_Ehdr*)kernel_buffer;
    UINT64 kernel_first_addr, kernel_last_addr;
    CalcLoadAddressRange(kernel_ehdr, &kernel_first_addr, &kernel_last_addr);

    UINTN num_pages = (kernel_last_addr - kernel_first_addr + 0xfff) / 0x1000;

    EFI_PHYSICAL_ADDRESS kernel_base_addr = 0;
    status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderCode, num_pages, &kernel_base_addr);
    if (EFI_ERROR(status))
    {
        Print(L"failed to allocate pages: %r\n", status);
        Halt();
    }

    CopyLoadSegments(kernel_ehdr, kernel_base_addr);
    Print(L"Kernel base: %08lx\n", kernel_base_addr);
    Print(L"Kernel entry: %08lx\n", kernel_ehdr->e_entry);

    // call kernel entry point
    UINT64 entry_addr = kernel_base_addr + kernel_ehdr->e_entry;
    Print(L"Kernel entry address: %08lx\n", entry_addr);

    status = gBS->FreePool(kernel_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"failed to free pool: %r\n", status);
        Halt();
    }

    EFI_PHYSICAL_ADDRESS kernel_stack_addr;
    UINTN stack_pages = 64;
    status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, stack_pages, &kernel_stack_addr);
    if (EFI_ERROR(status))
    {
        Print(L"failed to allocate pages for kernel stack: %r\n", status);
        Halt();
    }
    UINT64 kernel_stack_top = kernel_stack_addr + stack_pages * 0x1000;
    Print(L"Kernel stack address: %08lx - %08lx\n", kernel_stack_addr, kernel_stack_top);

    struct FrameBufferConfig config = {
        (UINT8*)gop->Mode->FrameBufferBase,
        gop->Mode->Info->PixelsPerScanLine,
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        0
    };
    switch (gop->Mode->Info->PixelFormat)
    {
    case PixelRedGreenBlueReserved8BitPerColor:
        config.pixel_format = kPixelRGBResv8BitPerColor;
        break;
    case PixelBlueGreenRedReserved8BitPerColor:
        config.pixel_format = kPixelBGRResv8BitPerColor;
        break;
    default:
        Print(L"unknown pixel format: %d\n", gop->Mode->Info->PixelFormat);
        Halt();
    }

    typedef void EntryPointType(const struct FrameBufferConfig*);
    EntryPointType* entry_point = (EntryPointType*)entry_addr;

    status = gBS->ExitBootServices(image_handle, memmap.map_key);
    if (EFI_ERROR(status))
    {
        Print(L"ExitBootServices failed: %r\n", status);
        status = GetMemoryMap(&memmap);
        if (EFI_ERROR(status))
        {
            Print(L"failed to get memory map: %r\n", status);
            Halt();
        }

        status = gBS->ExitBootServices(image_handle, memmap.map_key);
        if (EFI_ERROR(status))
        {
            Print(L"could not exit boot services: %r\n", status);
            Halt();
        }
    }

    asm volatile(
        "mov %0, %%rdi\n"
        "mov %1, %%rsp\n"
        "call *%2\n"
        :
        : "r"(&config), "r"(kernel_stack_top), "r"(entry_point)
    );

    Print(L"All done.");

    Halt();
    return EFI_SUCCESS;
}
