Elysium, OpenGL Game Engine
In order to link the engine:
1. Add the Elysium folder (found in bin folder) as well as the Dependencies folder to your directory
2. Add #include directories from the Elysium and Dependencies folder as below
...\Dependencies\include\
...\Elysium\include\Glad\
...\Elysium\include\
...\Elysium\include\vendor\
...\Elysium\include\vendor\spdlog\include\
3. Add libraries directory from the Elysium and Dependencies folder as below
...\Dependencies\lib\$(Platform)\
...\Elysium\lib\$(Configuration)-$(Platform)\
4. Add additional dependencies or link to glfw3.h as well as Elysium.lib
5. Move the file imgui.ini and res folder to the one containing your generated executable and start-up project