#include "xr/XREyeViews/XREyeViews.h"
#include "core/Logger.h"

void XREyeViews::Init(XrInstance xrInstance, XrSystemId systemId)
{
    m_Views.clear();

    uint32_t viewCount = 0;
    xrEnumerateViewConfigurationViews(
        xrInstance,
        systemId,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        0,
        &viewCount,
        nullptr
    );

    if (viewCount == 0)
    {
        LOG_ERROR("[OpenXR] No view configuration views found.");
        return;
    }

    std::vector<XrViewConfigurationView> viewConfigs(viewCount);
    for (auto& v : viewConfigs) v.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

    xrEnumerateViewConfigurationViews(
        xrInstance,
        systemId,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        viewCount,
        &viewCount,
        viewConfigs.data()
    );

    m_Views.resize(viewCount);
    for (uint32_t i = 0; i < viewCount; ++i)
    {
        m_Views[i].recommendedWidth = viewConfigs[i].recommendedImageRectWidth;
        m_Views[i].recommendedHeight = viewConfigs[i].recommendedImageRectHeight;
        m_Views[i].recommendedSampleCount = viewConfigs[i].recommendedSwapchainSampleCount;
    }

    LOG_INFO("[OpenXR] Eye views initialized. Count: " + std::to_string(viewCount));
}
