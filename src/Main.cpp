#include <SFML/Graphics.hpp>

#include "utils.h"

#include "imgui-SFML.h"
#include "imgui.h"

constexpr f32 WIDTH = 800;
constexpr f32 HEIGHT = 800;

int main(int argc, char **args)
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Window");
    window.setPosition({100, 100});
    window.setFramerateLimit(60);
    (void)ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        // -- imgui window setup
        ImGui::Begin("ImGui Window!", nullptr, ImGuiWindowFlags_MenuBar);

        ImGui::End();
        // -- imgui window setup

        // shape updates

        // clear, draw, imgui render, display
        window.clear(sf::Color::Black);
        // {}.draw

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}
