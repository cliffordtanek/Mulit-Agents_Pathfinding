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

Vec2 winSize = { 1600.f, 900.f };
std::string winTitle = "sfml";
bool isFullscreen = false;

sf::RenderWindow window(sf::VideoMode(winSize.x, winSize.y), winTitle);

Editor editor;
Factory factory;

//! temp
bool isMousePressed{ false };


int main()
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    sf::Clock clock;
    sf::Font font;

    font.loadFromFile("../Assets/Fonts/PoorStoryRegular.ttf");
    window.setFramerateLimit(60);

    // Initialize ImGui-SFML
    ImGui::SFML::Init(window);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    editor.init();

    // initialize other systems
    factory.init();
    Enemy *enemy = factory.createEntity<Enemy>(Vec2{ 200.f, 200.f }, Vec2{ 50.f, 100.f });
    std::list<Vec2> waypoints
    { { 100.f, 125.f }, { 325.f, 250.f }, { 500.f, 575.f }, { 775.f, 375.f }, { 800.f, 600.f } };

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
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

                }
                break;

            case sf::Event::MouseMoved:
                if (isMousePressed)
                {
                    // calculate grid coordinates from mouse position
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int col = mousePos.x / factory.grid->getCellSize();
                    int row = mousePos.y / factory.grid->getCellSize();

                    // set colour of grid upon click
                    factory.grid->SetColour(row, col, sf::Color::Green);
                }
                break;

            }

        }

        // mouse event must put outside of switch case for some reason
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left &&
            ALIVE(Enemy, enemy))
            enemy->setTargetPos({ static_cast<float>(event.mouseButton.x), 
                static_cast<float>(event.mouseButton.y) }, true);

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