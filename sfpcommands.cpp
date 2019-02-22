#include <ipmid/api.h>
#include <utils.hpp>
#include <fcntl.h>
#include <boost/container/flat_map.hpp>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <sdbusplus/bus.hpp>
#include <sfpcommands.hpp>
// #include <main.hpp>

namespace inspur
{

void registerSFPFunctions() __attribute__((constructor));


int getSFPGPIOValue(const int gpio)
{
    char path[64];
    char value_str[3];
    int fd;

    std::snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        std::fprintf(stderr, "Failed to open gpio value for reading!\n");
        return -1;
    }

    if (read(fd, value_str, 3) < 0)
    {
        std::fprintf(stderr, "Failed to read value!\n");
        return -1;
    }

    close(fd);
    return (atoi(value_str));
}

int setSFPGPIOValue(const int gpio, int value)
{
    char path[64];
    static const char value_str[] = "01";
    int fd;

    std::snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        std::fprintf(stderr, "Failed to open gpio value for reading!\n");
        return -1;
    }

    int rst = write(fd, &value_str[value == 0 ? 0 : 1], 1);
    if (rst < 0)
    {
        std::fprintf(stderr, "Failed to write value! %d\n", rst);
        return -1;
    }

    close(fd);
    return 0;
}

int initSFPGpio(int gpio)
{
    char buffer[64];
    int len;
    int fp;
    fp = open("/sys/class/gpio/export", O_WRONLY);
    if (fp == -1)
    {
        std::fprintf(stderr, "Failed to open gpio export!");
        close(fp);
        return -1;
    }
    len = snprintf(buffer, sizeof(buffer), "%d", gpio);    
    if (write(fp, buffer, len) < 0)
    {
        std::fprintf(stderr, "Failed to export gpio!");
    }
    close(fp);
    return 0;
}

int gpioSFPUnexport(int gpio)
{
    char buffer[64];
    int len;
    int fp;
    fp = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fp == -1)
    {
        std::fprintf(stderr, "Failed to open gpio export!");
        close(fp);
        return -1;
    }
    len = snprintf(buffer, sizeof(buffer), "%d", gpio);    
    if (write(fp, buffer, len) < 0)
    {
        std::fprintf(stderr, "Failed to unexport gpio!");
        close(fp);
        return -1;
    }
    close(fp);
    return 0;
}

int setSFPDirection(int gpio, int dir)
{
    // 0-->IN, 1-->OUT
    static const char dir_str[] = "in\0out";
    char path[64];
    int fp;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio);
    fp = open(path, O_WRONLY);
    if (fp < 0)
    {
        std::fprintf(stderr, "Failed to open gpio direction!");
        close(fp);
        return -1;
    }

    if (write(fp, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0)
    {
        std::fprintf(stderr, "Failed to set gpio direction!");
        return -1;
    }

    close(fp);
    return 0;
}

int getSFPSum()
{
    int sum = 32;
    return sum;
}

// constexpr auto sfp_service = "xyz.openbmc_project.Sfp.sfp";
// constexpr auto sfp_object = "/xyz/inspur/sfp";
// constexpr auto sfp_interface = "xyz.openbmc_project.Sfp.sfp%d";
// constexpr auto sfp_lowpower_property = "LowPowerMode";
// constexpr auto sfp_Present_property = "Present";
// constexpr auto sfp_EEPROM_property = "eeprom";
// constexpr auto sfp_reset_method = "Reset";

// bool getSFPLowPowerMode(int sfp_id)
// {
//     sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};

//     ipmi::DbusObjectInfo sfpObject = ipmi::getDbusObject(bus, sfp_interface);
//     auto variant = ipmi::getDbusProperty(bus, sfpObject.second, sfpObject.first, sfp_interface, sfp_lowpower_property);

//     return sdbusplus::message::variant_ns::get<bool>(variant);
//     // Value value;
//     // auto bus = sdbusplus::bus::new_default();
//     // auto method = bus.new_method_call(sfp_service, sfp_object, PROP_INTF, METHOD_GET);
//     // method.append(sfp_interface, sfp_lowpower_property);

//     // auto reply = bus.call(method);

//     // if (reply.is_method_error())
//     // {
//     //     std::fprintf(stderr, "Failed to call dbus method!\n");
//     // }
//     // reply.read(value);
//     // return variant_ns::get<bool>(value);
// }

// int getSFPSum()
// {
//     int sum = 32;
//     return sum;
// }

// bool getSFPPresent(int sfp_id)
// {
//     sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};

//     ipmi::DbusObjectInfo sfpObject = ipmi::getDbusObject(bus, sfp_interface);
//     auto variant = ipmi::getDbusProperty(bus, sfpObject.second, sfpObject.first, sfp_interface, sfp_Present_property);

//     return sdbusplus::message::variant_ns::get<bool>(variant);
// }

// int setSFPLowPowerMode(int sfp_id, bool mode)
// {
//     sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};
//     ipmi::setDbusProperty(bus, sfp_service, sfp_object, sfp_interface, sfp_lowpower_property, mode);
//     return 1;
// }

// int resetSFP(int sfp_id)
// {
//     sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};
//     ipmi::method_no_args::callDbusMethod(bus, sfp_service, sfp_object, sfp_interface, sfp_reset_method);
//     return 1;
// }

