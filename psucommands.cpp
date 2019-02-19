#include <ipmid/api.h>
#include <fcntl.h>
#include <boost/container/flat_map.hpp>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <sdbusplus/timer.hpp>
#include <psucommands.hpp>
// #include <main.hpp>

namespace inspur
{

void registerPSUFunctions() __attribute__((constructor));

int getPSUNum()
{
    return sizeof(psuConf)/sizeof(psuConf[0]);
}

int getGPIOValue(const int gpio)
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

int initGpio(int gpio)
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

int gpioUnexport(int gpio)
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

int setDirection(int gpio, int dir)
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


ipmi_ret_t ipmiGetPSUCount(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    // if (*dataLen != 1)
    // {
    //     *dataLen = 0;
    //     return IPMI_CC_REQ_DATA_LEN_INVALID;
    // }
    *dataLen = 0; // default to 0 in case of an error

    // auto req = static_cast<GetPSUNumReq*>(request);
    GetPSUNumRes psuNumRes{};

    // if (req->psuCmd != psuSubCmd)
    // {
    //     return IPMI_CC_INVALID_FIELD_REQUEST;
    // }

    *dataLen = sizeof(GetPSUNumRes);

    psuNumRes.completeCode = 0x00;
    psuNumRes.psuNum = getPSUNum();
    std::memcpy(response, &psuNumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetPSUStatus(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
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

    auto req = static_cast<GetPSUStatusReq*>(request);
    GetPSUStatusRes psuInfoRes{};
    int psuIndex = req->psuIndex;
    //  判断index是否超出PSU 总数
    int psuNum = getPSUNum();
    if (psuIndex >= psuNum)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int initFlag = initGpio(psuConf[psuIndex]);
    if (initFlag == -1)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int dirFlag = setDirection(psuConf[psuIndex], 0);
    if (dirFlag == -1)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int psuPresent = getGPIOValue(psuConf[psuIndex]);
    if (psuPresent == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    int unexport = gpioUnexport(psuConf[psuIndex]);
    if (unexport == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    *dataLen = sizeof(GetPSUStatusRes);
    psuInfoRes.completeCode = 0x00;
    psuInfoRes.PSUInfo.Index = psuIndex;
    psuInfoRes.PSUInfo.Present = psuPresent == 1 ? 0 : 1;

    std::memcpy(response, &psuInfoRes, *dataLen);

    return IPMI_CC_OK;
}

void registerPSUFunctions()
{
    // <Get PSU number>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetPSUNumber),
        NULL, ipmiGetPSUCount, PRIVILEGE_USER);
    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, IPMINetfnInspurOEMCmd::cmdGetPSUNumber);

    // <Get PSU info>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetPSUStatus),
        NULL, ipmiGetPSUStatus, PRIVILEGE_USER);
    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, IPMINetfnInspurOEMCmd::cmdGetPSUStatus);
    
    return;
}
} // namespace ipmi
