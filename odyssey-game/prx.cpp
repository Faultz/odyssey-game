#include "stdafx.h"
#include <liborbisrender.h>
#include <png_dec.h>

#include <audioout.h>

#include "app.h"

#pragma comment(lib, "liborbisrender.a")

// We need to provide an export to force the expected stub library to be generated
__declspec (dllexport) void dummy()
{
}

void run_menu()
{
	static bool has_prev_pressed_key = false;
	bool touchpad_down = liborbisutil::pad::current_pad_data.touchData.touchNum == 2;
	if (touchpad_down && !has_prev_pressed_key)
	{
		has_prev_pressed_key = true;
		app.is_open = !app.is_open;
	}
	else if (!touchpad_down)
	{
		has_prev_pressed_key = false;
	}

	if (!app.is_open)
		return;

	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");

	// test osk
	static char buf[256] = { 0 };
	ImGui::InputText("string", buf, 256);

	ImGui::End();
}

void render(int flipIndex)
{
	if (app.update(flipIndex))
	{
		run_menu();

		app.render(flipIndex);
	}
}

extern "C"
{
	int __cdecl module_start(size_t argc, const void* args)
	{
		liborbisutil::thread t([](void*) {
			auto res = MH_Initialize();
			if (res != MH_OK)
			{
				LOG_ERROR("failed to initialize minhook: %d\n", res);
				return;
			}

			liborbisutil::patcher::create("mutex on list patch", "libkernel.sprx", 0x7850, { 0xC3 }, true);

			app.init(FunctionImGui | HookFlip, render);

			liborbisutil::pad::initialize(liborbisutil::pad::state::read_state, true, [](ScePadData* pad, int num) {

				ImGui_ImplOrbis_Data* bd = ImGui_ImplOrbis_GetBackendData();
				if (bd)
					memcpy(&bd->m_sce_pad, pad, sizeof(ScePadData));

				});

			}, "init");

		t.join();

		return SCE_OK;
	}

	int __cdecl module_stop(size_t argc, const void* args)
	{
		liborbisutil::thread t([](void*) {

			liborbisutil::pad::finalize();
			app.cleanup();

			sceKernelUsleep(500 * 1000);

			MH_Uninitialize();
			}, "release");

		t.join();

		return SCE_OK;
	}
}