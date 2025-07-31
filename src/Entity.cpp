#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(std::string tag, size_t id)
{
    m_tag   = tag;
    m_id    = id;
    m_alive = true;
}

size_t Entity::id() const
{
    return m_id;
}

bool Entity::isAlive() const
{
    return m_alive;
}

void Entity::destroy()
{
    m_alive = false;
}

const std::string& Entity::tag() const
{
    return m_tag;
}

void Entity::print() const
{
    std::print("[ ");
    std::print("Tag: {}\t| ", m_tag);
    std::print("ID: {}\t| ", m_id);
    std::print("isAlive: {}", m_alive);
    std::println(" ]");
}
