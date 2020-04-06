#pragma once

enum class FileDialogType : uint
{
    Browser,
    FileSelection,
};

enum class FileDialogOperation : uint
{
    Open,
    Load,
    Save,
};

enum class FileDialogFilter : uint
{
    All,
    Scene,
    Model,
    Texture,
};

class FileDialogItem final
{
public:
    FileDialogItem(const Thumbnail& thumbnail, const std::string& path)
        : thumbnail(thumbnail)
        , path(path)
    {
        id              = GUID_Generator::Generate();
        label           = FileSystem::GetFileNameFromPath(path);
        is_directory    = FileSystem::IsDirectory(path);
    }
    ~FileDialogItem() = default;

    auto GetPath() const -> const std::string& { return path; }
    auto GetLabel() const -> const std::string& { return label; }
    auto GetID() const -> const uint& { return id; }
    auto GetTexture() const -> const std::shared_ptr<ITexture>& { return thumbnail.texture; }
    auto IsDirectory() const -> const bool { return is_directory; }
    auto GetTimeSinceLastClickedMs() const -> const float { return static_cast<float>(time_since_last_click.count()); }

    void Clicked()
    {
        const auto now          = std::chrono::high_resolution_clock::now();
        time_since_last_click   = now - last_click_time;
        last_click_time         = now;
    }

private:
    Thumbnail thumbnail;
    uint id;
    std::string path;
    std::string label;
    bool is_directory;
    std::chrono::duration<double, std::milli> time_since_last_click;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_click_time;
};

class FileDialog final
{
public:
    FileDialog
    (
        class Context* context,
        const FileDialogType& type,
        const FileDialogOperation& operation,
        const FileDialogFilter& filter,
        const bool& is_windowed
    );
    ~FileDialog() = default;

    auto GetType() const -> const FileDialogType& { return type; }
    auto GetFilter() const -> const FileDialogFilter& { return filter; }

    auto GetOperation() const -> const FileDialogOperation& { return operation; }
    void SetOperation(const FileDialogOperation& operation);

    auto Show(bool* is_visible, std::string* directory = nullptr, std::string* full_path = nullptr) -> const bool;

private:
    void ShowTop(bool* is_visible);
    void ShowMiddle();
    void ShowBottom(bool* is_visible);
    void ShowMenuPopup();

    void ItemDrag(FileDialogItem* item) const;
    void ItemClick(FileDialogItem* item) const;
    void ItemMenuPopup(FileDialogItem* item);

    auto SetDialogCurrentDirectory(const std::string& directory) -> const bool;
    auto UpdateDialogItemFromDirectory(const std::string& directory) -> const bool;

private:
    class Context* context          = nullptr;
    FileDialogType type             = FileDialogType::Browser;
    FileDialogOperation operation   = FileDialogOperation::Load;
    FileDialogFilter filter         = FileDialogFilter::All;
    ImVec2 item_size                = ImVec2(100.0f, 100.0f);
    bool is_windowed                = false;
    bool is_selection               = false;
    bool is_update                  = true;
    std::string title               = "";
    std::string current_directory   = "";
    std::string input_box           = "";
    std::vector<FileDialogItem> items;
    std::function<void(const std::string&)> on_item_clicked;
    std::function<void(const std::string&)> on_item_double_clicked;

	//°úÁ¦
	bool is_change_script_name = false;
	bool is_change = false;
	class Input* input = nullptr; 
};