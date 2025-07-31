#pragma once

#include <SFML/Graphics.hpp>
#include <print>
#include <fstream>
#include <tuple>

#include "EntityFactory.h"
#include "json.hpp"

using json = nlohmann::json;

struct WindowSettings
{
        unsigned int width;
        unsigned int height;
        u16          frame_limit;
        bool         isFullscreen;
};

struct FontSettings
{
        std::string file;
        u32         size;
        sf::Color   color;
};

struct PlayerSettings
{
        f32       shape_radius;
        f32       collision_radius;
        f32       speed;
        sf::Color fillColor;
        sf::Color outline_color;
        f32       outline_thickness;
        u32       shape_vertices;
};

struct EnemySettings
{
        f32       shape_radius;
        f32       collision_radius;
        f32       min_speed;
        f32       max_speed;
        sf::Color outline_color;
        u32       min_vertices;
        u32       max_vertices;
        u32       small_lifespawn;
        u32       spawn_interval;
};

struct BulletSettings
{
        f32       shape_radius;
        f32       collision_radius;
        f32       speed;
        sf::Color fill_color;
        sf::Color outline_color;
        u32       shape_vertices;
        u32       lifespan;
};

struct FoodSettings
{
        u32 healAmount;
};

class Game
{
    private:
        sf::RenderWindow m_window;
        EntityFactory    m_entityFactory;
        bool             m_paused  = false;
        bool             m_running = true;

        WindowSettings ws;
        FontSettings   fs;
        PlayerSettings ps;
        EnemySettings  es;
        BulletSettings bs;
        FoodSettings   fds;

        void init(const std::string config);
        void sMovement();
        void sUserInput();
        void sEnemySpanwer();
        void sCollision();
        void sRender();

    public:
        Game(const std::string config);
        void update();
        void printSettings();
};