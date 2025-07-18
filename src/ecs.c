#include "ecs.h"
#include <string.h>

void ecs_init(ECS* ecs) {
    memset(ecs, 0, sizeof(ECS));
    ecs->entity_count = 0;
}

Entity ecs_create_entity(ECS* ecs) {
    if (ecs->entity_count >= MAX_ENTITIES) {
        return MAX_ENTITIES;
    }
    
    Entity entity = ecs->entity_count++;
    ecs->entities[entity].active = true;
    ecs->entities[entity].component_mask = 0;
    
    return entity;
}

void ecs_destroy_entity(ECS* ecs, Entity entity) {
    if (entity >= MAX_ENTITIES || !ecs->entities[entity].active) {
        return;
    }
    
    ecs->entities[entity].active = false;
    ecs->entities[entity].component_mask = 0;
}

bool ecs_is_valid_entity(ECS* ecs, Entity entity) {
    return entity < MAX_ENTITIES && ecs->entities[entity].active;
}

void ecs_add_component(ECS* ecs, Entity entity, ComponentType type) {
    if (!ecs_is_valid_entity(ecs, entity) || type >= COMPONENT_COUNT) {
        return;
    }
    
    ecs->entities[entity].component_mask |= (1 << type);
}

void ecs_remove_component(ECS* ecs, Entity entity, ComponentType type) {
    if (!ecs_is_valid_entity(ecs, entity) || type >= COMPONENT_COUNT) {
        return;
    }
    
    ecs->entities[entity].component_mask &= ~(1 << type);
}

bool ecs_has_component(ECS* ecs, Entity entity, ComponentType type) {
    if (!ecs_is_valid_entity(ecs, entity) || type >= COMPONENT_COUNT) {
        return false;
    }
    
    return (ecs->entities[entity].component_mask & (1 << type)) != 0;
}

PositionComponent* ecs_get_position(ECS* ecs, Entity entity) {
    if (!ecs_has_component(ecs, entity, COMPONENT_POSITION)) {
        return NULL;
    }
    return &ecs->positions[entity];
}

VelocityComponent* ecs_get_velocity(ECS* ecs, Entity entity) {
    if (!ecs_has_component(ecs, entity, COMPONENT_VELOCITY)) {
        return NULL;
    }
    return &ecs->velocities[entity];
}

RenderComponent* ecs_get_render(ECS* ecs, Entity entity) {
    if (!ecs_has_component(ecs, entity, COMPONENT_RENDER)) {
        return NULL;
    }
    return &ecs->renders[entity];
}

void ecs_movement_system(ECS* ecs, float deltaTime) {
    for (Entity entity = 0; entity < ecs->entity_count; entity++) {
        if (!ecs_is_valid_entity(ecs, entity)) {
            continue;
        }
        
        if (ecs_has_component(ecs, entity, COMPONENT_POSITION) && 
            ecs_has_component(ecs, entity, COMPONENT_VELOCITY)) {
            
            PositionComponent* pos = ecs_get_position(ecs, entity);
            VelocityComponent* vel = ecs_get_velocity(ecs, entity);
            
            if (pos && vel) {
                pos->position.x += vel->velocity.x * deltaTime;
                pos->position.y += vel->velocity.y * deltaTime;
            }
        }
    }
}

void ecs_render_system(ECS* ecs) {
    for (Entity entity = 0; entity < ecs->entity_count; entity++) {
        if (!ecs_is_valid_entity(ecs, entity)) {
            continue;
        }
        
        if (ecs_has_component(ecs, entity, COMPONENT_POSITION) && 
            ecs_has_component(ecs, entity, COMPONENT_RENDER)) {
            
            PositionComponent* pos = ecs_get_position(ecs, entity);
            RenderComponent* render = ecs_get_render(ecs, entity);
            
            if (pos && render) {
                Rectangle rect = render->rect;
                rect.x = pos->position.x;
                rect.y = pos->position.y;
                DrawRectangleRec(rect, render->color);
            }
        }
    }
}

Entity ecs_create_moveable_entity(ECS* ecs, Vector2 position, Vector2 velocity, Rectangle rect, Color color) {
    Entity entity = ecs_create_entity(ecs);
    if (entity == MAX_ENTITIES) {
        return MAX_ENTITIES;
    }
    
    ecs_add_component(ecs, entity, COMPONENT_POSITION);
    ecs_add_component(ecs, entity, COMPONENT_VELOCITY);
    ecs_add_component(ecs, entity, COMPONENT_RENDER);
    
    PositionComponent* pos = ecs_get_position(ecs, entity);
    VelocityComponent* vel = ecs_get_velocity(ecs, entity);
    RenderComponent* render = ecs_get_render(ecs, entity);
    
    if (pos) pos->position = position;
    if (vel) vel->velocity = velocity;
    if (render) {
        render->rect = rect;
        render->color = color;
    }
    
    return entity;
}