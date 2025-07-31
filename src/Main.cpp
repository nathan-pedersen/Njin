#include <SFML/Graphics.hpp>
#include <print>

#include "imgui-SFML.h"
#include "imgui.h"

#include "Vec2.h"
#include "Game.h"

#include "EntityFactory.h"

constexpr unsigned int WIDTH  = 800;
constexpr unsigned int HEIGHT = 600;

// #define TESTING

int main(void)
{
    // --- If need to test something define TESTING
#ifdef TESTING
    Game game("config.json");
    game.printSettings();

    return 0;
#endif
    // --- testing end

    // --- Actual program start ---
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Window");
    window.setPosition({400, 400});
    (void) ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    sf::Clock totalTime;
    while (window.isOpen())
    {
        std::println("{}", totalTime.getElapsedTime().asSeconds());
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
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("Save", "ctrl+s");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::MenuItem("Save", "ctrl+s");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Save", "ctrl+s");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Go"))
            {
                ImGui::MenuItem("Save", "ctrl+s");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
        // -- imgui window setup

        // updates

        // clear, draw, imgui render, display
        window.clear(sf::Color::Black);
        // {}.draw

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}
