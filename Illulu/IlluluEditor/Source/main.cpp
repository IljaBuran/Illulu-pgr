//-----------------------------------------------------------------------------------------
/**
 * \file       main.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      [magic starts here] Starts the editor application and runs the main loop.
 */
 //-----------------------------------------------------------------------------------------

#include "pch.h"

#include "EditorApp.h"

int main()
{
    EditorApp app;
    if (!app.init())
        return -1;

    return app.run();
}