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
std::string winTitle = "sfml";
bool isFullscreen = false;
float dt = 0.f;

sf::RenderWindow window(sf::VideoMode(winSize.x, winSize.y), winTitle);
sf::Font font;

Editor editor;
Factory factory;
Grid grid(75, 75, 50.f);
Loader loader;
Camera camera;

//! temp
bool isMousePressed{ false };
bool canExit = false;

int main()
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    sf::Clock clock;

    font.loadFromFile("../Assets/Fonts/PoorStoryRegular.ttf");
    window.setFramerateLimit(60);

    // initialize systems
    editor.init();
    factory.init();
    Enemy *enemy = factory.createEntity<Enemy>(Vec2{ 50.f, 50.f }, Vec2{ 50.f, 50.f });
    std::list<Vec2> waypoints
    { { 100.f, 125.f }, { 325.f, 250.f }, { 500.f, 575.f }, { 775.f, 375.f }, { 800.f, 600.f } };

    while (window.isOpen() && !canExit)
    {
        dt = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event))
        {
            // Pass events to ImGui
            ImGui::SFML::ProcessEvent(event);

            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::F11:
                    window.close();
                    if (isFullscreen)
                        window.create(sf::VideoMode(winSize.x, winSize.y), winTitle);
                    else
                        window.create(sf::VideoMode::getDesktopMode(), winTitle, sf::Style::Fullscreen);
                    isFullscreen = !isFullscreen;
                    break;

                case sf::Keyboard::T:
                    std::cout << "dt: " << dt << nl;
                    break;

                case sf::Keyboard::M:
                    for (Enemy *enemy : factory.getEntities<Enemy>())
                        factory.destroyEntity<Enemy>(enemy);
                    break;

                case sf::Keyboard::P:
                    if (ALIVE(Enemy, enemy))
                        enemy->setWaypoints(waypoints);
                    break;

                case sf::Keyboard::N:
                    enemy = factory.createEntity<Enemy>(Vec2{ 200.f, 200.f }, Vec2{ 50.f, 100.f });
                    break;

                case sf::Keyboard::D:
                    factory.grid->debugDrawRadius = !factory.grid->debugDrawRadius;

                }
                break;
                case sf::Keyboard::Num1:
                    loader.saveMap("test");
                    break;

                case sf::Keyboard::Num2:
                    grid.changeMap("test");
                    break;

                case sf::Keyboard::Num3:
                    grid.clearMap();
                    break;
                }
                break;

            }

        // mouse event must put outside of switch case for some reason
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && ALIVE(Enemy, enemy))
        {
            isMousePressed = true;
           
            Vec2 target = { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };

            if (!factory.grid->isWall(factory.grid->getGridPos(target)));
            {
                factory.grid->updateHeatMap(target);
                factory.grid->generateFlowField();

                enemy->setTargetPos(target, true);
            }

            // factory.grid->computePath(*enemy, target);
        }

        if (event.type == sf::Event::MouseMoved && isMousePressed)
        {
            // calculate grid coordinates from mouse position
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
            Grid::GridPos pos = factory.grid->getGridPos((float)mousePos.x, (float)mousePos.y);

            // set colour of grid upon click
            factory.grid->SetColour(pos.row, pos.col, sf::Color::Green);
        }

        // FOG TEST (Mouse cursor)
#if 1
        if (event.type == sf::Event::MouseMoved)
        {
           // factory.grid->updateHeatMap({ static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) });
        }
#endif


            if (event.type == sf::Event::MouseButtonReleased)
                isMousePressed = false;

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camera.move({ 0.f, -1.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camera.move({ -1.f, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camera.move({ 0.f, 1.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camera.move({ 1.f, 0.f });
        camera.calcOffset();

        // Start the ImGui frame
        ImGui::SFML::Update(window, clock.restart());

        // update the editor
        editor.createDockspace();
        editor.update();

        // Render ImGui into window
        window.clear();
        ImGui::SFML::Render(window);

        // update other systems
        factory.update();

        window.display();
        camera.update();
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