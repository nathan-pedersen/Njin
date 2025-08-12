#define _USE_MATH_DEFINES

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

    // load font
    sf::Font f;
    f.loadFromFile(fontSettings.file);

    // window
    m_window = new sf::RenderWindow(sf::VideoMode(windowSettings.width, windowSettings.height),
                                    "SFML Window", sf::Style::Default);

    /* m_window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(),
                                     "SFML Window", sf::Style::Fullscreen);*/

    m_window->setVerticalSyncEnabled(true);
    m_window->setPosition({50, 50});
    m_window->setFramerateLimit(windowSettings.frame_limit);
    m_window->setKeyRepeatEnabled(false);
    (void) ImGui::SFML::Init(*m_window);

    // create player entity
    auto player = m_entityFactory.addEntity("Player");
    player->add<CTransform>(Vec2<f32>(static_cast<f32>(windowSettings.width) / 2.0f,
                                      static_cast<f32>(windowSettings.height) / 2.0f),
                            Vec2<f32>(0, 0), 45);
    player->add<CShape>(playerSettings.shape_radius, playerSettings.shape_vertices,
                        playerSettings.fillColor, playerSettings.outline_color,
                        playerSettings.outline_thickness);
    player->add<CInput>();
    player->add<CCollision>(playerSettings.collision_radius);

    auto player_pointer = m_entityFactory.addEntity("Player_Pointer");
    player_pointer->add<CTransform>(player->get<CTransform>().pos,
                                    player->get<CTransform>().velocity, 0);
    player_pointer->add<CShape>(playerSettings.shape_radius / 5.0f, 3, playerSettings.fillColor,
                                sf::Color::Red, playerSettings.outline_thickness);
    player_pointer->add<CInput>();
    player_pointer->add<CCollision>(playerSettings.collision_radius / 5.0f);

    sf::Clock totalTime;
    while (m_window->isOpen())
    {
        sf::Event event;
        while (m_window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(*m_window, event);
            if (event.type == sf::Event::Closed)
                m_window->close();
            else if (event.type == sf::Event::Resized)
            {
                windowSettings.width  = event.size.width;
                windowSettings.height = event.size.height;

                sf::View view = m_window->getView();
                view.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(windowSettings.width),
                                         static_cast<float>(windowSettings.height)));
                m_window->setView(view);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                m_paused = !m_paused;
            }
        }

        ImGui::SFML::Update(*m_window, m_deltaClock.restart());
        sf::Text t(std::to_string(m_points), f, fontSettings.size);
        t.setFillColor(fontSettings.color);
        t.setOutlineColor(sf::Color::Black);
        t.setOutlineThickness(1.0f);
        sf::Text p("PAUSED", f, 72);
        p.setFillColor(sf::Color::Red);
        p.setOutlineColor(sf::Color::Black);
        p.setOutlineThickness(1.0f);

        // -- imgui window setup
        ImGui::Begin("ImGui Window!", nullptr, ImGuiWindowFlags_MenuBar);
        ImGui::Text("player vel x: %.2f, player vel y: %.2f", player->get<CTransform>().velocity.x,
                    player->get<CTransform>().velocity.y);
        ImGui::Text("down: %d, up: %d", player->get<CInput>().down, player->get<CInput>().up);
        ImGui::Text("left: %d, right: %d", player->get<CInput>().left, player->get<CInput>().right);
        ImGui::Text("shoot: %d", player->get<CInput>().shoot);
        ImGui::Checkbox("Pause", &m_paused);
        ImGui::Text("Width: %ld", windowSettings.width);
        ImGui::Text("Height: %ld", windowSettings.height);

        if (ImGui::CollapsingHeader("Entities"))
        {
            for (auto& e : m_entityFactory.getEntites())
            {   
                ImGui::PushID(e->id());
                if (ImGui::SmallButton("X"))
                {
                    e->destroy();
                }
                ImGui::SameLine();
                if (e->tag() == "Player" || e->tag() == "Player_Pointer")
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.3f, 1.0f));
                }
                else if (e->tag() == "Enemy")
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.3f, 1.0f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.3f, 1.0f, 1.0f));
                }
                ImGui::Text("[%d] - %s", e->id(), e->tag().c_str());
                if (e->has<CTransform>())
                {
                    ImGui::SameLine();
                    ImGui::Text("@ [%.2f,%.2f], vel [%.2f,%.2f]", e->get<CTransform>().pos.x,
                                e->get<CTransform>().pos.y, e->get<CTransform>().velocity.x,
                                e->get<CTransform>().velocity.y);
                }

                ImGui::SameLine();
                ImGui::Text("[%s]", (e->isAlive()) ? "alive" : "marked destroyed");
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }

        ImGui::End();
        // -- imgui window setup end
        
        // clear, draw, imgui render, display
        m_window->clear(sf::Color(30, 30, 30));

        update();

        m_window->draw(t);
        if (m_paused)
        {
            sf::FloatRect b = p.getLocalBounds();
            p.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
            p.setPosition({static_cast<f32>(windowSettings.width) / 2.0f, static_cast<f32>(windowSettings.height) / 2.0f});
            m_window->draw(p);
        }

        ImGui::SFML::Render(*m_window);
        m_window->display();
        //
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
        if (e->has<CTransform>())
        {
            e->get<CTransform>().pos += e->get<CTransform>().velocity;
        }

        if (e->has<CTransform>() && e->has<CShape>())
        {
            e->get<CShape>().shape.setPosition(e->get<CTransform>().pos);

            // make enemies constantly rotate, anything else (just player right now) will not
            // constantly rotate.
            if (e->tag() == "Enemy")
            {
                e->get<CShape>().shape.setRotation(e->get<CShape>().shape.getRotation() +
                                                   e->get<CTransform>().angle);
            }
            else
            {
                e->get<CShape>().shape.setRotation(e->get<CTransform>().angle);
            }
        }

        if (e->has<CInput>())
        {
            // apply correct velocity based on input
            auto& in = e->get<CInput>();
            auto& tr = e->get<CTransform>();

            float vx = static_cast<float>(in.right) - static_cast<float>(in.left); // 1, 0, or -1
            float vy = static_cast<float>(in.down) - static_cast<float>(in.up);    // 1, 0, or -1

            Vec2 v{vx, vy};

            // consistant diagonal speed
            if (v.x != 0.f || v.y != 0.f)
                v = v.normalized() * playerSettings.speed;

            tr.velocity = v;

            // rotate player towards mouse position
            if (e->tag() == "Player" || e->tag() == "Player_Pointer")
            {
                sf::Vector2i _mp = sf::Mouse::getPosition(*m_window);
                Vec2<i32>    mp  = {_mp.x, _mp.y};
                Vec2<i32>    ipos{static_cast<int>(e->get<CTransform>().pos.x),
                               static_cast<int>(e->get<CTransform>().pos.y)};
                Vec2<i32>    sub = mp - ipos;
                Vec2<f32>    subf{static_cast<f32>(sub.x), static_cast<f32>(sub.y)};
                e->get<CTransform>().angle =
                    (atan2f(subf.y, subf.x) * 180.0f / static_cast<f32>(M_PI) + 90.0f);
            }
            if (e->tag() == "Player_Pointer")
            {
                auto& vplayer = m_entityFactory.getEntites("Player");

                sf::Vector2i _mp = sf::Mouse::getPosition(*m_window);
                Vec2<i32>    mp  = {_mp.x, _mp.y};
                Vec2<i32>    ipos{static_cast<i32>(e->get<CTransform>().pos.x),
                               static_cast<i32>(e->get<CTransform>().pos.y)};
                Vec2<i32>    sub  = mp - ipos;
                Vec2<f32>    subf = {static_cast<f32>(sub.x), static_cast<f32>(sub.y)};
                Vec2<f32> subfnr  = subf.normalized() * vplayer[0]->get<CShape>().shape.getRadius();
                e->get<CTransform>().pos = vplayer[0]->get<CTransform>().pos + subfnr;
            }
        }
    }
}

