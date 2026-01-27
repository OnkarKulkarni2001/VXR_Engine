#pragma once
#include <openxr/openxr.h>
#include <vector>

struct XREyeViewInfo
{
    uint32_t recommendedWidth = 0;
    uint32_t recommendedHeight = 0;
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
};

class XREyeViews
{
public:
    void Init(XrInstance instance, XrSystemId systemId);

    const std::vector<XREyeViewInfo>& GetViews() const { return m_Views; }
    XrViewConfigurationType GetViewType() const { return m_ViewType; }

private:
    std::vector<XREyeViewInfo> m_Views;
    XrViewConfigurationType m_ViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
};
