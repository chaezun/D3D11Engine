#pragma once

class Editor final
{
public:
    Editor();
    ~Editor();

    void Resize(const uint& width, const uint& height);
    void Update();
    void Render();

private:
    void BeginDockspace();
    void EndDockspace();
    void ApplyStyle();

private:
    std::unique_ptr<class Engine> engine;
    std::vector<std::unique_ptr<class IWidget>> widgets;

    class Context* context      = nullptr;
    class Graphics* graphics    = nullptr;
    bool is_using_dock_space    = true;
};