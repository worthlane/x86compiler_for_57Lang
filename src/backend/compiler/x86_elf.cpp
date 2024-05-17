#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "x86_elf.h"
#include "x86_encode.h"
#include "common/errors.h"
#include "common/input_and_output.h"
#include "byte_code.h"

#ifdef CHECK_FWRITE
#undef CHECK_FWRITE
#endif
#define CHECK_FWRITE(fwrite_code)   do                                  \
                                    {                                   \
                                        size_t status = fwrite_code;    \
                                                                        \
                                        if (status == -1)               \
                                        {                                   \
                                            error->code = (int) ERRORS::PRINT_DATA; \
                                            error->data = "FWRITE ERROR";   \
                                            return;             \
                                        }       \
                                    } while (0)                         \

static const Elf64_Phdr STARTING_PHDR =
    {
        .p_type = PT_LOAD,          // Type of the program header entry (loadable segment)
        .p_flags = PF_R,            // Segment flags
        .p_offset = 0x0000,         // Offset of the segment in the file

        .p_vaddr = 0x400000,        // Virtual address
        .p_paddr = 0x400000,        // Physical address

        .p_filesz = sizeof(Elf64_Ehdr) + PROGRAM_HDRS_AMT * sizeof(Elf64_Phdr),
        .p_memsz  = sizeof(Elf64_Ehdr) + PROGRAM_HDRS_AMT * sizeof(Elf64_Phdr),

        .p_align = 0x1000
    };

static const Elf64_Phdr MAIN_CODE_PHDR =
    {
        .p_type = PT_LOAD,          // Type of the program header entry (loadable segment)
        .p_flags = PF_R | PF_X,     // Segment flags
        .p_offset = 0x1000,         // Offset of the segment in the file

        .p_vaddr = 0x401000,        // Virtual address
        .p_paddr = 0x401000,        // Physical address

        .p_filesz = 0,
        .p_memsz  = 0,

        .p_align = 0x1000
    };

static const Elf64_Phdr VARIABLES_PHDR =
    {
        .p_type = PT_LOAD,          // Type of the program header entry (loadable segment)
        .p_flags = PF_R | PF_W,     // Segment flags
        .p_offset = 0x2000,         // Offset of the segment in the file

        .p_vaddr = 0x402000,        // Virtual address
        .p_paddr = 0x402000,        // Physical address

        .p_filesz = 0x19,
        .p_memsz  = 0x19,

        .p_align = 0x1000
    };

// ELF header
static const Elf64_Ehdr ELF_HEADER =
    {
        .e_ident =
            {
                ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,  // Magic number identifying ELF file format
                ELFCLASS64, ELFDATA2LSB,
                EV_CURRENT,     // ELF class, data encoding, and version
                ELFOSABI_NONE, 0, 0, 0, 0, 0, 0,
                0              // OS-specific identification (none in this case)
            },

        .e_type    = ET_EXEC,        // Executable file type
        .e_machine = EM_X86_64,   // Target machine architecture
        .e_version = EV_CURRENT,  // ELF version

        .e_entry = START_ADDR,
        .e_phoff = sizeof(Elf64_Ehdr),      // Offset of the program header table
        .e_shoff = 0,                       // Offset of the section header table (none in this case)
        .e_flags = 0,                       // Processor-specific flags
        .e_ehsize = sizeof(Elf64_Ehdr),     // Size of ELF header
        .e_phentsize = sizeof(Elf64_Phdr),  // Size of program header entry
        .e_phnum = PROGRAM_HDRS_AMT,        // Number of program header entries
        .e_shentsize = sizeof(Elf64_Shdr),  // Size of section header entry
        .e_shnum = 0,                       // Number of section header entries
        .e_shstrndx = 0                     // Index of section header entry for section name strings
    };

static const char* STDLIB_CODE_PATH = "src/assets/stdlib_code.bin";
static const char* STDLIB_VARS_PATH = "src/assets/stdlib_vars.bin";

