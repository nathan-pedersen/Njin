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
    parseJson(config);

    // test entities
    auto e = m_entityFactory.addEntity("Player");
    e->add<CTransform>(Vec2{200, 200}, Vec2{2.4, 2.4}, 1);
    e->add<CShape>(playerSettings.shape_radius, playerSettings.shape_vertices,
                   playerSettings.fillColor, playerSettings.outline_color,
                   playerSettings.outline_thickness);
    e->get<CShape>().shape.setPosition(e->get<CTransform>().pos);

    auto f = m_entityFactory.addEntity("Enemy");
    f->add<CTransform>(Vec2{1000, 1000}, Vec2{-1, 3.2}, -1);
    f->add<CShape>(enemySettings.shape_radius,
                   rand_in_range_i32(enemySettings.min_vertices, enemySettings.max_vertices),
                   playerSettings.fillColor, enemySettings.outline_color,
                   playerSettings.outline_thickness);
    f->get<CShape>().shape.setPosition(f->get<CTransform>().pos);

    auto k = m_entityFactory.addEntity("Enemy");
    k->add<CTransform>(Vec2{1820, 140}, Vec2{-4.5, 7.9}, -1);
    k->add<CShape>(enemySettings.shape_radius,
                   rand_in_range_i32(enemySettings.min_vertices, enemySettings.max_vertices),
                   playerSettings.fillColor, enemySettings.outline_color,
                   playerSettings.outline_thickness);
    k->get<CShape>().shape.setPosition(k->get<CTransform>().pos);
    k->add<CLifeSpan>(120);

    // window
    m_window = new sf::RenderWindow(sf::VideoMode(windowSettings.width, windowSettings.height),
                                    "SFML Window");
    m_window->setPosition({50, 50});
    m_window->setFramerateLimit(windowSettings.frame_limit);
    (void) ImGui::SFML::Init(*m_window);

    sf::Clock deltaClock;
    sf::Clock totalTime;
    while (m_window->isOpen())
    {
        sf::Event event;
        while (m_window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(*m_window, event);
            if (event.type == sf::Event::Closed)
            {
                m_window->close();
            }
        }
        ImGui::SFML::Update(*m_window, deltaClock.restart());

        // -- imgui window setup
        ImGui::Begin("ImGui Window!", nullptr, ImGuiWindowFlags_MenuBar);

        ImGui::Checkbox("Pause", &m_paused);

        ImGui::End();
        // -- imgui window setup end

        // updates
        if (!m_paused)
        {
            update();
        }

        m_entityFactory.update();

        // clear, draw, imgui render, display
        m_window->clear(sf::Color(30, 30, 30));
        // {}.draw
        sRender();

        ImGui::SFML::Render(*m_window);
        m_window->display();
    }
    ImGui::SFML::Shutdown();
}

void Game::parseJson(std::string config)
{
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
}

void Game::sMovement()
{
    for (auto& e : m_entityFactory.getEntites())
    {
        if (e->has<CTransform>() && e->has<CShape>())
        {
            e->get<CTransform>().pos += e->get<CTransform>().velocity;
            e->get<CShape>().shape.setRotation(e->get<CShape>().shape.getRotation() +
                                               e->get<CTransform>().angle);
            e->get<CShape>().shape.setPosition(e->get<CTransform>().pos);
        }
    }
}

void Game::sUserInput()
{
}

void Game::sEnemySpanwer()
{
}

void Game::sCollision()
{
    for (auto& e : m_entityFactory.getEntites())
    {
        if (!e->has<CTransform>())
            continue;

        if (e->get<CTransform>().pos.x < 0 + e->get<CShape>().shape.getRadius() ||
            e->get<CTransform>().pos.x + e->get<CShape>().shape.getRadius() > windowSettings.width)
        {
            e->get<CTransform>().velocity.x *= -1;
        }

        if (e->get<CTransform>().pos.y < 0 + e->get<CShape>().shape.getRadius() ||
            e->get<CTransform>().pos.y + e->get<CShape>().shape.getRadius() > windowSettings.height)
        {
            e->get<CTransform>().velocity.y *= -1;
        }
    }
}

void Game::sRender()
{
    for (auto& e : m_entityFactory.getEntites())
    {
        if (e->has<CShape>())
        {
            m_window->draw(e->get<CShape>().shape);
        }
    }
}

void Game::sLifespan()
{
    for (auto& e : m_entityFactory.getEntites())
    {
        if (!e->has<CLifeSpan>())
            continue;

        if (e->get<CLifeSpan>().remainingLifeSpan <= 0)
        {
            e->destroy();
        }
        e->get<CLifeSpan>().remainingLifeSpan--;

        // opacity of shape based on lifespan
        if (e->has<CShape>())
        {
            u8 lifespanOpacity =
                ((f32) e->get<CLifeSpan>().remainingLifeSpan / e->get<CLifeSpan>().totalLifeSpan) *
                255.0;

            auto currentFillColor = e->get<CShape>().shape.getFillColor();
            e->get<CShape>().shape.setFillColor(sf::Color(currentFillColor.r, currentFillColor.g,
                                                          currentFillColor.b, lifespanOpacity));

            auto currentOutlineColor = e->get<CShape>().shape.getOutlineColor();
            e->get<CShape>().shape.setOutlineColor(
                sf::Color(currentOutlineColor.r, currentOutlineColor.g, currentOutlineColor.b,
                          lifespanOpacity));
        }
    }
}

void Game::update()
{
    sMovement();
    sUserInput();
    sEnemySpanwer();
    sCollision();
    sLifespan();
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
