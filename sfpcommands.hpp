#pragma once
#include <cstdint>
// #include <main.hpp>
#include <types.hpp>

namespace inspur
{

void registerSFPFunctions();

static constexpr const uint8_t completeCode = 0x00;
static constexpr const uint8_t maxArrayLength = 0xFF;
static constexpr const int present_index = 192;
static constexpr const int lpmode_index = 256;
static constexpr const int interrupt_index = 224;
static constexpr const uint8_t block_base = 8;


#pragma pack(push, 1)
struct SFPInfoReq
{
    uint8_t sfpIndex;
};

struct SFPSetModeReq
{
    uint8_t sfpIndex;
    uint8_t lowMode;
};

struct SFPStatusRes
{
    uint8_t completeCode;
    uint8_t status;
};

struct SFPSumRes
{
    uint8_t completeCode;
    uint8_t sum;
};

struct SFPEEPromRes
{
    uint8_t completeCode;
    uint8_t eeprom[maxArrayLength];
};

struct InpurCompleteCodeRes
{
    uint8_t completeCode;
};
# pragma pack(pop)

}