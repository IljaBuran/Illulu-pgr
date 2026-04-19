#include "pch.h"

#include "EditorApp.h"

int main()
{
    EditorApp app;
    if (!app.init())
        return -1;

    return app.run();
}