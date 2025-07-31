#include <SFML/Graphics.hpp>
#include <print>

#include "Game.h"
#include "imgui.h"
#include "imgui-SFML.h"

Game::Game(const std::string config)
{
    init(config);
}

void Game::init(const std::string config)
{
    // parse json config file and set settings member structs
    std::ifstream in(config);
    if (!in.is_open())
    {
        std::println(stderr, "Failed to open {}", config);
        exit(EXIT_FAILURE);
    }

    json j;
    in >> j;

    try
    {
        auto windowObject = j.at("window");
        auto fontObject   = j.at("font");
        auto playerObject = j.at("player");
        auto enemyObject  = j.at("enemy");
        auto bulletObject = j.at("bullet");
        auto foodObject   = j.at("food");

        // parse window settings
        windowSettings.width        = windowObject.at("width").get<unsigned int>();
        windowSettings.height       = windowObject.at("height").get<unsigned int>();
        windowSettings.frame_limit  = windowObject.at("frame-limit").get<u16>();
        windowSettings.isFullscreen = windowObject.at("isFullscreen").get<bool>();

        // parse font settings
        fontSettings.file = fontObject.at("file").get<std::string>();
        fontSettings.size = fontObject.at("size").get<u32>();

        std::vector<u8> color = fontObject.at("color").get<std::vector<u8>>();
        fontSettings.color    = sf::Color(color[0], color[1], color[2]);

        // parse player settings
        playerSettings.shape_radius     = playerObject.at("shape-radius").get<f32>();
        playerSettings.collision_radius = playerObject.at("collision-radius").get<f32>();
        playerSettings.speed            = playerObject.at("speed").get<f32>();

        std::vector<u8> p_fillColor = playerObject.at("fill-color").get<std::vector<u8>>();
        playerSettings.fillColor    = sf::Color(p_fillColor[0], p_fillColor[1], p_fillColor[2]);

        std::vector<u8> p_outlineColor = playerObject.at("outline-color").get<std::vector<u8>>();
        playerSettings.outline_color =
            sf::Color(p_outlineColor[0], p_outlineColor[1], p_outlineColor[2]);

        playerSettings.outline_thickness = playerObject.at("outline-thickness").get<f32>();
        playerSettings.shape_vertices    = playerObject.at("shape-vertices").get<u32>();

        // parse enemy settings
        enemySettings.shape_radius     = enemyObject.at("shape-radius").get<f32>();
        enemySettings.collision_radius = enemyObject.at("collision-radius").get<f32>();
        enemySettings.min_speed        = enemyObject.at("min-speed").get<f32>();
        enemySettings.max_speed        = enemyObject.at("max-speed").get<f32>();

        std::vector<u8> e_outlineColor = enemyObject.at("outline-color").get<std::vector<u8>>();
        enemySettings.outline_color =
            sf::Color(e_outlineColor[0], e_outlineColor[1], e_outlineColor[2]);

        enemySettings.min_vertices    = enemyObject.at("min-vertices").get<u32>();
        enemySettings.max_vertices    = enemyObject.at("max-vertices").get<u32>();
        enemySettings.small_lifespawn = enemyObject.at("small-lifespan").get<u32>();
        enemySettings.spawn_interval  = enemyObject.at("spawn-interval").get<u32>();

        // parse bullet settings
        bulletSettings.shape_radius     = bulletObject.at("shape-radius").get<f32>();
        bulletSettings.collision_radius = bulletObject.at("collision-radius").get<f32>();
        bulletSettings.speed            = bulletObject.at("speed").get<f32>();

        std::vector<u8> b_fillColor = bulletObject.at("fill-color").get<std::vector<u8>>();
        bulletSettings.fill_color   = sf::Color(b_fillColor[0], b_fillColor[1], b_fillColor[2]);

        std::vector<u8> b_outlineColor = bulletObject.at("outline-color").get<std::vector<u8>>();
        bulletSettings.outline_color =
            sf::Color(b_outlineColor[0], b_outlineColor[1], b_outlineColor[2]);

        bulletSettings.shape_vertices = bulletObject.at("shape-vertices").get<u32>();
        bulletSettings.lifespan       = bulletObject.at("lifespan").get<u32>();

        // parse food settings
        foodSettings.healAmount = foodObject.at("heal-amount").get<u32>();
    }
    catch (json::exception& e)
    {
        std::println(stderr, "JSON Error: {}", e.what());
        exit(EXIT_FAILURE);
    }

    // window
    sf::RenderWindow window(sf::VideoMode(windowSettings.width, windowSettings.height),
                            "SFML Window");
    window.setPosition({50, 50});
    window.setFramerateLimit(windowSettings.frame_limit);
    (void) ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    sf::Clock totalTime;
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

void Game::printSettings()
{
    std::println("--- Window Settings ---");
    std::println("width: {} height: {}", windowSettings.width, windowSettings.height);
    std::println("frame-limit: {}", windowSettings.frame_limit);
    std::println("isFullscreen: {}", windowSettings.isFullscreen);
    std::println();

    std::println("--- Font Settings ---");
    std::println("font: {}", fontSettings.file);
    std::println("color: {},{},{}", fontSettings.color.r, fontSettings.color.g,
                 fontSettings.color.b);
    std::println("size: {}", fontSettings.size);
    std::println();

    std::println("--- Player Settings ---");
    std::println("shape-radius: {}", playerSettings.shape_radius);
    std::println("collision-radius: {}", playerSettings.collision_radius);
    std::println("speed: {}", playerSettings.speed);
    std::println("fillColor: {},{},{}", playerSettings.fillColor.r, playerSettings.fillColor.g,
                 playerSettings.fillColor.b);
    std::println("outlineColor: {},{},{}", playerSettings.outline_color.r,
                 playerSettings.outline_color.g, playerSettings.outline_color.b);
    std::println("outline-thickness: {}", playerSettings.outline_thickness);
    std::println("shape-vertices: {}", playerSettings.shape_vertices);
    std::println();

    std::println("--- Enemy Settings ---");
    std::println("shape-radius: {}", enemySettings.shape_radius);
    std::println("collision-radius: {}", enemySettings.collision_radius);
    std::println("min-speed: {}", enemySettings.min_speed);
    std::println("max-speed: {}", enemySettings.max_speed);
    std::println("outlineColor: {},{},{}", enemySettings.outline_color.r,
                 enemySettings.outline_color.g, enemySettings.outline_color.b);
    std::println("min-vertices: {}", enemySettings.min_vertices);
    std::println("max-vertices: {}", enemySettings.max_vertices);
    std::println("small-lifespan: {}", enemySettings.small_lifespawn);
    std::println("spawn-interval: {}", enemySettings.spawn_interval);
    std::println();

    std::println("--- Bullet Settings ---");
    std::println("shape-radius: {}", bulletSettings.shape_radius);
    std::println("collision-radius: {}", bulletSettings.collision_radius);
    std::println("speed: {}", bulletSettings.speed);
    std::println("fillColor: {},{},{}", bulletSettings.fill_color.r, bulletSettings.fill_color.g,
                 bulletSettings.fill_color.b);
    std::println("outlineColor: {},{},{}", bulletSettings.outline_color.r,
                 bulletSettings.outline_color.g, bulletSettings.outline_color.b);
    std::println("shape-vertices: {}", bulletSettings.shape_vertices);
    std::println("lifespan: {}", bulletSettings.lifespan);
    std::println();

    std::println("--- Food Settings ---");
    std::println("heal-amount: {}", foodSettings.healAmount);
}
