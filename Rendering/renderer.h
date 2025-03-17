#pragma once
#include "../common.h"
#include "../ImGui/imgui.h"

template <typename T>
using comptr = Microsoft::WRL::ComPtr<T>;

namespace big
{
	class renderer
	{
	public:
		explicit renderer();
		~renderer();

		void on_present();

		void pre_reset();
		void post_reset();

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		comptr<IDXGISwapChain> m_dxgi_swapchain;
		comptr<ID3D11Device> m_d3d_device;
		comptr<ID3D11DeviceContext> m_d3d_device_context;
	public:
		ImFont *m_font;
	private:
		void mouse_act();
	};

	inline renderer *g_renderer{};
}
