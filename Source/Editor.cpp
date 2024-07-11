#include "Editor.h"
#include "imgui.h"
#include "Utility.h"
#include "Factory.h"
#include "imgui-SFML.h"

extern Editor editor;
extern Factory factory;
extern Grid grid;
extern sf::RenderWindow window;
//extern sf::RenderTexture renderer;
extern float dt;

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

bool Window::isWindowOpen()
{
	return isOpen;
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

	ImGui::Text("FPS: %.2f", 1.f / dt);
	editor.addSpace(3);

	for (const auto &[name, window] : editor.getWindows())
		if (name != "MainMenu" && ImGui::Button(("Toggle "s + name).c_str()))
			editor.toggleWindow(name);

	ImGui::End();
}

void MainMenu::onExit()
{
	Window::onExit();
}

void Inspector::onEnter() 
{
	Window::onEnter();
}

void Inspector::onUpdate() 
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);
	
	for (const auto &[type, entities] : factory.getAllEntities())
	{
		ImGui::SeparatorText((type + " entities").c_str());

		for (const auto &[entityPtr, entity] : entities)
		{
			if (ImGui::CollapsingHeader(utl::ptrToStr(entity).c_str()))
			{
				// pos
				Vec2 mapSize = { grid.getWidth() * grid.getCellSize(), grid.getHeight() * grid.getCellSize() };
				ImGui::SliderFloat("X Position", &entity->pos.x, 0.f, mapSize.x);
				ImGui::SliderFloat("Y Position", &entity->pos.y, 0.f, mapSize.y);

				// dir
				ImGui::SliderFloat("X Direction", &entity->dir.x, -1.f, 1.f);
				ImGui::SliderFloat("Y Direction", &entity->dir.y, -1.f, 1.f);

				// color
				float color[4] = { entity->color.r / 256.f, entity->color.g / 256.f, entity->color.b / 256.f,
				entity->color.a / 256.f };
				ImGui::ColorEdit4("Color", color);
				entity->color.r = (sf::Uint8)(color[0] * 256.f);
				entity->color.g = (sf::Uint8)(color[1] * 256.f);
				entity->color.b = (sf::Uint8)(color[2] * 256.f);
				entity->color.a = (sf::Uint8)(color[3] * 256.f);

				// scale
				ImGui::SliderFloat("X Scale", &entity->scale.x, 0.f, mapSize.x);
				ImGui::SliderFloat("Y Scale", &entity->scale.y, 0.f, mapSize.y);

				// speed
				ImGui::SliderFloat("Speed", &entity->speed, 0.f, 1000.f);

				// shape
				static int shapeIndex = entity->shape;
				const char *shapes[] = { "None", "Circle", "Triangle", "Rectangle" };
				const char *preview = shapes[shapeIndex];

				if (ImGui::BeginCombo("Shape", preview))
				{
					for (int i = 0; i < IM_ARRAYSIZE(shapes); ++i)
					{
						const bool is_selected = shapeIndex == i;
						if (ImGui::Selectable(shapes[i], is_selected))
							shapeIndex = i;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				entity->shape = static_cast<Shape>(shapeIndex);
			}
		}

		editor.addSpace(3);
	}

	ImGui::End();
}

void Game::onEnter()
{
	Window::onEnter();
}

void Game::onUpdate()
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);

	//sf::Vector2u textureSize = renderer.getSize();
	//sf::Texture texture = renderer.getTexture();
	//ImVec2 size = ImVec2(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
	//ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture.getNativeHandle())), size);

	ImGui::End();
}

void Game::onExit()
{
	Window::onExit();
}

void Inspector::onExit() 
{
	Window::onExit();
}

void Editor::init()
{
	// Initialize ImGui-SFML
	ImGui::SFML::Init(window);

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("../Assets/Fonts/PoorStoryRegular.ttf", 24.f);
	ImGui::SFML::UpdateFontTexture();

	addWindow<MainMenu>();
	addWindow<Inspector>();
	addWindow<Game>();
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

void Editor::toggleWindow(const std::string &windowName)
{
	crashIf(!windows.count(windowName), "Could not find window with name " + utl::quote(windowName));
	if (windows.at(windowName)->isWindowOpen())
		windows.at(windowName)->close();
	else
		windows.at(windowName)->open();
}

void Editor::createDockspace()
{
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// Set window background color to be transparent
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

	ImGui::Begin("DockSpace Demo", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor();  // Pop the color style

	ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
}

const std::unordered_map<std::string, Window *> &Editor::getWindows()
{
	return windows;
}

void Editor::addSpace(int spaceCount)
{
	for (int i = 0; i < spaceCount; ++i)
		ImGui::Spacing();
}