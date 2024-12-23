//==============================================================================
/*!
\file		Editor.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Definition of the Editor class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#include "Editor.h"
#include "imgui.h"
#include "Utility.h"
#include "Factory.h"
#include "Loader.h"
#include "imgui-SFML.h"

extern Editor editor;
extern Factory factory;
extern Grid grid;
extern Loader loader;
extern sf::RenderWindow window;
extern float dt;
extern bool isPaused;
extern DrawMode mode;
extern MapConfig config;
extern FovConfig fov;
extern PotentialConfig pConfig;
extern RepulsionConfig rConfig;

// local globals for constant dropdown lists
std::vector<const char *> colorNames;
std::vector<std::string> rowNums; // rows or cols
std::vector<const char *>rowNames;

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

bool Window::canWindowBeOpened()
{
	return canBeOpened;
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

	ImGui::Begin(name.c_str());

	ImGui::Text("FPS: %.2f", 1.f / dt);
	editor.addSpace(3);

	for (const auto &[name, window] : editor.getWindows())
		if (window->canWindowBeOpened() && ImGui::Button(("Toggle "s + name).c_str()))
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
	int count = 0;
	
	for (const auto &[type, entities] : factory.getAllEntities())
	{
		ImGui::SeparatorText((type + " entities").c_str());

		for (const auto &[entityPtr, entity] : entities)
		{
			if (ImGui::CollapsingHeader(("Entity " + std::to_string(++count)).c_str()))
			{
				// pos
				Vec2 mapSize = { grid.getWidth() * grid.getCellSize(), grid.getHeight() * grid.getCellSize() };
				ImGui::SliderFloat(("[" + std::to_string(count) + "] X Position").c_str(), 
					&entity->pos.x, 0.f, mapSize.x);
				ImGui::SliderFloat(("[" + std::to_string(count) + "] Y Position").c_str(),
					&entity->pos.y, 0.f, mapSize.y);

				// dir
				ImGui::SliderFloat(("[" + std::to_string(count) + "] X Direction").c_str(), 
					&entity->dir.x, -1.f, 1.f);
				ImGui::SliderFloat(("[" + std::to_string(count) + "] Y Direction").c_str(),
					&entity->dir.y, -1.f, 1.f);

				// color
				float color[4] = { entity->color.r / 256.f, entity->color.g / 256.f, entity->color.b / 256.f,
				entity->color.a / 256.f };
				ImGui::ColorEdit4(("[" + std::to_string(count) + "] Colour").c_str(), color);
				entity->color.r = (sf::Uint8)(color[0] * 256.f);
				entity->color.g = (sf::Uint8)(color[1] * 256.f);
				entity->color.b = (sf::Uint8)(color[2] * 256.f);
				entity->color.a = (sf::Uint8)(color[3] * 256.f);

				// scale
				ImGui::SliderFloat(("[" + std::to_string(count) + "] X Scale").c_str(), 
					&entity->scale.x, 0.f, mapSize.x);
				ImGui::SliderFloat(("[" + std::to_string(count) + "] Y Scale").c_str(), 
					&entity->scale.y, 0.f, mapSize.y);

				// speed
				ImGui::SliderFloat("Speed", &entity->speed, 0.f, 1000.f);

				// shape
				static int shapeIndex = entity->shape;
				const char *shapes[] = { "None", "Circle", "Triangle", "Rectangle" };
				const char *preview = shapes[shapeIndex];

				if (ImGui::BeginCombo(("[" + std::to_string(count) + "] Shape").c_str(), preview))
				{
					for (int i = 0; i < IM_ARRAYSIZE(shapes); ++i)
					{
						const bool isSelected = shapeIndex == i;
						if (ImGui::Selectable(shapes[i], isSelected))
							shapeIndex = i;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (isSelected)
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

void Inspector::onExit()
{
	Window::onExit();
}

void MapMaker::onEnter()
{
	Window::onEnter();
}

void MapMaker::onUpdate()
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);

	// tried to render sfml textures onto imgui window but failed
	//sf::Vector2u textureSize = renderer.getSize();
	//sf::Texture texture = renderer.getTexture();
	//ImVec2 size = ImVec2(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));
	//ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture.getNativeHandle())), size);

	static int mapIndex = INVALID;
	std::vector<const char *> mapNames;
	const std::unordered_map<std::string, std::vector<std::vector<bool>>> &maps = loader.getMaps();
	std::transform(maps.begin(), maps.end(), std::back_inserter(mapNames), [](const auto &elem) 
		{ return elem.first.c_str(); });

	ImGui::SeparatorText("Loading & Saving");
	editor.addSpace(2);
	mapIndex = mapNames.size() ? mapIndex : INVALID;
	const char *mapPreview = mapIndex == INVALID ? "" : mapNames[mapIndex];

	if (ImGui::BeginCombo("Select Map", mapPreview))
	{
		for (int i = 0; i < mapNames.size(); ++i)
		{
			const bool isSelected = mapIndex == i;
			if (ImGui::Selectable(mapNames[i], isSelected))
				mapIndex = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Save Map As"))
		editor.openWindow("SaveAsMapPopup");

	if (mapIndex != INVALID)
	{
		if (ImGui::Button("Save Selected Map"))
			loader.saveMap(mapNames[mapIndex]);

		if (ImGui::Button("Load Selected Map"))
			grid.changeMap(mapNames[mapIndex]);

		if (ImGui::Button("Delete Selected Map"))
		{
			loader.deleteMap(mapNames[mapIndex]);
			mapIndex = 0;
		}

		if (ImGui::Button("Rename Selected Map"))
		{
			editor.getWindow<SaveAsMapPopup>()->initMode(false, mapNames[mapIndex]);
			editor.openWindow("SaveAsMapPopup");
		}
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_ROSE);
		ImGui::Text("No map selected");
		ImGui::PopStyleColor();
	}

	editor.addSpace(5);
	ImGui::SeparatorText("Drawing");
	editor.addSpace(2);

#if 0
	static int colorIndex = INVALID;
	const char *colorPreview = colorIndex == INVALID ? "" : colorNames[colorIndex];
	int oldColorIndex = colorIndex;

	if (ImGui::BeginCombo("Pen", colorPreview))
	{
		canZoom = false;

		for (int i = 0; i < colorNames.size(); ++i)
		{
			const bool isSelected = colorIndex == i;
			if (ImGui::Selectable(colorNames[i], isSelected))
				colorIndex = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (oldColorIndex != colorIndex)
		grid.setPenColour(colorNames[colorIndex]);
#endif

	if (ImGui::Button("Clear Map"))
		grid.clearMap();
	if (ImGui::Button("Clear Fog of War"))
		grid.resetFog();
	if (ImGui::Button("Clear Goal and Entities"))
		grid.resetMap();

	int rowIndex = grid.getHeight() - 1, colIndex = grid.getWidth() - 1;
	int oldRowIndex = rowIndex, oldColIndex = colIndex;

	if (ImGui::BeginCombo("Rows", std::to_string(rowIndex + 1).c_str()))
	{
		for (int i = 0; i < rowNames.size(); ++i)
		{
			const bool isSelected = rowIndex == i;
			if (ImGui::Selectable(rowNames[i], isSelected))
				rowIndex = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Columns", std::to_string(colIndex + 1).c_str()))
	{
		for (int i = 0; i < rowNames.size(); ++i)
		{
			const bool isSelected = colIndex == i;
			if (ImGui::Selectable(rowNames[i], isSelected))
				colIndex = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (rowIndex != oldRowIndex)
		grid.setHeight(rowIndex + 1);
	if (colIndex != oldColIndex)
		grid.setWidth(colIndex + 1);

	static std::vector<char const *> modeNames{ "None", "Wall", "Goal", "Entity" };

	if (ImGui::BeginCombo("Draw Mode", modeNames[static_cast<int>(mode)]))
	{
			for (int i = 0; i < modeNames.size(); ++i)
			{
				const bool isSelected = static_cast<int>(mode) == i;
					if (ImGui::Selectable(modeNames[i], isSelected))
						mode = static_cast<DrawMode>(i);

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (isSelected)
							ImGui::SetItemDefaultFocus();
			}

		ImGui::EndCombo();
	}

	ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_GREEN);

	switch (mode)
	{
	case DrawMode::WALL:
		ImGui::Text("Left click to draw wall");
		ImGui::Text("Right click to erase wall");
		break;

	case DrawMode::ENTITY:
		ImGui::Text("Left click to spawn Enemy");
		ImGui::Text("Right click to despawn Enemy");
		break;

	case DrawMode::GOAL:
		ImGui::Text("Left click to start exploration");
		ImGui::Text("Right click to set goal");
		break;
	
	case DrawMode::NONE:
		ImGui::Text("Walls are hidden until discovered");
		break;

	default:
		break;
	}

	ImGui::PopStyleColor();
	editor.addSpace(5);
	ImGui::SeparatorText("Procedural Generation");
	editor.addSpace(2);

	if (config.wallSize > config.tunnelSize)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_ROSE);
		ImGui::Text("For optimal generation, wall + tunnel width");
		ImGui::Text("should be smaller than or equal to tunnel width");
		ImGui::PopStyleColor();
	}

	ImGui::SliderInt("Wall Width", &config.wallSize, 1, 10);
	ImGui::SliderInt("Wall + Tunnel Width", &config.tunnelSize, 1, 10);
	ImGui::SliderInt("Min Island Size", &config.minIslandSize, 1, 100);
	ImGui::SliderInt("Noise", &config.noise, 0, 10);
	ImGui::SliderInt("Min Connections", &config.minConnections, 1, 4);
	ImGui::SliderInt("Max Connections", &config.maxConnections, 1, 4);
	ImGui::Checkbox("Equal Wall Width", &config.isEqualWidth);

	if (config.minConnections > config.maxConnections)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_ROSE);
		ImGui::Text("Mininum connections must be smaller");
		ImGui::Text("than maximum connections");
		ImGui::PopStyleColor();
	}

	if (ImGui::Button("Generate Map"))
		grid.generateMap();

	ImGui::End();
}

void MapMaker::onExit()
{
	Window::onExit();
}

void SaveAsMapPopup::onEnter()
{
	Window::onEnter();
}

void SaveAsMapPopup::onUpdate()
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);

	static char buffer[SMALL];
	ImGui::InputText("Map Name", buffer, SMALL, ImGuiInputTextFlags_CallbackCharFilter, utl::filterAlphanum);

	if (loader.doesMapExist(buffer))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_ROSE);
		ImGui::Text("File already exists");
		ImGui::PopStyleColor();
	}
	else if (ImGui::Button("Save##SAMP"))
	{
		if (isSaveAsMode)
			loader.saveMap(buffer);
		else
		{
			crashIf(!oldName, "Original name of map to be renamed has not been initialised");
			loader.renameMap(oldName, buffer); // knn see above line leh i check liao sia
		}

		editor.closeWindow("SaveAsMapPopup");
	}

	ImGui::End();
}

void SaveAsMapPopup::onExit()
{
	Window::onExit();
}

void SaveAsMapPopup::initMode(bool _isSaveAsMode, const char *_oldName)
{
	isSaveAsMode = _isSaveAsMode;
	oldName = _oldName;
}

void ControlPanel::onEnter()
{
	Window::onEnter();
}

void ControlPanel::onUpdate()
{
	if (!isOpen)
		return;
	Window::onUpdate();

	ImGui::Begin(name.c_str(), &isOpen);

	ImGui::SeparatorText("Game");
	editor.addSpace(2);

	if (ImGui::Button("Toggle Pause"))
		isPaused = !isPaused;
	if (isPaused)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_ROSE);
		ImGui::Text("Game is paused");
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LIGHT_GREEN);
		ImGui::Text("Game is running");
	}
	ImGui::PopStyleColor();

	editor.addSpace(5);
	ImGui::SeparatorText("Flow Field");
	editor.addSpace(2);

	ImGui::Checkbox("Draw Vision Radius", &grid.debugDrawRadius);
	ImGui::Checkbox("Draw Heat Map", &grid.showHeatMap);
	ImGui::Checkbox("Draw Flow Field", &grid.flowFieldArrow);

	editor.addSpace(5);
	ImGui::SeparatorText("Fog of War");
	editor.addSpace(2);

	ImGui::SliderFloat("Cone Radius", &fov.coneRadius, 0.f, 1000.f);
	ImGui::SliderFloat("Cone Angle", &fov.coneAngle, 0.f, 360.f);
	ImGui::SliderFloat("Circle Radius", &fov.circleRadius, 0.f, 1000.f);

	editor.addSpace(5);
	ImGui::SeparatorText("Repulsion Field");
	editor.addSpace(2);

	ImGui::SliderFloat("Repulsion Radius", &rConfig.radius, 0.f, 1000.f);
	ImGui::Checkbox("Use Repulsion Field", &rConfig.useRepulsionMap);
	ImGui::Checkbox("Draw Repulsion Field (must check above box)", &rConfig.showRepulsionMap);

	editor.addSpace(5);
	ImGui::SeparatorText("Potential Field");
	editor.addSpace(2);

	ImGui::SliderFloat("Potential Weight", &pConfig.potentialWeight, 0.f, 50.f);
	ImGui::SliderFloat("Max Manhattan Distance", &pConfig.maxMd, 0.f, 50.f);
	ImGui::SliderFloat("Max Potential", &pConfig.maxPotential, 0.1f, 1.f);
	ImGui::SliderFloat("Min Unknown Percentage", &pConfig.minUnknownPercent, 0.f, 1.f);
	ImGui::SliderInt("Block Size", &pConfig.blockSize, 1, 10);
	ImGui::Checkbox("Use Potential Field", &pConfig.usePotentialField);
	ImGui::Checkbox("Draw Potential Field (must check above box)", &pConfig.showPotentialField);
	ImGui::Checkbox("Draw Final Field", &pConfig.showFinalMap);

	ImGui::End();
}

void ControlPanel::onExit()
{
	Window::onExit();
}

void Editor::init()
{
	// Initialize ImGui-SFML
	ImGui::SFML::Init(window);

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
	io.WantCaptureMouse = true;
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("../Assets/Fonts/PoorStoryRegular.ttf", 24.f);
	ImGui::SFML::UpdateFontTexture();

	addWindow<MainMenu>(true, false);
	addWindow<Inspector>(false, true);
	addWindow<MapMaker>(true, true);
	addWindow<SaveAsMapPopup>(false, false);
	addWindow<ControlPanel>(true, true);

	std::transform(colors.begin(), colors.end(), std::back_inserter(colorNames), [](const auto &elem)
		{ return elem.first.c_str(); });
	std::generate_n(std::back_inserter(rowNums), 100, [n = 0]() mutable { return std::to_string(++n); });
	std::transform(rowNums.begin(), rowNums.end(), std::back_inserter(rowNames),
		[](const auto &elem) { return elem.c_str(); });
	auto entities = factory.getAllEntities();
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