# Oubinity Voxel Engine

### Overview
Oubinity is a voxel engine that focuses on a terrain rendering.

![](./documentation_resources/scene-1.png)  |  ![](./documentation_resources/canyon-1.png)
:------------------------------------------:|:-------------------------------------------:
![](./documentation_resources/canyon-2.png)  |  ![](./documentation_resources/hills-1.png)

  <div style="display:flex">
    <img src="./documentation_resources/scene-8.png"/>
  </div>

### Build
Build on Windows or Linux using Cmake
##### Windows
```
1. git clone https://github.com/nlins8224/Oubinity.git
2. cd Oubinity
3. cmake .  
4. cmake  --build . --config Release
5. Navigate to Oubinity/bin
```
Either run the binary in `Oubinity/bin` or open `Oubinity.sln` and run under Visual Studio.
If using Visual Studio `Release` or `RelWithDebInfo` is recommended.

##### Linux
```
1. git clone https://github.com/nlins8224/Oubinity.git
2. cd Oubinity
3. cmake -DCMAKE_BUILD_TYPE=Release .
4. cmake  --build .
5. Navigate to Oubinity/bin
```
Run the binary in `Oubinity/bin`.

### Documentation
A usage cheatsheet and an overview of the world generation pipeline, terrain generation, meshing, rendering and core algorithms used is available under [DOCUMENTATION.md](https://github.com/nlins8224/Oubinity/blob/docs_update/DOCUMENTATION.md)

### Roadmap
Roadmap can be found [here](https://github.com/users/nlins8224/projects/2/views/1)