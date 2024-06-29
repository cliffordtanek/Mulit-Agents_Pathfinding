#include <iostream>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Editor.h"
#include "Utility.h"
#include "Vector2D.h"

Editor editor;

int main()
{
    Vec2 winSize = { 1600.f, 900.f };
    std::string winTitle = "sfml";
    bool isFullscreen = false;

    sf::Clock clock;
    sf::Font font;

    font.loadFromFile("../Assets/Fonts/PoorStoryRegular.ttf");

    sf::RenderWindow window(sf::VideoMode(winSize.x, winSize.y), winTitle);
    window.setFramerateLimit(60);

    // Initialize ImGui-SFML
    ImGui::SFML::Init(window);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    editor.init();

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
                }
                break;
            }
        }

        // Start the ImGui frame
        ImGui::SFML::Update(window, clock.restart());

        // update the editor
        editor.createDockspace();
        ImGui::End();
        editor.update();

        // Render ImGui into window
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    editor.free();

    return 0;
}