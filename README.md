# computer-graphics
To be continued...
<h3>Building development environment</h3>

1. Install OpenGL </br><code>
sudo apt-get install cmake libx11-dev xorg-dev libglu1-mesa-dev freeglut3-dev libglew1.5 libglew1.5-dev libglu1-mesa libglu1-mesa-dev libgl1-mesa-glx libgl1-mesa-dev libglfw3-dev libglfw3</code>

2. Install OpenCV </br>
  2.1. Update the Ubuntu System Package  </br>
<code>sudo apt-get update && sudo apt-get upgrade</code></br>
  2.2. Install Required tools and packages </br>
<code>sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev</code></br>
  2.3. Install Python-Dev </br>
<code>sudo apt-add-repository ppa:deadsnakes/ppa</code> </br>
<code>sudo apt-get install python3.5-dev python3-numpy libtbb2 libtbb-dev</code>

3. Install GLM </br>
`sudo apt-get install libglm-dev`

4. Install SDL </br>
`sudo apt-get install libsdl2-2.0` </br>
`sudo apt-get install libsdl2-dev`

5. Install SOIL2 </br>
  5.1 Copy </br>
 `sudo cp -r /home/jordi /Downloads/SOIL2/src/SOIL2 /usr/local/include/SOIL2  ` </br>
 `sudo cp lib/linux/libsoil2.a /usr/lib/x86_64-linux-gnu/ ` </br>
 `sudo cp lib/linux/libsoil2-debug.a /usr/lib/x86_64-linux-gnu/ `

6. Example code </br>
`#include <SOIL2/SOIL2.h>`
  
7. Compilation </br> 
`g++ *.cpp -lmysqlclient -lm -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -L/usr/include/ -lsoil2 -o main.out`

# computer-graphics
