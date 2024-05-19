#include <iostream>
#include <SFML/Graphics.hpp>

#define nl "\n"
#define EPSILON 1.f

bool isEqual(float lhs, float rhs)
{
    return fabs(lhs - rhs) < EPSILON;
}

sf::Vector2f winSize = { 800.f, 600.f };
sf::String winTitle = "sfml";
bool isFullscreen = false;

int main()
{
    sf::Vector2f targetPos;
    sf::Vector2f moveVec;
    bool isMoving = false;
    float playerSpd = 500.f;
    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(winSize.x, winSize.y), winTitle);
    sf::CircleShape shape(100.f, 3);
    sf::RectangleShape player({ 50.f, 100.f });

    shape.setFillColor(sf::Color::Green);
    player.setFillColor(sf::Color::Yellow);

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event))
        {
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

        if (isMoving)
        {
            player.move(moveVec * dt);
            if (isEqual(player.getPosition().x, targetPos.x) && isEqual(player.getPosition().y, targetPos.y))
                isMoving = false;
        }
        else
        {
            float speed = playerSpd * dt;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                player.move({ 0.f, -speed });
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
                player.move({ -speed, 0.f });
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                player.move({ 0.f, speed });
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                player.move({ speed, 0.f });
        }

        // sorry i lazy write function to normalise vector for consistent speed
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            targetPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            moveVec = targetPos - player.getPosition();
            isMoving = true;
        }

        //player.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));

        window.clear();
        window.draw(shape);
        window.draw(player);
        window.display();
    }

    return 0;
}