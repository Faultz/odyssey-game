#include "app.h"

bool Application::init(uint32_t flags, std::function<void(int)> callback, std::function<void(ImGuiIO&)> font_callback)
{
	if (!render_context::create(flags, callback, font_callback))
		return false;

	return true;
}

void Application::cleanup()
{
	render_context::release();
}

bool Application::update(int flipIndex)
{
	if (!begin_scene(flipIndex))
		return false;

	update_scene();

	return true;
}

void Application::render(int flipIndex)
{
	// render
	// render_scene();

	end_scene();
}