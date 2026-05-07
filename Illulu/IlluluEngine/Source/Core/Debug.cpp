//-----------------------------------------------------------------------------------------
/**
 * \file       Debug.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements native Windows message box display and parent window handle setup.
 */
 //-----------------------------------------------------------------------------------------

#include "pch.h"

#include "Common.h"

#include "Core/Debug.h"

void* message_box::hwnd = nullptr;

bool message_box::pop_up()
{
    if (type == MSG_BOX_TYPE::DEFAULT)
    {
        MessageBoxExA(static_cast<HWND>(hwnd), body.c_str(), title.c_str(), MB_OK, LANG_NEUTRAL);
        return true;
    }
    else if (type == MSG_BOX_TYPE::OK_NO)
    {
        i32 res = MessageBoxA(static_cast<HWND>(hwnd), body.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION);
        return res == IDYES;
    }

    return true;
}

void message_box::set_hwnd(void* hwnd_)
{
    message_box::hwnd = static_cast<HWND>(hwnd_);
}