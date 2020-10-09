Elysium, OpenGL Game Engine
In order to link the engine:
1. Add the Elysium folder as well as the dependencies folder to your directory
2. Add #include directories from the Elysium folder as below
...\Dependencies\include\
...\Elysium\include\Glad\
...\Elysium\include\
...\Elysium\include\vendor\
...\Elysium\include\vendor\spdlog\include\
3. As well as adding the directory containing Elysium.lib, add a library directory such as the one below
...\Dependencies\lib\$(Platform)\
4. Link to glfw3.h as well as Elysium.lib