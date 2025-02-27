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

void render(int flipIndex)
{
	if(app.update(flipIndex))
		app.render(flipIndex);
}

extern "C"
{
	int __cdecl module_start(size_t argc, const void* args)
	{
		liborbisutil::thread t([]() {
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
		liborbisutil::thread t([]() {

			liborbisutil::pad::finalize();
			app.cleanup();

			sceKernelUsleep(500 * 1000);

			MH_Uninitialize();
			}, "release");

		t.join();

		return SCE_OK;
	}
}