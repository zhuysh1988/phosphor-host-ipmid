#pragma once

#include <ipmid/api.h>

#include <cstdint>
#include "sys_eeprom.h"

using std::uint8_t;

namespace inspur
{

/*
 * Request
 */
constexpr size_t request_rdwr = 0;
constexpr size_t request_offset = 1;
constexpr size_t request_len = 2;
// static u_int8_t eeprom[SYS_EEPROM_SIZE];

#pragma pack(push, 1)
struct GetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
};

struct GetEEPROMInfoRes
{
    uint8_t completeCode;
    void*  result;
};

struct SetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
    uint8_t value[128];
};

struct InpurCompleteCodeRes
{
    uint8_t completeCode;
};

# pragma pack(pop)
void setupGlobalInpsurI2c();

/**
 * I2c is a global i2c-via-ipmi manager and IPMI handler.
 */
class I2c
{
  public:

    /**
     * The i2c-via-ipmi commands go through this method.
     *
     * @param[in] cmd - the IPMI command.
     * @param[in] reqBuf - the IPMI command buffer.
     * @param[in,out] replyBuf - the IPMI response buffer.
     * @param[in,out] dataLen - pointer to request length, set to reply length.
     * @return IPMI return code.
     */
    ipmi_ret_t transfer(ipmi_cmd_t cmd, const uint8_t* reqBuf,
                        uint8_t* replyBuf, size_t* dataLen);
};

} // namespace oem

// #pragma once

// #include <host-ipmid/ipmid-api.h>

// #include <cstdint>
// #include <host-ipmid/oemrouter.hpp>

// using std::uint8_t;

// namespace inspur
// {
// namespace i2c
// {

// /*
//  * EEPROM configure info.
//  */


// /*
//  * Request header
//  */
// constexpr size_t requestHeaderBus = 0;
// constexpr size_t requestHeaderFlags = 1;
// constexpr size_t requestHeaderLen = 2;

// typedef uint8_t BusId;
// typedef uint8_t ReqFlags;

// constexpr ReqFlags requestFlagsUsePec = (1 << 7);

// /*
//  * Request step.
//  */
// constexpr size_t stepHeaderDevAndDir = 0;
// constexpr size_t stepHeaderFlags = 1;
// constexpr size_t stepHeaderParm = 2;
// constexpr size_t stepHeaderLen = 3;

// typedef uint8_t DevAddr;
// typedef uint8_t StepFlags;
// constexpr StepFlags stepFlagsRecvLen = (1 << 7);
// constexpr StepFlags stepFlagsNoStart = (1 << 6);

// // So far 2 steps suffics, so 4 should be safe.
// constexpr size_t maxSteps = 4;

// // Currently we specify 32 byte payload limit;
// // but for block read with PEC that entails 34 total bytes.
// constexpr size_t largestReply = 34;

// enum ToolI2CCmd
// {
//     // 0x01.
//     gpioCmd = 1,
//     // 0x02.
//     i2cCmd = 2,
//     // 0x03.
//     flashCmd = 3,
//     // 0x04.
//     fanManualCmd = 4,
//     // 0x30.
//     ethStatsCmd = 48,
//     // 0x80
//     blobTransferCmd = 128,
// };


// static constexpr ipmi_netfn_t netfunInspurAppOEM = 0x3D;

// void setupGlobalInpsurI2c();

// } // namespace i2c

// /**
//  * I2c is a global i2c-via-ipmi manager and IPMI handler.
//  */
// class I2c
// {
//   public:
//     /**
//      * Allows specification of the mechanism to register OEM IPMI handler.
//      *
//      * @param[in] oemRouter - A pointer to a router instance.
//      */
//     // void registerWith();

//     /**
//      * The i2c-via-ipmi commands go through this method.
//      *
//      * @param[in] cmd - the IPMI command.
//      * @param[in] reqBuf - the IPMI command buffer.
//      * @param[in,out] replyBuf - the IPMI response buffer.
//      * @param[in,out] dataLen - pointer to request length, set to reply length.
//      * @return IPMI return code.
//      */
//     // ipmi_ret_t transfer(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
//     //                                     ipmi_request_t request,
//     //                                     ipmi_response_t response,
//     //                                     ipmi_data_len_t dataLen,
//     //                                     ipmi_context_t context);
//     ipmi_ret_t transfer(ipmi_cmd_t cmd, const uint8_t* reqBuf,
//                         uint8_t* replyBuf, size_t* dataLen);
// };

// } // namespace oem
