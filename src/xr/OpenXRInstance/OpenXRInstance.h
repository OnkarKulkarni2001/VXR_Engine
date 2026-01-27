#pragma once
#include <openxr/openxr.h>
#include <string>

class OpenXRInstance
{
public:
    OpenXRInstance();
    ~OpenXRInstance();

    XrInstance Get() const { return m_Instance; }
    XrSystemId GetSystemId() const { return m_SystemId; }

private:
    void CreateInstance();
    void GetSystem();

private:
    XrInstance m_Instance = XR_NULL_HANDLE;
    XrSystemId m_SystemId = XR_NULL_SYSTEM_ID;
};
