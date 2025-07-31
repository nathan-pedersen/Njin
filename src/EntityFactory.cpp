#include "EntityFactory.h"

EntityFactory::EntityFactory()
{
}

void EntityFactory::update()
{
    for (auto& e : m_toAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();

    removeDeadEntites(m_entities);

    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntites(entityVec);
    }
}

void EntityFactory::removeDeadEntites(EntityVec& entities)
{
    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](auto const& e) { return !e->isAlive(); }),
                   entities.end());
}

std::shared_ptr<Entity> EntityFactory::addEntity(const std::string& tag)
{
    auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
    m_toAdd.push_back(e);
    return e;
}

EntityVec& EntityFactory::getEntites()
{
    return m_entities;
}

EntityVec& EntityFactory::getEntites(const std::string& tag)
{
    return m_entityMap[tag];
}
