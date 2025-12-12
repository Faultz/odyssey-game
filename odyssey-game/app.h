#pragma once

#include <liborbisrender.h>
#include <audioout.h>
#include <liborbisutil.h>

class Application : public render_context
{
public:
	bool init(uint32_t flags, std::function<void(int)> callback, std::function<void(ImGuiIO&)> font_callback = nullptr);
	void cleanup();

	bool update(int flipIndex);
	void render();

	bool is_open;
};

inline Application app; // Global application instance