static void PrintProgInElf(FILE* fp, byte_code_t* ProgramCode, error_t* error);
static void FillVoidSpace(FILE* fp, const size_t zeros_amt, error_t* error);
static void PrintStdlib(FILE* out_stream, const char* file_name, const size_t size, error_t* error);

// ======================================================================================

void TranslateIrToElf(const char* file_name, ir_t* ir, error_t* error)
{
    byte_code_t* program_code = ByteCodeCtor(MIN_CODE_SIZE, error);

    FILE* out_stream = OpenFile(file_name, "wb", error);
    BREAK_IF_ERROR(error);

    ByteCodePush(program_code, 0xB8, error);
    ByteCodePush(program_code, 0x3C, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0xBF, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x00, error);
    ByteCodePush(program_code, 0x0F, error);
    ByteCodePush(program_code, 0x05, error);

    PrintProgInElf(out_stream, program_code, error);

    fclose(out_stream);
    ByteCodeDtor(program_code);
}

// --------------------------------------------------------------------------------------------

static void PrintProgInElf(FILE* fp, byte_code_t* program_code, error_t* error)
{
    assert(fp);
    assert(program_code);

    static const int ALIGN = 0x1000;

    Elf64_Phdr main_phdr = MAIN_CODE_PHDR;
    int code_len       = STDLIB_CODE_SIZE + program_code->size * sizeof(uint8_t);
    main_phdr.p_filesz = code_len;
    main_phdr.p_memsz  = code_len;

    Elf64_Phdr vars_phdr = VARIABLES_PHDR;
    int vars_shift      = (STDLIB_CODE_SIZE + program_code->size * sizeof(uint8_t)) / ALIGN;
    vars_phdr.p_vaddr  += vars_shift;
    vars_phdr.p_paddr  += vars_shift;
    vars_phdr.p_offset += vars_shift;

    CHECK_FWRITE(fwrite(&ELF_HEADER, sizeof(Elf64_Ehdr), 1, fp));
    CHECK_FWRITE(fwrite(&STARTING_PHDR, sizeof(Elf64_Phdr), 1, fp));
    CHECK_FWRITE(fwrite(&main_phdr, sizeof(Elf64_Phdr), 1, fp));
    CHECK_FWRITE(fwrite(&vars_phdr, sizeof(Elf64_Phdr), 1, fp));

    size_t zeros_amt = MAIN_CODE_PHDR.p_offset - STARTING_PHDR.p_filesz;
    FillVoidSpace(fp, zeros_amt, error);

    PrintStdlib(fp, STDLIB_CODE_PATH, STDLIB_CODE_SIZE, error);
    CHECK_FWRITE(fwrite(program_code->array, sizeof(uint8_t), program_code->size, fp));

    zeros_amt = vars_phdr.p_offset - code_len - MAIN_CODE_PHDR.p_offset;
    FillVoidSpace(fp, zeros_amt, error);

    PrintStdlib(fp, STDLIB_VARS_PATH, STDLIB_VARS_SIZE, error);
}

// --------------------------------------------------------------------------------------------

static void FillVoidSpace(FILE* fp, const size_t zeros_amt, error_t* error)
{
    assert(error);
    assert(fp);

    uint8_t* array = (uint8_t*) calloc(zeros_amt, sizeof(uint8_t));
    assert(array);

    CHECK_FWRITE(fwrite(array, sizeof(uint8_t), zeros_amt, fp));

    free(array);
}

// --------------------------------------------------------------------------------------------

static void PrintStdlib(FILE* out_stream, const char* file_name, const size_t size, error_t* error)
{
    assert(error);
    assert(out_stream);

    uint8_t* array = (uint8_t*) calloc(size, sizeof(uint8_t));
    assert(array);

    FILE* in_stream = OpenFile(file_name, "rb", error);
    BREAK_IF_ERROR(error);

    fread(array, sizeof(uint8_t), size, in_stream);

    CHECK_FWRITE(fwrite(array, sizeof(uint8_t), size, out_stream));

    free(array);
}

