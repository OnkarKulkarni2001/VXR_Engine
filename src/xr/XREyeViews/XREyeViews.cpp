#include "XREyeViews.h"
#include "core/Logger.h"
#include <vector>

void XREyeViews::Init(XrInstance instance, XrSystemId systemId)
{
    // Query view configuration views (usually 2 for stereo)
    uint32_t count = 0;
    xrEnumerateViewConfigurationViews(
        instance,
        systemId,
        m_ViewType,
        0,
        &count,
        nullptr
    );

    std::vector<XrViewConfigurationView> views(count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });

    xrEnumerateViewConfigurationViews(
        instance,
        systemId,
        m_ViewType,
        count,
        &count,
        views.data()
    );

    m_Views.clear();
    m_Views.reserve(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        XREyeViewInfo v{};
        v.recommendedWidth = views[i].recommendedImageRectWidth;
        v.recommendedHeight = views[i].recommendedImageRectHeight;
        v.maxWidth = views[i].maxImageRectWidth;
        v.maxHeight = views[i].maxImageRectHeight;
        m_Views.push_back(v);

        LOG_INFO("XR Eye " + std::to_string(i) +
            " recommended: " + std::to_string(v.recommendedWidth) + "x" + std::to_string(v.recommendedHeight) +
            " max: " + std::to_string(v.maxWidth) + "x" + std::to_string(v.maxHeight));
    }
}
