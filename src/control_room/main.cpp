#include <iostream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main()
{
	using namespace ftxui;
	Screen screen = Screen::Create(Dimension::Full());
	Element elements = text(L"Hello World");
	Render(screen, elements);
	screen.Print();
	return 0;
}