void Game::sUserInput()
{
    auto vplayer = m_entityFactory.getEntites("Player");
    auto player  = vplayer[0];

    for (auto& e : m_entityFactory.getEntites())
    {
        if (e->has<CInput>())
        {
            auto& in   = e->get<CInput>();
            in.up      = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
            in.down    = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            in.left    = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            in.right   = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            in.shoot   = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            in.shotgun = sf::Mouse::isButtonPressed(sf::Mouse::Right);
            in.special = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

            // Normal bullet fire
            if (in.shoot &&
                (m_bulletClock.getElapsedTime() >= m_bulletCooldown || m_bulletShots == 0))
            {
                auto bullet = m_entityFactory.addEntity("Bullet");

                sf::Vector2i _mp    = sf::Mouse::getPosition(*m_window);
                Vec2<i32>    mp     = {_mp.x, _mp.y};
                Vec2<i32>    ppos   = {static_cast<i32>(player->get<CTransform>().pos.x),
                                       static_cast<i32>(player->get<CTransform>().pos.y)};
                Vec2<i32>    sub    = mp - ppos;
                Vec2<f32>    subf   = {static_cast<f32>(sub.x), static_cast<f32>(sub.y)};
                Vec2<f32>    subfns = subf.normalized() * bulletSettings.speed;
                Vec2<f32>    vel    = subfns;

                bullet->add<CTransform>(player->get<CTransform>().pos, vel,
                                        rand_in_range_f64(-1.0f, 1.0f));
                bullet->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                    bulletSettings.fill_color, bulletSettings.outline_color, 1);
                bullet->add<CLifeSpan>(bulletSettings.lifespan);
                bullet->add<CCollision>(bulletSettings.collision_radius);

                m_bulletClock.restart();
                m_bulletShots++;
            }

            // Shotgun bullet fire
            if (in.shotgun &&
                (m_shotgunClock.getElapsedTime() >= m_shotgunCooldown || m_shotgunShots == 0))
            {
                auto pellet1 = m_entityFactory.addEntity("Bullet");
                auto pellet2 = m_entityFactory.addEntity("Bullet");
                auto pellet3 = m_entityFactory.addEntity("Bullet");

                sf::Vector2i _mp    = sf::Mouse::getPosition(*m_window);
                Vec2<i32>    mp     = {_mp.x, _mp.y};
                Vec2<i32>    ppos   = {static_cast<i32>(player->get<CTransform>().pos.x),
                                       static_cast<i32>(player->get<CTransform>().pos.y)};
                Vec2<i32>    sub    = mp - ppos;
                Vec2<f32>    subf   = {static_cast<f32>(sub.x), static_cast<f32>(sub.y)};
                Vec2<f32>    subfns = subf.normalized() * bulletSettings.speed;

                Vec2<f32> vel_1 = subfns;
                Vec2<f32> vel_2 = subfns.rotate(-8);
                Vec2<f32> vel_3 = subfns.rotate(8);

                pellet1->add<CTransform>(player->get<CTransform>().pos, vel_1,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet1->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet1->add<CLifeSpan>(bulletSettings.lifespan);
                pellet1->add<CCollision>(bulletSettings.collision_radius);

                pellet2->add<CTransform>(player->get<CTransform>().pos, vel_2,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet2->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet2->add<CLifeSpan>(bulletSettings.lifespan);
                pellet2->add<CCollision>(bulletSettings.collision_radius);

                pellet3->add<CTransform>(player->get<CTransform>().pos, vel_3,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet3->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet3->add<CLifeSpan>(bulletSettings.lifespan);
                pellet3->add<CCollision>(bulletSettings.collision_radius);

                m_shotgunClock.restart();
                m_shotgunShots++;
            }

            // Special fire
            if (in.special &&
                (m_specialClock.getElapsedTime() >= m_specialCooldown || m_specialShots == 0))
            {
                auto pellet1  = m_entityFactory.addEntity("Bullet");
                auto pellet2  = m_entityFactory.addEntity("Bullet");
                auto pellet3  = m_entityFactory.addEntity("Bullet");
                auto pellet4  = m_entityFactory.addEntity("Bullet");
                auto pellet5  = m_entityFactory.addEntity("Bullet");
                auto pellet6  = m_entityFactory.addEntity("Bullet");
                auto pellet7  = m_entityFactory.addEntity("Bullet");
                auto pellet8  = m_entityFactory.addEntity("Bullet");
                auto pellet9  = m_entityFactory.addEntity("Bullet");
                auto pellet10 = m_entityFactory.addEntity("Bullet");
                auto pellet11 = m_entityFactory.addEntity("Bullet");
                auto pellet12 = m_entityFactory.addEntity("Bullet");
                auto pellet13 = m_entityFactory.addEntity("Bullet");
                auto pellet14 = m_entityFactory.addEntity("Bullet");
                auto pellet15 = m_entityFactory.addEntity("Bullet");
                auto pellet16 = m_entityFactory.addEntity("Bullet");
                auto pellet17 = m_entityFactory.addEntity("Bullet");
                auto pellet18 = m_entityFactory.addEntity("Bullet");

                sf::Vector2i _mp    = sf::Mouse::getPosition(*m_window);
                Vec2<i32>    mp     = {_mp.x, _mp.y};
                Vec2<i32>    ppos   = {static_cast<i32>(player->get<CTransform>().pos.x),
                                       static_cast<i32>(player->get<CTransform>().pos.y)};
                Vec2<i32>    sub    = mp - ppos;
                Vec2<f32>    subf   = {static_cast<f32>(sub.x), static_cast<f32>(sub.y)};
                Vec2<f32>    subfns = subf.normalized() * bulletSettings.speed * 1.5;

                Vec2<f32> vel_1  = subfns;
                Vec2<f32> vel_2  = subfns.rotate(20);
                Vec2<f32> vel_3  = subfns.rotate(40);
                Vec2<f32> vel_4  = subfns.rotate(60);
                Vec2<f32> vel_5  = subfns.rotate(80);
                Vec2<f32> vel_6  = subfns.rotate(100);
                Vec2<f32> vel_7  = subfns.rotate(120);
                Vec2<f32> vel_8  = subfns.rotate(140);
                Vec2<f32> vel_9  = subfns.rotate(160);
                Vec2<f32> vel_10 = subfns.rotate(180);
                Vec2<f32> vel_11 = subfns.rotate(200);
                Vec2<f32> vel_12 = subfns.rotate(220);
                Vec2<f32> vel_13 = subfns.rotate(240);
                Vec2<f32> vel_14 = subfns.rotate(260);
                Vec2<f32> vel_15 = subfns.rotate(280);
                Vec2<f32> vel_16 = subfns.rotate(300);
                Vec2<f32> vel_17 = subfns.rotate(320);
                Vec2<f32> vel_18 = subfns.rotate(340);

                auto lifespan = 120;
                //
                pellet1->add<CTransform>(player->get<CTransform>().pos, vel_1,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet1->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet1->add<CLifeSpan>(lifespan);
                pellet1->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet2->add<CTransform>(player->get<CTransform>().pos, vel_2,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet2->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet2->add<CLifeSpan>(lifespan);
                pellet2->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet3->add<CTransform>(player->get<CTransform>().pos, vel_3,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet3->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet3->add<CLifeSpan>(lifespan);
                pellet3->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet4->add<CTransform>(player->get<CTransform>().pos, vel_4,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet4->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet4->add<CLifeSpan>(lifespan);
                pellet4->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet5->add<CTransform>(player->get<CTransform>().pos, vel_5,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet5->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet5->add<CLifeSpan>(lifespan);
                pellet5->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet6->add<CTransform>(player->get<CTransform>().pos, vel_6,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet6->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet6->add<CLifeSpan>(lifespan);
                pellet6->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet7->add<CTransform>(player->get<CTransform>().pos, vel_7,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet7->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet7->add<CLifeSpan>(lifespan);
                pellet7->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet8->add<CTransform>(player->get<CTransform>().pos, vel_8,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet8->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet8->add<CLifeSpan>(lifespan);
                pellet8->add<CCollision>(bulletSettings.collision_radius);

                //
                pellet9->add<CTransform>(player->get<CTransform>().pos, vel_9,
                                         rand_in_range_f64(-1.0f, 1.0f));
                pellet9->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                     bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet9->add<CLifeSpan>(lifespan);

                //
                pellet10->add<CTransform>(player->get<CTransform>().pos, vel_10,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet10->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet10->add<CLifeSpan>(lifespan);

                //
                pellet11->add<CTransform>(player->get<CTransform>().pos, vel_11,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet11->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet11->add<CLifeSpan>(lifespan);

                //
                pellet12->add<CTransform>(player->get<CTransform>().pos, vel_12,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet12->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet12->add<CLifeSpan>(lifespan);

                //
                pellet13->add<CTransform>(player->get<CTransform>().pos, vel_13,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet13->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet13->add<CLifeSpan>(lifespan);

                //
                pellet14->add<CTransform>(player->get<CTransform>().pos, vel_14,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet14->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet14->add<CLifeSpan>(lifespan);

                //
                pellet15->add<CTransform>(player->get<CTransform>().pos, vel_15,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet15->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet15->add<CLifeSpan>(lifespan);

                //
                pellet16->add<CTransform>(player->get<CTransform>().pos, vel_16,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet16->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet16->add<CLifeSpan>(lifespan);

                //
                pellet17->add<CTransform>(player->get<CTransform>().pos, vel_17,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet17->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet17->add<CLifeSpan>(lifespan);

                //
                pellet18->add<CTransform>(player->get<CTransform>().pos, vel_18,
                                          rand_in_range_f64(-1.0f, 1.0f));
                pellet18->add<CShape>(bulletSettings.shape_radius, bulletSettings.shape_vertices,
                                      bulletSettings.fill_color, bulletSettings.outline_color, 1);
                pellet18->add<CLifeSpan>(lifespan);

                m_specialClock.restart();
                m_specialShots++;
            }
        }
    }
}

void Game::sEnemySpanwer()
{
    if (m_entityFactory.getEntites("Enemy").size() == 100)
    {
        return;
    }

    Vec2<f32> topLeftSpawnBounds;
    Vec2<f32> bottomRightSpawnBounds;

    topLeftSpawnBounds.x = 0 + enemySettings.shape_radius;
    topLeftSpawnBounds.y = 0 + enemySettings.shape_radius;

    bottomRightSpawnBounds.x = windowSettings.width - enemySettings.shape_radius;
    bottomRightSpawnBounds.y = windowSettings.height - enemySettings.shape_radius;

    // generate spawn point, brute force not spawn near player kek
    f32       dist = 0.0f;
    Vec2<f32> spawnPoint;
    auto      player = m_entityFactory.getEntites("Player")[0];

    while (dist < enemySettings.collision_radius + playerSettings.collision_radius + 100)
    {
        spawnPoint = Vec2<f32>::randomPointInBounds(topLeftSpawnBounds, bottomRightSpawnBounds);
        dist       = (player->get<CTransform>().pos - spawnPoint).length();
    }

    if (m_totalFrames % enemySettings.spawn_interval == 0)
    {
        // spawn enemy
        auto a = m_entityFactory.addEntity("Enemy");
        a->add<CTransform>(
            spawnPoint,
            Vec2<f32>(rand_in_range_f64(enemySettings.min_speed, enemySettings.max_speed) *
                          rand_in_range_f64(-1.0, 1.0),
                      rand_in_range_f64(enemySettings.min_speed, enemySettings.max_speed) *
                          rand_in_range_f64(-1.0, 1.0)),
            rand_in_range_f64(-1.0, 1.0));

        a->add<CShape>(enemySettings.shape_radius,
                       rand_in_range_i32(enemySettings.min_vertices, enemySettings.max_vertices),
                       playerSettings.fillColor, enemySettings.outline_color,
                       playerSettings.outline_thickness);

        a->add<CCollision>(enemySettings.collision_radius);

        a->get<CShape>().shape.setPosition(a->get<CTransform>().pos);
    }
}

void Game::sCollision()
{
    for (auto& e : m_entityFactory.getEntites())
    {
        if (!e->has<CCollision>())
            continue;

        if (!e->has<CTransform>())
            continue;

        if (e->tag() == "Player")
        {
            if (e->get<CTransform>().pos.x <= 0 + e->get<CShape>().shape.getRadius())
            {
                e->get<CTransform>().pos.x = 0 + e->get<CShape>().shape.getRadius();
            }
            if (e->get<CTransform>().pos.x + e->get<CShape>().shape.getRadius() >
                windowSettings.width)
            {
                e->get<CTransform>().pos.x =
                    windowSettings.width - e->get<CShape>().shape.getRadius();
            }
            if (e->get<CTransform>().pos.y < 0 + e->get<CShape>().shape.getRadius())
            {
                e->get<CTransform>().pos.y = 0 + e->get<CShape>().shape.getRadius();
            }
            if (e->get<CTransform>().pos.y + e->get<CShape>().shape.getRadius() >
                windowSettings.height)
            {
                e->get<CTransform>().pos.y =
                    windowSettings.height - e->get<CShape>().shape.getRadius();
            }
        }

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

    // check collision between bullets and enemies
    for (auto& b : m_entityFactory.getEntites("Bullet"))
    {
        for (auto& e : m_entityFactory.getEntites("Enemy"))
        {
            f32 dist = (b->get<CTransform>().pos - e->get<CTransform>().pos).length();
            if (dist <= bulletSettings.collision_radius + enemySettings.collision_radius)
            {
                b->destroy();
                e->destroy();
                m_points += 100;
            }
        }
    }

    // check collision between enemies and player
    for (auto& e : m_entityFactory.getEntites("Enemy"))
    {
        auto vplayer = m_entityFactory.getEntites("Player");
        auto player  = vplayer[0];

        f32 dist = (e->get<CTransform>().pos - player->get<CTransform>().pos).length();
        if (dist <= enemySettings.collision_radius + playerSettings.collision_radius)
        {
            // TODO: reset something on death (points, upgrades, etc)
            m_paused = true;
            m_points = 0;
            for (auto& e : m_entityFactory.getEntites("Enemy"))
            {
                e->destroy();
            }

            // respawn
            player->get<CTransform>().pos =
                Vec2<f32>{(f32) windowSettings.width / 2.0f, (f32) windowSettings.height / 2.0f};
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

        if (e->get<CLifeSpan>().remainingLifeSpan <= 1)
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
    if (!m_paused)
    {
        m_entityFactory.update();
        sMovement();
        sUserInput();
        sEnemySpanwer();
        sCollision();
        sLifespan();
        m_totalFrames++;
    }

    sRender();
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
