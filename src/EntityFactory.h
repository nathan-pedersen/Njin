#pragma once

#include <vector>
#include <memory>
#include <map>

#include "Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityFactory
{
        EntityVec m_entities;
        EntityVec m_toAdd;
        EntityMap m_entityMap;
        size_t    m_totalEntities = 0;

    public:
        EntityFactory();
        void                    update();
        void                    removeDeadEntites(EntityVec& entities);
        std::shared_ptr<Entity> addEntity(const std::string& tag);
        EntityVec&              getEntites();
        EntityVec&              getEntites(const std::string& tag);
};