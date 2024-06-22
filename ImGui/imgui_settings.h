#include "imgui.h"

namespace c {

	inline ImVec4 accent = ImColor(105, 163, 255, 255);

	namespace background {

		inline ImVec4 bg = ImColor(12, 10, 9, 240);

		inline ImVec2 size = ImVec2(950, 750);
		inline float rounding = 16.f;
	}

	namespace child {

		inline ImVec4 bg = ImColor(41, 37, 36, 75);
		inline ImVec4 border = ImColor(87, 83, 78, 65);
		inline ImVec4 border_text = ImColor(255, 255, 255, 255);


		inline float rounding = 8.f;
	}

	namespace tabs {

		inline ImVec4 bg_active = ImColor(87, 83, 78, 75);
		inline ImVec4 bg_hov = ImColor(87, 83, 78, 45);
		inline ImVec4 bg = ImColor(87, 83, 78, 25);

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 45);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 25);

		inline float rounding = 4.f;

	}

	namespace checkbox {
		inline ImVec4 checkmark_active = ImColor(255, 255, 255, 255);
		inline ImVec4 checkmark_inactive = ImColor(87, 83, 78, 25);


		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 2.f;
	}

	namespace slider {

		inline ImVec4 circle = ImColor(255, 255, 255, 255);

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 30.f;
	}

	namespace input_text {

		inline ImVec4 i_bg_selected = ImColor(87, 83, 78, 75);

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 4.f;
	}

	namespace keybind {
		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 3.f;
	}

	namespace combo {

		inline ImVec4 i_bg_selected = ImColor(27, 23, 18, 230);

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 4.f;
	}

	namespace selectable {

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 4.f;
	}

	namespace scroll {

		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 30.f;
	}

	namespace picker {
		inline ImVec4 i_bg = ImColor(12, 10, 9, 255);

		inline float rounding = 4.f;
	}

	namespace button {
		inline ImVec4 i_bg_hov = ImColor(87, 83, 78, 75);
		inline ImVec4 i_bg = ImColor(87, 83, 78, 45);

		inline float rounding = 4.f;

	}

	namespace text {
		inline ImVec4 text_active = ImColor(255, 255, 255, 255);
		inline ImVec4 text_hov = ImColor(255, 255, 255, 150);
		inline ImVec4 text = ImColor(255, 255, 255, 100);
	}

}
