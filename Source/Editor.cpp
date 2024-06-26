#include "Editor.h"
#include "imgui.h"

Window::~Window()
{

}

void Window::onEnter()
{
	// add general code for all windows
}

void Window::onUpdate()
{
	if (!isOpen)
		return;

	// add general code for all windows
}

void Window::onExit()
{
	// add general code for all windows
}

void Window::open()
{
	isOpen = true;
	onEnter();
}

void Window::close()
{
	isOpen = false;
	onExit();
}

void MainMenu::onEnter()
{
	Window::onEnter();
}

void MainMenu::onUpdate()
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);
	ImGui::Text("This is some useful text.");
	ImGui::End();
}

void MainMenu::onExit()
{
	Window::onExit();
}

void Editor::init()
{

}

void Editor::update()
{
	for (auto &[key, value] : windows)
		value->onUpdate();
}

void Editor::free()
{
	for (auto &[key, value] : windows)
		delete value;
}

void Editor::openWindow(const std::string &windowName)
{
	if (windows.count(windowName))
		windows.at(windowName)->open();
}

void Editor::closeWindow(const std::string &windowName)
{
	if (windows.count(windowName))
		windows.at(windowName)->close();
}