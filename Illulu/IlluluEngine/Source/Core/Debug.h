#pragma once

#include "Common.h"

#include "Core/Window.h"
#include <cstdlib>
#include <string>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#define FATAL_CRASH(DESC)                         \
{                                                 \
    char buffer[1024];                            \
    std::snprintf(buffer, sizeof(buffer),         \
        "%s\n\nFile: %s\nLine: %d\nFunction: %s", \
        DESC, __FILE__, __LINE__, __func__);      \
    message_box("CRASH", buffer).pop_up();         \
    std::exit(EXIT_FAILURE);                      \
}

struct message_box
{
    enum class MSG_BOX_TYPE : u8
    {
        DEFAULT,
        OK_NO,
        COUNT
    };

    static void* hwnd; // note(ilja): modern c++ guys are having seizure

    std::string title;
    std::string body;
    MSG_BOX_TYPE type;


    message_box(std::string title_, std::string body_, MSG_BOX_TYPE type_ = MSG_BOX_TYPE::DEFAULT)
        : title(std::move(title_)), body(std::move(body_)), type(type_) {}

    bool pop_up();

    static void set_hwnd(void* hwnd_);
};