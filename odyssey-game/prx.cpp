#include "stdafx.h"
#include <liborbisrender.h>
#include <png_dec.h>

#include <audioout.h>

#include "app.h"

#pragma comment(lib, "liborbisrender.a")
#pragma comment(lib, "libSceDbgShaderLiveEditing.a")

#include <C:\Users\notmy\Documents\Github\Fusion\External\libFusionDriver/libFusionDriver/FusionDriver.h>
#pragma comment(lib, "C:/Users/notmy/Documents/Github/Fusion/External/libFusionDriver/Build/libFusionDriver.a")

// This sample aims to provide a clear example of the basic usage of the liborbisrender library.
// function flags used: FunctionImGui - to enable ImGui rendering and HookFlip - to hook into the flip process.
// the other flags like HookFlipForWorkload & HookFlipForVideoOut are optional and demonstrate additional hooking capabilities.
// if needed. Most games will rely on just HookFlip but there are scenarios where the other hooks may be beneficial.
// function flags like: FunctionRenderDebug will display additional debug information on screen if FunctionImGui is also enabled.
// UnlockFps will set the fliprate to 0, uncapping framerate at the expense of causing potential screen tearing.
// SubmitSelf will call sce::Gnm::submitDone after each frame submission to help with GPU synchronization in some scenarios.
// RenderBeforeFlip will execute the user render callback before the flip occurs rather than after.
/*
+-----------------------------------------------------------------------+
|     Hook Flag         |       Hooked Function							|
+-----------------------+-----------------------------------------------+
|   HookFlip			|   sceGnmSubmitAndFlipCommandBuffers			|
|   HookFlipForWorkload |   sceGnmSubmitAndFlipCommandBufferForWorkload	|
|   HookFlipVideoOut	|   sceVideoOutSubmitFlip						|
+-----------------------------------------------------------------------+
*/

// We need to provide an export to force the expected stub library to be generated
__declspec (dllexport) void dummy()
{
}

extern "C" {
	__declspec(dllexport) void* _ZTINSt8ios_base7failureE = nullptr;
	__declspec(dllexport) void* _ZTVSt5ctypeIcE = nullptr;
	__declspec(dllexport) void* _ZTISt8bad_cast = nullptr;
}

void run_gui()
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

	static char buf[256] = { 0 };
	ImGui::InputText("string", buf, sizeof(buf));

	static texture tex("https://downloads.fadedhd.com/IMG/logo.png");
	if (tex)
	{
		ImGui::Image(&tex, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y });
	}

	ImGui::End();

}

void render(int flipIndex)
{
	if (app.update(flipIndex))
	{
		run_gui();

		app.render();
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
			liborbisutil::http::initialize();

			app.init(FunctionImGui | HookFlip | FunctionRenderDebug, render);

			liborbisutil::pad::initialize(liborbisutil::pad::state::read_state, true, [](ScePadData* pad, int num) {

				ImGui_ImplOrbis_Data* bd = ImGui_ImplOrbis_GetBackendData();
				if (bd)
					memcpy(&bd->m_sce_pad, pad, sizeof(ScePadData));

				});

			JailBackup backup;
			Fusion::Jailbreak(getpid(), &backup);
			
			int size = 4;
			int flag = 1;
			sysctlbyname("Fusion.FeatureFlag.DirectMemory", nullptr, 0, &flag, size);

			
			Fusion::RestoreJail(getpid(), backup);

			}, "init");

		t.join();

		return SCE_OK;
	}

	int __cdecl module_stop(size_t argc, const void* args)
	{
		liborbisutil::thread t([](void*) {

			liborbisutil::pad::finalize();
			app.cleanup();

			liborbisutil::http::finalize();

			sceKernelUsleep(500 * 1000);

			MH_Uninitialize();
			}, "release");

		t.join();

		return SCE_OK;
	}
}