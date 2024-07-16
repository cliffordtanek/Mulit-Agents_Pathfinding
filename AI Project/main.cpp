//==============================================================================
/*!
\file		main.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		main loop

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#include <iostream>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Editor.h"
#include "Utility.h"
#include "Vector2D.h"
#include "Factory.h"
#include "Loader.h"
#include "Camera.h"
#include "Grid.h"

Vec2 winSize = { 1600.f, 900.f };
float ratio = winSize.x / winSize.y;
Vec2 minimapOffset;
Vec2 mapSize;
std::string winTitle = "sfml";
bool isFullscreen = false;
bool canZoom = true; // disable zooming when in dropdown menus
bool isPaused = false;
float dt = 0.f;
DrawMode mode = DrawMode::WALL;
extern RepulsionConfig rConfig;
extern PotentialConfig pConfig;

sf::RenderWindow window(sf::VideoMode((unsigned int)winSize.x, (unsigned int)winSize.y), winTitle, sf::Style::Titlebar | sf::Style::Close);
sf::Font font;
sf::View view(winSize / 2.f, winSize);
sf::View minimap(mapSize / 2.f, mapSize);

float cellSize = 100.f;
Editor editor;
Factory factory;
Grid grid(25, 50, cellSize); // this is height x width not width x height omg
Loader loader;
Camera camera;

//! temp
bool isLMousePressed{ false }, isRMousePressed{ false };
bool canExit = false;

Vec2 target{};

int main()
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    srand((unsigned)time(0));
    sf::Clock clock;

    font.loadFromFile("../Assets/Fonts/PoorStoryRegular.ttf");
    window.setFramerateLimit(60);
    minimap.setViewport(sf::FloatRect(0.75f, 0.0208f, 0.25f, 0.25f));

    // initialize systems
    factory.init();
    editor.init();
    //Enemy *enemy = factory.createEntity<Enemy>(Vec2{ 0.f, 0.f }, Vec2{ 30.f, 50.f });
    std::list<Vec2> waypoints
    { { 100.f, 125.f }, { 325.f, 250.f }, { 500.f, 575.f }, { 775.f, 375.f }, { 800.f, 600.f } };

    while (window.isOpen() && !canExit)
    {
        dt = clock.restart().asSeconds();
        sf::Event event;

        // if exit found, path to exit
        if (grid.isExitFound())
        {
            grid.updateHeatMap(grid.getWorldPos(grid.exitCell->pos));

            grid.CombineMaps();
            grid.generateFlowField();

            // set all enemy to the target
            for (Enemy* enemy : factory.getEntities<Enemy>())
                enemy->setTargetPos(grid.getWorldPos(grid.exitCell->pos), true);
        }
        else
        {
            grid.updateHeatMap();

            if (rConfig.useRepulsionMap)
            {
                // for walls
                //grid.updateRepulsionMap(rConfig.radius, 1.f);

                for (Enemy* enemy : factory.getEntities<Enemy>())
                    grid.updateRepulsionMap(grid.getGridPos(enemy->pos), rConfig.radius, 1.f);
            }




            if (pConfig.usePotentialField)
            {
                grid.updatePotentialMap();
            }

            grid.CombineMaps();
            grid.generateFlowField();
        }    

        while (window.pollEvent(event))
        {
            // Pass events to ImGui
            ImGui::SFML::ProcessEvent(event);

            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    if (event.mouseWheelScroll.delta > 0.f)
                        view.zoom(1.f - CAM_ZOOM);
                    else
                        view.zoom(1.f + CAM_ZOOM);
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::F11:
                    window.close();
                    if (isFullscreen)
                        window.create(sf::VideoMode((unsigned int)winSize.x, (unsigned int)winSize.y), winTitle, 
                            sf::Style::Titlebar | sf::Style::Close);
                    else
                        window.create(sf::VideoMode::getDesktopMode(), winTitle, sf::Style::Fullscreen);
                    isFullscreen = !isFullscreen;
                    break;

                    break;
                }
                break;
            }

        // mouse event must put outside of switch case for some reason
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                isLMousePressed = true;
                Vec2 target = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                grid.setIntensity(grid.getGridPos(target));
                if (mode == DrawMode::ENTITY)
                    factory.cloneEnemyAt(target);

                // move enemy to cell
                if (!grid.isWall(grid.getGridPos(target)) && mode == DrawMode::GOAL)
                {
                    // set all enemy to the target
                    for (Enemy *enemy : factory.getEntities<Enemy>())
                        enemy->setTargetPos(target, true);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                isRMousePressed = true;
                Vec2 target = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                grid.setIntensity(grid.getGridPos(target));

                if (mode == DrawMode::ENTITY)
                    for (Enemy *enemy : factory.getEntities<Enemy>())
                        if (grid.getGridPos(target) == grid.getGridPos(enemy->pos))
                            factory.destroyEntity<Enemy>(enemy);

                if (!grid.isWall(grid.getGridPos(target)) && mode == DrawMode::GOAL)
                    grid.setExit(grid.getGridPos(target));
            }

            // mouse event must put outside of switch case for some reason
            if (event.type == sf::Event::MouseMoved && isRMousePressed)
            {
                Vec2 target = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // erase wall
                if (mode == DrawMode::WALL)
                    grid.setWall(grid.getGridPos(target), false);
            }

            if (event.type == sf::Event::MouseMoved && isLMousePressed)
            {
                // calculate grid coordinates from mouse position
                Vec2 target = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
                // insert wall
                if (mode == DrawMode::WALL)
                    grid.setWall(grid.getGridPos(target), true);

            }

            // FOG TEST (Mouse cursor)
    #if 1
            if (event.type == sf::Event::MouseMoved)
            {
               // grid.updateHeatMap({ static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) });
            }
    #endif

            if (event.type == sf::Event::MouseButtonReleased)
            {
                isLMousePressed = isRMousePressed = false;
            }

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            view.move({ 0.f, -1.f * CAM_MOVE });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            view.move({ -1.f * CAM_MOVE, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            view.move({ 0.f, 1.f * CAM_MOVE });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            view.move({ 1.f * CAM_MOVE, 0.f });

        // Start the ImGui frame
        ImGui::SFML::Update(window, clock.restart());
        window.clear(colors.at("Background").first);
        editor.createDockspace();

        // minimap variables
        int width = grid.getWidth();
        int height = grid.getHeight();
        bool isWidthLonger = width * 1.f > height * ratio; // height and width are flipped in grid object
        float gridLength = std::max(height * ratio, width * 1.f) * cellSize;

        // init minimap
        mapSize = { gridLength, gridLength / ratio };
        minimapOffset = { (mapSize.x - width * cellSize) / 2.f, (mapSize.y - height * cellSize) / 2.f };
        minimap.setCenter(mapSize / 2.f);
        minimap.setSize(mapSize);

        // update other systems
        window.setView(view);
        editor.update();
        factory.update();

        // draw minimap
        window.setView(minimap);
        sf::RectangleShape rectangle;
        float stroke = 20.f;

        // draw blue background
        rectangle.setSize(mapSize);
        rectangle.setFillColor(colors.at("Background").first);
        window.draw(rectangle);
        window.setView(view);
        camera.flushDrawQueue();

        // draw white border
        window.setView(minimap);
        rectangle.setSize({ mapSize.x - stroke * 2.f, mapSize.y - stroke * 2.f });
        rectangle.setPosition({ stroke, stroke });
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineThickness(stroke);
        rectangle.setOutlineColor(sf::Color::White);
        window.draw(rectangle);

        // draw camera highlight
        rectangle.setSize(view.getSize());
        rectangle.setPosition(view.getCenter() - winSize / 2.f + minimapOffset + 
            (winSize - view.getSize()) / 2.f);
        rectangle.setFillColor(colors.at("Translucent").first);
        rectangle.setOutlineThickness(0.f);
        window.draw(rectangle);
        window.setView(view);

        // render imgui onto the window
        ImGui::SFML::Render(window);
        window.display();

        // update other systems
    }

    // free systems
    editor.free();
    factory.free();

    // Cleanup ImGui-SFML resources
    ImGui::SFML::Shutdown();

    // free resources
    window.close();

    return 0;
    
}