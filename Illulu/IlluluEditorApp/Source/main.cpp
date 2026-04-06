#include "Core/App.h"
#include "EditorLayer.h"

#include <memory>

int main()
{
    App app;
    app.push_layer(std::make_unique<EditorLayer>());
    app.init();
    return app.run();
}