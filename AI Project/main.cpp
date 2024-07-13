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
float dt = 0.f;

sf::RenderWindow window(sf::VideoMode((unsigned int)winSize.x, (unsigned int)winSize.y), winTitle, sf::Style::Titlebar | sf::Style::Close);
//sf::RenderTexture renderer;
sf::Font font;
sf::View view(winSize / 2.f, winSize);
sf::View minimap(mapSize / 2.f, mapSize);

int cellSize = 100;
Editor editor;
Factory factory;
Grid grid(50, 25, cellSize); // this is height x width not width x height omg
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
    minimap.setViewport(sf::FloatRect(0.75f, 0.0208f, 0.25f, 0.25f));

    // initialize systems
    editor.init();
    factory.init();
    Enemy *enemy = factory.createEntity<Enemy>(Vec2{ 0.f, 0.f }, Vec2{ 50.f, 50.f });
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

            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && canZoom)
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

                case sf::Keyboard::M:
                    for (Enemy *enemy : factory.getEntities<Enemy>())
                        factory.destroyEntity<Enemy>(enemy);
                    break;

                case sf::Keyboard::P:
                    if (ALIVE(Enemy, enemy))
                        enemy->setWaypoints(waypoints);
                    break;

                case sf::Keyboard::N:
                    enemy = factory.createEntity<Enemy>(Vec2{ 200.f, 200.f }, Vec2{ 50.f, 50.f });
                    break;

                case sf::Keyboard::L:
                    grid.debugDrawRadius = !grid.debugDrawRadius;
                    break;

                case sf::Keyboard::K:
                    grid.showHeatMap = !grid.showHeatMap;
                    break;

                case sf::Keyboard::J:
                    grid.flowFieldArrow = !grid.flowFieldArrow;
                    break;

                }
                break;
            }

        // mouse event must put outside of switch case for some reason
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && ALIVE(Enemy, enemy))
        {
            Vec2 target = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

            if (!grid.isWall(grid.getGridPos(target)))
            {
                grid.updateHeatMap(target);
                grid.generateFlowField();

                // set all enemy to the target
                for (Enemy* enemy : factory.getEntities<Enemy>())
                    enemy->setTargetPos(target, true);
            }

        }

        // mouse event must put outside of switch case for some reason
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && ALIVE(Enemy, enemy))
            isMousePressed = true;

        if (event.type == sf::Event::MouseMoved && isMousePressed)
        {
            // calculate grid coordinates from mouse position
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            
            Grid::GridPos pos = grid.getGridPos(mousePos);

            // set colour of grid upon click
            grid.SetColour(pos.row, pos.col);
        }

        // FOG TEST (Mouse cursor)
#if 1
        if (event.type == sf::Event::MouseMoved)
        {
           // grid.updateHeatMap({ static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) });
        }
#endif


            if (event.type == sf::Event::MouseButtonReleased)
                isMousePressed = false;

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            view.move({ 0.f, -1.f * CAM_MOVE });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            view.move({ -1.f * CAM_MOVE, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            view.move({ 0.f, 1.f * CAM_MOVE });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            view.move({ 1.f * CAM_MOVE, 0.f });
        //camera.calcOffset();

#if 0
        // Start the ImGui frame
        ImGui::SFML::Update(window, clock.restart());
        //renderer.clear();

        // update systems
        factory.update();

        // test
        //Vec2 scale{ 50.f, 50.f };
        //Vec2 newPos{ 100.f, 100.f };
        //sf::Color color = sf::Color::Red;
        //sf::CircleShape circle;
        //circle.setRadius(scale.x / 2.f);
        //circle.setPosition(newPos - Vec2{ scale.x / 2.f, scale.x / 2.f });
        //circle.setFillColor(color);
        //renderer.draw(circle);

        // update the editor
        //renderer.display();
        editor.update();
        editor.createDockspace();

        // Render ImGui into window
        window.clear();
        ImGui::SFML::Render(window);
        window.display();

        // update other systems
        camera.update();
#endif

        // Start the ImGui frame
        ImGui::SFML::Update(window, clock.restart());
        window.clear(colors.at("Background"));
        editor.createDockspace();

        int width = grid.getHeight();
        int height = grid.getWidth();
        bool isWidthLonger = width > height; // height and width are flipped in grid object
        float gridLength = std::max(height, width) * cellSize;

        mapSize = { isWidthLonger ? gridLength : gridLength * ratio, 
            isWidthLonger ? gridLength / ratio : gridLength };
        minimapOffset = { (mapSize.x - width * cellSize) / 2.f, (mapSize.y - height * cellSize) / 2.f };
        minimap.setCenter(mapSize / 2.f);
        minimap.setSize(mapSize);

        // update other systems
        canZoom = true;
        window.setView(view);
        editor.update();
        factory.update();

        window.setView(minimap);
        sf::RectangleShape rectangle;
        float stroke = 25.f;

        rectangle.setSize(mapSize);
        rectangle.setFillColor(colors.at("Background"));
        window.draw(rectangle);
        window.setView(view);

        camera.flushDrawQueue();

        window.setView(minimap);
        rectangle.setSize({ mapSize.x - stroke * 2.f, mapSize.y - stroke * 2.f });
        rectangle.setPosition({ stroke, stroke });
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineThickness(stroke);
        rectangle.setOutlineColor(sf::Color::White);
        window.draw(rectangle);

        rectangle.setSize(view.getSize());
        rectangle.setPosition(view.getCenter() - winSize / 2.f + minimapOffset + 
            (winSize - view.getSize()) / 2.f);
        rectangle.setFillColor(colors.at("Translucent"));
        rectangle.setOutlineThickness(0.f);
        window.draw(rectangle);
        window.setView(view);

        // render imgui onto the window
        ImGui::SFML::Render(window);
        window.display();

        // update other systems
        //camera.update();
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