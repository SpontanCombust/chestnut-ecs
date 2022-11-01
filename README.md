# chestnut-ecs

Entity Component System library made to be used in my Chestnut Game Engine.
The aim is to provide an API capable of flexible creation of entities, attaching components to them at runtime and being able to query them based on the component types they have.

Each entity is an ordinary number. That number can be used to create components for that entry. 
A component is a data structure specified by the user. Entity can only have one component of a given type.
Components can be created, accessed and destroyed using the entity ID.
Entities and their respective components can also be accessed through the use of "queries". Queries filter out entities based on a specified set of components they should and should not have.

This repository uses Catch v2.x library for running unit tests.