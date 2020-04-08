#pragma once
#include "IWidget.h"

class Widget_Inspector final : public IWidget
{
public:
	Widget_Inspector(class Context* context);
	~Widget_Inspector() = default;

	void Render() override;

private:
    void ShowTransform(std::shared_ptr<class Transform>& transform) const;
	void ShowRenderable(std::shared_ptr<class Renderable>& renderable) const;
	void ShowMaterial(std::shared_ptr<class Material>& material) const;
	void ShowScript(std::shared_ptr<class Script>& script) const;
	void ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source);
	void ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener);

private:
    void ShowAddComponentButton();
	void ShowComponentPopup();
	
private:
    std::unique_ptr<ColorPicker> material_color_picker;
};