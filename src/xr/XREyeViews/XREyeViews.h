#pragma once

#include "xr/OpenXRCommon.h"

#include <vector>

struct XREyeViewInfo
{
    uint32_t recommendedWidth = 0;
    uint32_t recommendedHeight = 0;
    uint32_t recommendedSampleCount = 1;
};

class XREyeViews
{
public:
    void Init(XrInstance xrInstance, XrSystemId systemId);

    const std::vector<XREyeViewInfo>& GetViews() const { return m_Views; }

private:
    std::vector<XREyeViewInfo> m_Views;
};
