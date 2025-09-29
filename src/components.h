#ifndef COMPONENTS_H
#define COMPONENTS_H

typedef enum {
    COMPONENT_NONE         = 0,
    COMPONENT_MOVEABLE     = 1 << 0,
    COMPONENT_JUMPABLE     = 1 << 1,
    COMPONENT_CONTROLLABLE = 1 << 2,
    COMPONENT_FALLABLE     = 1 << 3,
} ComponentType;

#endif  // !COMPONENTS_H
