#include <iostream>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#define nl "\n"
#define EPSILON 1.f
#define PI 3.141592654

bool isEqual(float lhs, float rhs)
{
    return fabs(lhs - rhs) < EPSILON;
}

struct Line
{
    sf::Vector3f point, dir;

    Line(sf::Vector3f _point, sf::Vector3f _dir)
        : point(_point), dir(_dir) { }
};

struct Plane
{
    sf::Vector3f point, normal;
    float a = 0.f, b = 0.f, c = 0.f, d = 0.f;

    Plane(sf::Vector3f _point, sf::Vector3f _normal)
        : point(_point), normal(_normal), a(normal.x), b(normal.y), c(normal.z), 
        d(-(point.x * normal.x + point.y * normal.y + point.z * normal.z)) { }

    void recalc()
    {
        a = normal.x;
        b = normal.y;
        c = normal.z;
        d = -(point.x * normal.x + point.y * normal.y + point.z * normal.z);
    }
};

struct Cube
{
    float len = 0.f;
    sf::Vector3f point;

    Cube(float _len, const sf::Vector3f &topLeftBack = sf::Vector3f())
        : len(_len), point(topLeftBack) { }

    sf::Vector3f points[8] =
    {
        { point.x, point.y, point.z },
        { point.x + len, point.y, point.z },
        { point.x, point.y + len, point.z },
        { point.x + len, point.y + len, point.z },
        { point.x, point.y, point.z + len },
        { point.x + len, point.y, point.z + len },
        { point.x, point.y + len, point.z + len },
        { point.x + len, point.y + len, point.z + len }
    };
};

Line findLine(const sf::Vector3f &point1, const sf::Vector3f &point2)
{
    return Line(point1, point2 - point1);
}

sf::Vector3f findIntersection(const Line &line, const Plane &plane)
{
    float t = -(plane.a * line.point.x + plane.b * line.point.y + plane.c * line.point.z + plane.d) /
        (plane.a * line.dir.x + plane.b * line.dir.y + plane.c * line.dir.z); // point + t * vector in line equation
    // do something if t > 1.f
    return { line.point.x + t * line.dir.x, line.point.y + t * line.dir.y, line.point.z + t * line.dir.z };
}

sf::Vector2f alignWithEye(const sf::Vector3f &point, const sf::Vector2f &winSize, const sf::Vector3f &eye)
{
    return { point.x + winSize.x / 2.f - eye.x, point.y + winSize.y / 2.f - eye.y };
}

sf::Vector2f swizzle(const sf::Vector3f &point)
{
    return { point.x, point.y };
}

