# chestnut-ecs

Entity Component System library made to be used in my Chestnut Game Engine.
The aim is to provide an API capable of flexible creation of entities, attaching components to them at runtime and being able to query them based on the component types they have. 


## Examples

### Basic usage
```cpp
using namespace chestnut::ecs;

struct HealthComponent
{
    int maxHealth;
    int currentHealth;
};

struct DamageComponent
{
    int damageDealt;
};

// Instantiate the entity world object, you will use it for most of the actions on entities.
// For full class API refer to chestnut/ecs/entity_world.hpp
CEntityWorld world;

// Create the entity.
entityid_t ent1 = world.createEntity();

// Attach a component to said entity.
// Method returns a handle to the component in the world.
// The full handle type is CEntityHandle<HealthComponent>, so use auto for convenience. 
// It is good to make your components as memory local as possible
// to optimize them for later querying.
auto healthHandle = world.createComponent<HealthComponent>(ent1);
// Use get(), '*' or '->' to access component data
healthHandle->maxHealth = 100;
healthHandle->currentHealth = 100;

world.createComponent<DamageComponent>(ent1);
auto damageHandle = world.getComponent<DamageComponent>(ent1);
// When getting the component with getComponent<>() method,
// if it hasn't been found an invalid handle is returned.
// It can be tested with a simple if statement.
if(damageHandle)
{
    damageHandle->damageDealt = 20;
}

...

// If you want to detach a component from an entity simply do
world.destroyComponent<DamageComponent>(ent1);

// Or if you're done with entire entity:
world.destroyEntity(ent1);
```


### Looking up entities

#### - Use getEntitySignature() method
```cpp
// CEntitySignature is a class which contains a set of type identifiers
// which refer to component types attached to the entity.
// It's current implementation is using std::type_index.
// For full class API refer to chestnut/ecs/entity_signature.hpp
CEntitySignature sign = world.getEntitySignature(ent1);

if(sign.has<HealthComponent>())
{
    printf("Has health component.\n");
}
```

#### - Use findEntities() method
```cpp
// Get a vector of IDs of entities that fit signature criteria
auto fightingEntities = world.findEntities([](const auto& sign) -> bool {
    return sign.has<HealthComponent, DamageComponent>();
});

for(auto id : fightingEntities)
{
    auto health = world.getComponent<HealthComponent>();
    ...
}
```

#### - Use world.entityIterator object
```cpp
// For full class API of the iterator refer to chestnut/ecs/entity_iterator.hpp
for(auto it = world.entityIterator.begin(); it != world.entityIterator.end(); ++it)
{
    // The iterator can only be used for lookup.
    // You can only create components with CEntityWorld class.
    printf("Entity id: %d\n", it.id());

    if(it.contains<HealthComponent>())
    {
        // Iterator's get() returns component reference directly
        // If a component is not attached to the entity, it throws an exception.
        HealthComponent& health = it.get<HealthComponent>();
        ...
    }
}
```

#### - Use CEntityQuery class
```cpp
// Queries are the most robust way to search for entities and their components.

// BONUS NOTE 
// Your components don't need to contain any data.
// They can merely be used as "tags" you assign to entites, like in the following example.
struct ImmunityComponent {};

// Create the query, specify types the entity should have for the first "requireSignature" argument
// and types it shouldn't have for second "rejectSignature" argument.
// Save the pointer to the query somewhere.
CEntityQuery *query = world.createQuery(
    // Look for entities that have HealthComponent and DamageComponent...
    makeEntitySignature<HealthComponent, DamageComponent>(),
    // ...but are not immune, thus we don't want ones with ImmunityComponent
    makeEntitySignature<ImmunityComponent>()
);

// Populate query
// Do this every time you want to use it as it removes entities 
// that don't fit the query anymore and adds new ones that do.
world.queryEntities(query);

// Method 1. Use query iterator
// Types provided to the iterator should not be in ImmunityComponent, otherwise you'll get assertion error.
auto it = query->begin<HealthComponent, DamageComponent>();
auto end = query->end<HealthComponent, DamageComponent>();
for(; it != end; ++it)
{
    printf("Entity id: %d\n", it.entityId());

    // Dereferencing the iterator gives you a tuple of components specified for the iterator
    // Use C++17's tuple destructuring syntax for convenience.
    // This one also returns direct references rather than handle objects.
    auto& [health, damage] = *it;

    health.currentHealth = ...
}

// Method 2. Use forEach()
// If you're getting erros regarding template type deduction you can wrap the argument lambda in std::function.
query->forEach<HealthComponent, DamageComponent>(std::function(
    [](HealthComponent& health, DamageComponent& damage) {
        ...   
    }
));

// You can also sort the query
query->sort<HealthComponent>(
    [](auto it1, auto it2) -> bool {
        // Sort query by health ascending
        return it1.currentHealth < it2.currentHealth;
    }
);
```