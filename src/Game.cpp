#include "Game.h"
#include <print>

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
        auto wo  = j.at("window");
        auto fo  = j.at("font");
        auto po  = j.at("player");
        auto eo  = j.at("enemy");
        auto bo  = j.at("bullet");
        auto fdo = j.at("food");

        // parse window settings
        ws.width        = wo.at("width").get<unsigned int>();
        ws.height       = wo.at("height").get<unsigned int>();
        ws.frame_limit  = wo.at("frame-limit").get<u16>();
        ws.isFullscreen = wo.at("isFullscreen").get<bool>();

        // parse font settings
        fs.file = fo.at("file").get<std::string>();
        fs.size = fo.at("size").get<u32>();

        std::vector<u8> color = fo.at("color").get<std::vector<u8>>();
        fs.color              = sf::Color(color[0], color[1], color[2]);

        // parse player settings
        ps.shape_radius     = po.at("shape-radius").get<f32>();
        ps.collision_radius = po.at("collision-radius").get<f32>();
        ps.speed            = po.at("speed").get<f32>();

        std::vector<u8> p_fillColor = po.at("fill-color").get<std::vector<u8>>();
        ps.fillColor                = sf::Color(p_fillColor[0], p_fillColor[1], p_fillColor[2]);

        std::vector<u8> p_outlineColor = po.at("outline-color").get<std::vector<u8>>();
        ps.outline_color = sf::Color(p_outlineColor[0], p_outlineColor[1], p_outlineColor[2]);

        ps.outline_thickness = po.at("outline-thickness").get<f32>();
        ps.shape_vertices    = po.at("shape-vertices").get<u32>();

        // parse enemy settings
        es.shape_radius     = eo.at("shape-radius").get<f32>();
        es.collision_radius = eo.at("collision-radius").get<f32>();
        es.min_speed        = eo.at("min-speed").get<f32>();
        es.max_speed        = eo.at("max-speed").get<f32>();

        std::vector<u8> e_outlineColor = eo.at("outline-color").get<std::vector<u8>>();
        es.outline_color = sf::Color(e_outlineColor[0], e_outlineColor[1], e_outlineColor[2]);

        es.min_vertices    = eo.at("min-vertices").get<u32>();
        es.max_vertices    = eo.at("max-vertices").get<u32>();
        es.small_lifespawn = eo.at("small-lifespan").get<u32>();
        es.spawn_interval  = eo.at("spawn-interval").get<u32>();

        // parse bullet settings
        bs.shape_radius     = bo.at("shape-radius").get<f32>();
        bs.collision_radius = bo.at("collision-radius").get<f32>();
        bs.speed            = bo.at("speed").get<f32>();

        std::vector<u8> b_fillColor = bo.at("fill-color").get<std::vector<u8>>();
        bs.fill_color               = sf::Color(b_fillColor[0], b_fillColor[1], b_fillColor[2]);

        std::vector<u8> b_outlineColor = bo.at("outline-color").get<std::vector<u8>>();
        bs.outline_color = sf::Color(b_outlineColor[0], b_outlineColor[1], b_outlineColor[2]);

        bs.shape_vertices = bo.at("shape-vertices").get<u32>();
        bs.lifespan       = bo.at("lifespan").get<u32>();

        // parse food settings
        fds.healAmount = fdo.at("heal-amount").get<u32>();
    }
    catch (json::exception& e)
    {
        std::println(stderr, "JSON Error: {}", e.what());
        exit(EXIT_FAILURE);
    }
}

void Game::printSettings()
{
    std::println("--- Window Settings ---");
    std::println("width: {} height: {}", ws.width, ws.height);
    std::println("frame-limit: {}", ws.frame_limit);
    std::println("isFullscreen: {}", ws.isFullscreen);
    std::println();

    std::println("--- Font Settings ---");
    std::println("font: {}", fs.file);
    std::println("color: {},{},{}", fs.color.r, fs.color.g, fs.color.b);
    std::println("size: {}", fs.size);
    std::println();

    std::println("--- Player Settings ---");
    std::println("shape-radius: {}", ps.shape_radius);
    std::println("collision-radius: {}", ps.collision_radius);
    std::println("speed: {}", ps.speed);
    std::println("fillColor: {},{},{}", ps.fillColor.r, ps.fillColor.g, ps.fillColor.b);
    std::println("outlineColor: {},{},{}", ps.outline_color.r, ps.outline_color.g,
                 ps.outline_color.b);
    std::println("outline-thickness: {}", ps.outline_thickness);
    std::println("shape-vertices: {}", ps.shape_vertices);
    std::println();

    std::println("--- Enemy Settings ---");
    std::println("shape-radius: {}", es.shape_radius);
    std::println("collision-radius: {}", es.collision_radius);
    std::println("min-speed: {}", es.min_speed);
    std::println("max-speed: {}", es.max_speed);
    std::println("outlineColor: {},{},{}", es.outline_color.r, es.outline_color.g,
                 es.outline_color.b);
    std::println("min-vertices: {}", es.min_vertices);
    std::println("max-vertices: {}", es.max_vertices);
    std::println("small-lifespan: {}", es.small_lifespawn);
    std::println("spawn-interval: {}", es.spawn_interval);
    std::println();

    std::println("--- Bullet Settings ---");
    std::println("shape-radius: {}", bs.shape_radius);
    std::println("collision-radius: {}", bs.collision_radius);
    std::println("speed: {}", bs.speed);
    std::println("fillColor: {},{},{}", bs.fill_color.r, bs.fill_color.g, bs.fill_color.b);
    std::println("outlineColor: {},{},{}", bs.outline_color.r, bs.outline_color.g,
                 bs.outline_color.b);
    std::println("shape-vertices: {}", bs.shape_vertices);
    std::println("lifespan: {}", bs.lifespan);
    std::println();

    std::println("--- Food Settings ---");
    std::println("heal-amount: {}", fds.healAmount);
	std::println("Test");
}