// int getSFPEEPROM(int sfp_id)
// {
//     sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};
//     ipmi::DbusObjectInfo sfpObject = ipmi::getDbusObject(bus, sfp_interface);
//     auto variant = ipmi::getDbusProperty(bus, sfpObject.second, sfpObject.first, sfp_interface, sfp_EEPROM_property);
//     return sdbusplus::message::variant_ns::get<uint8_t>(variant);
// }

ipmi_ret_t ipmiGetSFPPresent(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<SFPInfoReq*>(request);
    SFPStatusRes sfpStatusRes{};

    if (req->sfpIndex >= getSFPSum())
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    *dataLen = sizeof(SFPStatusRes);

    int gpio = present_index - (req->sfpIndex / block_base) * block_base + req->sfpIndex % block_base;

    int initFlag = initSFPGpio(gpio);
    if (initFlag == -1)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int sfpPresent = getSFPGPIOValue(gpio);
    if (sfpPresent == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    int unexport = gpioSFPUnexport(gpio);
    if (unexport == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    sfpStatusRes.completeCode = completeCode;
    sfpStatusRes.status = sfpPresent == 1 ? 0x00 : 0x01; // gpio value 0 : present
    std::memcpy(response, &sfpStatusRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetSFPInterruptInfo(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<SFPInfoReq*>(request);
    SFPStatusRes sfpStatusRes{};

    if (req->sfpIndex >= getSFPSum())
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    *dataLen = sizeof(SFPStatusRes);

    int gpio = interrupt_index - (req->sfpIndex / block_base) * block_base + req->sfpIndex % block_base;

    int initFlag = initSFPGpio(gpio);
    if (initFlag == -1)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int sfpInterrupt = getSFPGPIOValue(gpio);
    if (sfpInterrupt == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    int unexport = gpioSFPUnexport(gpio);
    if (unexport == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    sfpStatusRes.completeCode = completeCode;
    sfpStatusRes.status = sfpInterrupt ? 0x00 : 0x01; // gpio value 0 : interrupt
    std::memcpy(response, &sfpStatusRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetSFPLowPowerMode(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<SFPInfoReq*>(request);
    SFPStatusRes sfpStatusRes{};

    if (req->sfpIndex >= getSFPSum())
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    *dataLen = sizeof(SFPStatusRes);

    int gpio = lpmode_index - (req->sfpIndex / block_base) * block_base + req->sfpIndex % block_base;

    int sfpLPMode = getSFPGPIOValue(gpio);
    if (sfpLPMode == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    sfpStatusRes.completeCode = completeCode;
    sfpStatusRes.status = sfpLPMode == 1 ? 0x01 : 0x00; // gpio value 1 : enable low power mode.
    std::memcpy(response, &sfpStatusRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetSFPSum(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    SFPSumRes sfpSumRes{};

    *dataLen = sizeof(SFPSumRes);

    sfpSumRes.completeCode = completeCode;
    sfpSumRes.sum = getSFPSum();
    std::memcpy(response, &sfpSumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiSFPReset(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    // auto req = static_cast<SFPInfoReq*>(request);
    InpurCompleteCodeRes inpurCompleteCodeRes{};

    *dataLen = sizeof(InpurCompleteCodeRes);

    // resetSFP(req->sfpIndex);

    inpurCompleteCodeRes.completeCode = completeCode;
    std::memcpy(response, &inpurCompleteCodeRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiSetSFPLowPowerMode(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 2)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<SFPSetModeReq*>(request);
    InpurCompleteCodeRes inpurCompleteCodeRes{};

    *dataLen = sizeof(InpurCompleteCodeRes);

    int gpio = lpmode_index - (req->sfpIndex / block_base) * block_base + req->sfpIndex % block_base;
    int setf = setSFPGPIOValue(gpio, req->lowMode == 0x00 ? 0 : 1);
    if (setf == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    inpurCompleteCodeRes.completeCode = completeCode;
    std::memcpy(response, &inpurCompleteCodeRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmigetSFPEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    // auto req = static_cast<SFPInfoReq*>(request);
    SFPEEPromRes sfpEEPromRes{};

    *dataLen = sizeof(SFPEEPromRes);

    sfpEEPromRes.completeCode = completeCode;
    // sfpEEPromRes.eeprom = getSFPEEPROM(req->sfpIndex);
    std::memcpy(response, &sfpEEPromRes, *dataLen);

    return IPMI_CC_OK;
}

void registerSFPFunctions()
{
    // <Get SFP number>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPNumber),
        NULL, ipmiGetSFPSum, PRIVILEGE_USER);

    // <Get SFP present>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPPrecentStatus),
        NULL, ipmiGetSFPPresent, PRIVILEGE_USER);

    // <Get SFP low power mode>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPLowPowerMode),
        NULL, ipmiGetSFPLowPowerMode, PRIVILEGE_USER);
    
    // <set SFP low power mode>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdSetSFPLowPowerMode),
        NULL, ipmiSetSFPLowPowerMode, PRIVILEGE_USER);

    // <Rest SFP>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdResetSFP),
        NULL, ipmiSFPReset, PRIVILEGE_USER);

    // <Get SFP EEPROM>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPEEPROMInfo),
        NULL, ipmigetSFPEEPROM, PRIVILEGE_USER);

    // <Get SFP interrupt>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPInterruptInfo),
        NULL, ipmiGetSFPInterruptInfo, PRIVILEGE_USER);

    return;
}
} // namespace ipmi
