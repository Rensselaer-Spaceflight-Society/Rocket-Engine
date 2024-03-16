#include <iostream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main()
{
	ftxui::Screen screen = ftxui::Screen::Create(ftxui::Dimension::Full());
	ftxui::Element elements = ftxui::text(L"Hello World");
	Render(screen, elements);
	screen.Print();
	return 0;
}
