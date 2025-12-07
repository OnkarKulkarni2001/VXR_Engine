#pragma once
#include "Window.h"

class Application {
public:
    Application();
    void Run();

private:
    Window* m_Window;
};
