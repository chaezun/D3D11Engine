#pragma once

class ScriptEditor final
{
public: 
	static ScriptEditor& Get()
	{
		static ScriptEditor instance;
		return instance;
	}

	void Initialize(class Context* context);
	void Render();

	void SetScript(const std::string& path);

	void CreateNewScript(const std::string& directory);
	void SaveToScript();
	void ChangeScriptName(const std::string& path, const std::string& change_name);

	auto IsVisible() const -> const bool& { return is_visible; }
	void SetVisible(const bool& is_visible) { this->is_visible = is_visible; }

	auto GetScriptDirectory() -> const std::string& { return current_directory; }

private:
	ScriptEditor() = default;
	~ScriptEditor() = default;

private:
    class Context* context = nullptr;
	std::string script_name = "";
    std::string path = "";
	bool is_visible = false;

	std::string current_directory = "";
};