float findLen(const sf::Vector3f &vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

sf::Vector3f normalize(const sf::Vector3f &vec)
{
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (length)
        return { vec.x / length, vec.y / length, vec.z / length };
    return sf::Vector3f();
}

sf::Vector3f findPoint(const sf::Vector3f &point, const sf::Vector3f &dir, float dist) 
{
    return point + normalize(dir) * dist;
}

// @brief converts the given angle from radians to degrees
// @param radians
// @return degrees
float radToDeg(float rad)
{
    return rad * 180.f / static_cast<float>(PI);
}

// @brief converts the given angle from degrees to radians
// @param degrees
// @param radians
float degToRad(float deg)
{
    return deg * static_cast<float>(PI) / 180.f;
}

sf::Vector3f dot(const sf::Vector3f &lhs, const sf::Vector3f &rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

sf::Vector3f findDir(const sf::Vector3f &point, const Line &line)
{
    sf::Vector3f pointToLine = line.point - point;
    sf::Vector3f proj = dot(pointToLine, line.dir) / findLen(line.dir);
    return pointToLine - dot(proj, normalize(line.dir));
}

sf::Vector3f rotateX(const sf::Vector3f &point, float angle)
{
    sf::Vector3f temp;
    temp.x = point.x;
    temp.y = cosf(angle) * point.y - sinf(angle) * point.z;
    temp.z = sinf(angle) * point.y + cosf(angle) * point.z;
    return temp;
}

void drawCube(const Cube &cube, const sf::Color &color, sf::RenderWindow &window)
{
    std::vector<std::array<sf::Vertex, 2>> lines;

    lines.push_back({ sf::Vertex(swizzle(cube.points[0]), color), sf::Vertex(swizzle(cube.points[1]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[0]), color), sf::Vertex(swizzle(cube.points[2]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[1]), color), sf::Vertex(swizzle(cube.points[3]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[2]), color), sf::Vertex(swizzle(cube.points[3]), color) });

    lines.push_back({ sf::Vertex(swizzle(cube.points[4]), color), sf::Vertex(swizzle(cube.points[5]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[4]), color), sf::Vertex(swizzle(cube.points[6]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[5]), color), sf::Vertex(swizzle(cube.points[7]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[6]), color), sf::Vertex(swizzle(cube.points[7]), color) });

    lines.push_back({ sf::Vertex(swizzle(cube.points[1]), color), sf::Vertex(swizzle(cube.points[5]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[0]), color), sf::Vertex(swizzle(cube.points[4]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[3]), color), sf::Vertex(swizzle(cube.points[7]), color) });
    lines.push_back({ sf::Vertex(swizzle(cube.points[2]), color), sf::Vertex(swizzle(cube.points[6]), color) });

    for (const auto &line : lines)
        window.draw(line.data(), 2, sf::Lines);
}

int main()
{
    sf::Vector2f winSize = { 1600.f, 900.f };
    sf::String winTitle = "sfml";
    bool isFullscreen = false;
    float cubeLen = 100.f;
    float distFromScreen = 1000.f;
    float speed = 250.f;

    sf::Clock clock;
    sf::Font font;
    sf::Vector2i prevMouse = sf::Mouse::getPosition();

    font.loadFromFile("../Assets/Fonts/PoorStoryRegular.ttf");
    sf::Text distText;
    distText.setFont(font);
    distText.setCharacterSize(32);
    distText.setFillColor(sf::Color::White);
    distText.setPosition(100.f, 800.f);

    Cube cube(cubeLen);
    Cube cube2(cubeLen, { -200.f, 0.f, 100.f });
    sf::Vector3f eye = { 50.f, 50.f, 300.f };
    Plane screen({ 50.f, 50.f, eye.z - distFromScreen }, { 0.f, 0.f, -1.f });
    sf::RenderWindow window(sf::VideoMode(winSize.x, winSize.y), winTitle);
    window.setFramerateLimit(60);

    // Initialize ImGui-SFML
    ImGui::SFML::Init(window);

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

        // Example ImGui window
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();

        window.clear();

        // Render ImGui into window
        ImGui::SFML::Render(window);

        Cube proj(0.f); // 0.f is placeholder
        for (int i = 0; i < 8; ++i)
        {
            proj.points[i] = findIntersection(findLine(cube.points[i], eye), screen);
            proj.points[i].x += winSize.x / 2.f - eye.x;
            proj.points[i].y += winSize.y / 2.f - eye.y;
            cube.points[i] = rotateX(cube.points[i], degToRad(speed * dt));
        }
        drawCube(proj, sf::Color::Green, window);

        Cube proj2(0.f); // 0.f is placeholder
        for (int i = 0; i < 8; ++i)
        {
            proj2.points[i] = findIntersection(findLine(cube2.points[i], eye), screen);
            proj2.points[i].x += winSize.x / 2.f - eye.x;
            proj2.points[i].y += winSize.y / 2.f - eye.y;
        }
        drawCube(proj2, sf::Color::Green, window);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            eye.y -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            eye.x -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            eye.y += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            eye.x += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M))
            eye.z -= speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
            eye.z += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O))
            distFromScreen += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
            distFromScreen -= distFromScreen > 0.f ? speed * dt : 0.f;

        // work in progress
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            sf::Vector2i currMouse = sf::Mouse::getPosition();
            sf::Vector2i deltaMouse = prevMouse - currMouse;
            screen.normal.x += deltaMouse.x * 0.1;
            screen.normal.y += deltaMouse.y * 0.1;
            prevMouse = currMouse;
        }
        else
            prevMouse = sf::Mouse::getPosition();

        screen.point = findPoint(eye, screen.normal, distFromScreen);
        screen.recalc();
        distText.setString("Focal Length: " + std::to_string(distFromScreen));

        window.draw(distText);
        window.display();
    }

    return 0;
}