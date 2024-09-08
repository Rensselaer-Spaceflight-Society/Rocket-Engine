#include <iostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ui.hpp"

int main()
{
	std::cout << "Hello, Control Room!" << std::endl;
	// Create the interactive screen.
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	auto layout = CreateMainLayout();

	screen.Loop(layout);

	return 0;
}
