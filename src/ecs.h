#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>
#include <raylib.h>

#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32

typedef uint32_t Entity;
typedef uint32_t ComponentMask;

typedef struct {
    bool active;
    ComponentMask component_mask;
} EntityInfo;

typedef enum {
    COMPONENT_POSITION = 0,
    COMPONENT_VELOCITY = 1,
    COMPONENT_RENDER = 2,
    COMPONENT_COUNT
} ComponentType;

typedef struct {
    Vector2 position;
} PositionComponent;

typedef struct {
    Vector2 velocity;
} VelocityComponent;

typedef struct {
    Rectangle rect;
    Color color;
} RenderComponent;

typedef struct {
    EntityInfo entities[MAX_ENTITIES];
    PositionComponent positions[MAX_ENTITIES];
    VelocityComponent velocities[MAX_ENTITIES];
    RenderComponent renders[MAX_ENTITIES];
    uint32_t entity_count;
} ECS;

Entity ecs_create_entity(ECS* ecs);
void ecs_destroy_entity(ECS* ecs, Entity entity);
bool ecs_is_valid_entity(ECS* ecs, Entity entity);

void ecs_add_component(ECS* ecs, Entity entity, ComponentType type);
void ecs_remove_component(ECS* ecs, Entity entity, ComponentType type);
bool ecs_has_component(ECS* ecs, Entity entity, ComponentType type);

PositionComponent* ecs_get_position(ECS* ecs, Entity entity);
VelocityComponent* ecs_get_velocity(ECS* ecs, Entity entity);
RenderComponent* ecs_get_render(ECS* ecs, Entity entity);

void ecs_init(ECS* ecs);
void ecs_movement_system(ECS* ecs, float deltaTime);
void ecs_render_system(ECS* ecs);

Entity ecs_create_moveable_entity(ECS* ecs, Vector2 position, Vector2 velocity, Rectangle rect, Color color);

#endif