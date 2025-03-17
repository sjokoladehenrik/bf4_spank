#include "renderer.h"
#include "gui.h"
#include "font.h"
#include "../common.h"
#include "../global.h"
#include "../SDK/sdk.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_internal.h"
#include "../Utilities/path.h"
#include "../Utilities/other.h"
#include "../Features/main.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer::renderer() :
		m_dxgi_swapchain(get_swapchain())
	{
		void *d3d_device{};
		if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
		{
			m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
		}
		else
		{
			throw std::runtime_error(xorstr_("Failed to get D3D device."));
		}

		m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());

		auto file_path = get_appdata_folder();
		file_path /= xorstr_("imgui.ini");

		ImGuiContext* context = ImGui::CreateContext();
		context->IO.DeltaTime = 1.0f / 60.0f;

		static std::string path = file_path.make_preferred().string();
		context->IO.IniFilename = path.c_str();

		ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
		ImGui_ImplWin32_Init(g_globals.g_hwnd);

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;
		std::strcpy(font_cfg.Name, xorstr_("Open Sans"));

		m_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
			const_cast<std::uint8_t*>(font_main),
			sizeof(font_main),
			MAIN_FONT_SIZE, // See font.h to edit
			&font_cfg);

		context->IO.FontDefault = m_font;
		
		g_gui.dx_init();
		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_renderer = nullptr;
	}

	void renderer::mouse_act()
	{
		const auto input = BorderInputNode::GetInstance();

		if (IsValidPtr(input) && IsValidPtr(input->m_pMouse) && IsValidPtr(input->m_pMouse->m_pDevice))
		{
			if (!input->m_pMouse->m_pDevice->m_CursorMode)
			{
				// Additional fixes for mouse
				input->m_pMouse->m_pDevice->m_UIOwnsInput = g_gui.m_opened;
				input->m_pMouse->m_pDevice->m_UseRawMouseInput = g_gui.m_opened;
				ImGui::GetIO().MouseDrawCursor = g_gui.m_opened;
			}
		}	
	}

	void renderer::on_present()
	{
		// Renewed
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = g_gui.m_opened;
		io.ConfigFlags = g_gui.m_opened ? (io.ConfigFlags & ~ImGuiConfigFlags_NoMouse) : (io.ConfigFlags | ImGuiConfigFlags_NoMouse);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_features->draw();

		mouse_act();
		if (g_gui.m_opened)
			g_gui.dx_on_tick();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		// Update swapchain reference
		m_dxgi_swapchain = get_swapchain();

		// Get the device again if needed
		if (!m_d3d_device && m_dxgi_swapchain)
		{
			void* d3d_device{};
			if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
			{
				m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
				m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());
			}
		}

		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == VK_INSERT)
		{
			// Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui.m_opened)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			g_gui.m_opened ^= true;
		}
			
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}
}
