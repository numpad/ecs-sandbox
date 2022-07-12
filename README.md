# ecs-sandbox
Personal entity component system (ECS) sandbox. Some day this will become a fun game I want to play.


## Features
- realtime, completely destructible terrain using the marching cubes algorithm.
- emergent gameplay trough flexible combinations of components.
- performance. instanced rendering and efficient ecs design allow for updating and rendering hundreds of thousand entities.
- first-class modding support using the lua programming language. if you don't like something, or are missing a feature you can always get your hands dirty and implement it yourself.
- procedural generation. technically infinite replayability as a new, unique level is generated every run.
- thousands of items. many different and unique base items, item combinations and random modifiers.
- free, non-linear gameplay. you decide how you want to play, the game tries to be as emergent and permissive as possible while trying to give few restrictions. there is no hard goal or forced playstyle.

_(Note: these are the features and goals I want to achieve, while the building blocks for most features are already set, nearly all of them are still WIP or not even implemented at all.)_


## Screenshots
A short eye-catcher showing the latest features of the game and engine, while keeping a simple history of development milestones.
![v0.1](/website/images/screenshot-v0.1.png)

Version | Screenshot & Notable features
--------|---------------------------------------------------------------------
 v0.1   | [Added basic deferred shading](/website/images/screenshot-v0.1.png)


## Changelog
Keep track of the current development status on the [feature board](https://github.com/numpad/ecs-sandbox/projects/1).
Planned, in-development and implemented features will be listed there.

Less frequently updated but still important is the [bug tracker](https://github.com/numpad/ecs-sandbox/projects/3).
This serves as a catch-all for noting any kind of bugs appearing. While not required, a detailed description of the bug and it's cause is greatly appreciated.

### v0.2 - (unreleased)
 - Added decal render system & components

### v0.1 - (2020-07-10)
- Added changelog and version tracking
- Added deferred shading


## Building
Compiling the game should be pretty straightforward, head over to the [build instructions](website/building/README.md) to learn more about it!  
You can also simply [download a prebuilt binary](https://github.com/numpad/ecs-sandbox/releases). Keep in mind that these get updated less frequently.


## Libraries
All libraries included and/or required for the game. Some libraries are not yet used but already included as they are pretty much guaranteed to be of use.

 Library                                          | Version | License             | Description
--------------------------------------------------|---------|---------------------|-------------
[glm](https://glm.g-truc.net/)                    |  0.9.9  | MIT / modified MIT  | The mathematics library for vectors, matrices and everything inbetween.
[glfw](https://www.glfw.org)                      |  3.4.0  | zlib                | Multi-platform OpenGL library for creating windows with an OpenGL context and receiving inputs.
[entt](https://github.com/skypjack/entt)          |  3.4.0  | MIT                 | Library for the ECS architectural pattern, also comes with a great observer pattern implementation.
[dear imgui](https://github.com/ocornut/imgui)    |  1.82   | MIT                 | GUI library for debugging and visualization.
[FastNoise](https://github.com/Auburns/FastNoise) |  0.4.1  | MIT                 | Noise generation library with support for many different noise algorithms.
[sol2](https://sol2.readthedocs.io/en/latest/)    |  3.2.0  | MIT                 | Generating bindings for and working with Lua.
[Lua](http://www.lua.org/)                        |    ?    | MIT                 | A powerful, efficient, lightweight and embeddable scripting language.
[cereal](http://uscilab.github.io/cereal/)        |  1.2    | 3-clause BSD        | A serialization library supporting many target formats, both binary and human readable.
[assimp](https://www.assimp.org)                  |  5.0.0  | 3-clause BSD        | Importing many 3D model formats in a uniform manner.
[gl3w](https://github.com/skaslev/gl3w)           |    ?    | public domain       | Simple OpenGL core profile loading library.
[stb_image.h](https://github.com/nothings/stb)    |  2.22   | public domain / MIT | Image loading library.
[FreeType 2](https://www.freetype.org/)           |  2.?    | GPLv2 / FTL         | Font rasterization library.
[OpenAL-soft](https://openal-soft.org/)           |    ?    | LGPLv2              | A software implementation of the OpenAL 3D audio API.
[Yoga](https://github.com/facebook/yoga)          | 1.19.0  | MIT                 | Layout engine implementing flexbox.


## Assets
All assets that are used in the game. Some assets currently in the project are not meant to be in the final release (because of licensing problems, or simply because they do not fit the game and are just an experiment or for debugging purposes) and will soon be completely removed. Assets listed in the table below are planned for the final release.

 Asset                                                              | Creator                       | License | Category
--------------------------------------------------------------------|-------------------------------|---------|------------------
[16x16 Dungeon Tileset](https://0x72.itch.io/16x16-dungeon-tileset) | [0x72](https://0x72.itch.io/) |  CC-0   | texture / sprite


## Resources
An (incomplete) list of the resources I used in no particular order.

 Website                                                                        | Description
--------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------
[learnopengl.com](https://learnopengl.com/)                                     | The main resource I used for learning (modern) OpenGL.
[docs.gl](https://docs.gl/)                                                     | Organized version of the OpenGL reference.
[OpenGL Reference Pages](https://www.khronos.org/registry/OpenGL-Refpages/gl4/) | The official OpenGL API reference.
[entt wiki](https://github.com/skypjack/entt/wiki)                              | The entt tutorial / API reference.
[ECS back and forth](https://skypjack.github.io/2019-02-14-ecs-baf-part-1/)     | An ongoing series about deeper insights into the design, implementation and usage of ECS. From the author of entt.
[open.gl](https://open.gl/)                                                     | Another resource for learning OpenGL.
[Game programming patterns](https://gameprogrammingpatterns.com/)               | Learning about architectural patterns for game engines and games in general.
[opengl-tutorial.org](http://www.opengl-tutorial.org/)                          | Yet another learning resource for modern OpenGL.
[The book of shaders](https://thebookofshaders.com/)                            | A great book for learning more about the OpenGL shading language.
[Polygonizing a scalar field.](http://paulbourke.net/geometry/polygonise/)      | The original implementation of the marching cubes algorithm.
[0fps](https://0fps.net/)                                                       | A blog covering many topics related to game programming.
[glm API documentation](https://glm.g-truc.net/0.9.9/api/index.html/)           | The glm API reference.
[decals](https://mtnphil.wordpress.com/2014/05/24/decals-deferred-rendering/)   | Drawing decals using deferred rendering. 
[more decals](http://broniac.blogspot.com/2011/06/deferred-decals.html)         | More information about drawing decals and their weak spots.
[even more decals](https://martindevans.me/game-development/2015/02/27/Drawing-Stuff-On-Other-Stuff-With-Deferred-Screenspace-Decals/) | Even more decal information.


## License
I have not yet decided on a license.  
The goal of this project is mostly for learning purposes, both for me and other developers in a similar position.
But as this project grows, ultimately my plan is to release it as a real game for people to buy. Therefore it is still undecided how I want to handle open-source, monetization and everything related to this.
Feel free to use my project as a learning resource until I come to a decision.

