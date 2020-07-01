#pragma once

class ColorPicker final
{
public:
	ColorPicker(const std::string& title);
	~ColorPicker()=default;

	auto GetColor() const -> const Color4& {return color;}
	void SetColor(const Color4& color) {this->color=color;}

	void Update();

private:
    void ShowColorPicker();

private:
   std::string title= "";
   Color4 color=Color4::Black;
   bool is_visible=false;
};