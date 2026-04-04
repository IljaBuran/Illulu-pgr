#pragma once

#include "Core/Window.h"


#define FATAL_CRASH(DESC)                         \
{                                                 \
    char buffer[1024];                            \
    std::snprintf(buffer, sizeof(buffer),         \
        "%s\n\nFile: %s\nLine: %d\nFunction: %s", \
        DESC, __FILE__, __LINE__, __func__);      \
    Window::message_box("CRASH", buffer);         \
    std::exit(EXIT_FAILURE);                      \
}