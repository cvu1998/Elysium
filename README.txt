Elysium, OpenGL Game Engine
In order to link the engine:
1. Add the Elysium folder (found in Export folder) as well as the Dependencies folder to your directory
2. Add #include directories from the Elysium and Dependencies folder as below
...\Elysium\include\Glad\
...\Elysium\include\
...\Elysium\include\vendor\
3. Add libraries directory from the Elysium and Dependencies folder as below
...\Dependencies\lib\$(Platform)\
4. Add additional dependencies or link to glfw3.h as well as Elysium.lib (which you can get by building the Visual Studio solution)
5. In your project settings C/C++ -> Code Generation -> Runtime Library
   For a Debug configuration, set the value to be Multi-threaded Debug DLL (MDd)
   For a Release configuration, set the value to be Multi-threaded DLL (MD)
6. Move the file imgui.ini and the res folder to the folder which will contain your generated executable and start-up project