#pragma once

#include <string>
#include <tuple>
#include <print>

#include "CLifeSpan.h"
#include "CShape.h"
#include "CTransform.h"
#include "CInput.h"

using ComponentTuple = std::tuple<CShape, CTransform, CLifeSpan, CInput>;

class Entity
{
        ComponentTuple m_components;
        bool           m_alive = true;
        std::string    m_tag   = "default";
        size_t         m_id    = 0;
        Entity();
        Entity(std::string tag, size_t id);
        friend class EntityFactory;

    public:
        template <typename T>
        T& get()
        {
            return std::get<T>(m_components);
        }

        template <typename T>
        const T& get() const
        {
            return std::get<T>(m_components);
        }

        template <typename T>
        bool has() const
        {
            return get<T>().exists;
        }

        template <typename T>
        void remove()
        {
            get<T>() = T();
        }

        template <typename T, typename... TArgs>
        T& add(TArgs&&... mArgs)
        {
            auto& component  = get<T>();
            component        = T(std::forward<TArgs>(mArgs)...);
            component.exists = true;
            return component;
        }

        size_t             id() const;
        bool               isAlive() const;
        void               destroy();
        const std::string& tag() const;
        void               print() const;
};
