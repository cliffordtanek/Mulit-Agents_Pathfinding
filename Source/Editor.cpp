#include "Editor.h"
#include "imgui.h"
#include "Utility.h"

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
	addWindow<MainMenu>();
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
	crashIf(!windows.count(windowName), "Could not find window with name " + utl::quote(windowName));
	windows.at(windowName)->open();
}

void Editor::closeWindow(const std::string &windowName)
{
	crashIf(!windows.count(windowName), "Could not find window with name " + utl::quote(windowName));
	windows.at(windowName)->close();
}

void Editor::createDockspace()
{
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	ImGui::Begin("DockSpace Demo", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
	ImGui::PopStyleVar(3);
	ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.f, 0.f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}