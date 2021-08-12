> ![citrus_logo11](https://user-images.githubusercontent.com/60492235/122637162-d3911e00-d0f5-11eb-8534-c1d7c1085e3b.png)
# Citrus
### Citrus is 3D renderer using Direct3D 11 - C++11! Citrus only working on Windows 7+ for now :)

![](https://img.shields.io/github/stars/choi303/Citrus.svg) ![](https://img.shields.io/github/forks/choi303/Citrus.svg) ![](https://img.shields.io/github/release/choi303/Citrus.svg) ![](https://img.shields.io/github/issues/choi303/Citrus.svg) 

### To Do List
- Particle System
- Global Illumination
- Volumetric Lighting
- PBR Support

### Current Features
- Phong Lighting
- 3D Model Loader
- Normal Map & Specular Map support
- Environment Mapping
- Shadow Mapping
- Ambient Occlusion
- Filters (blur, depth, fog)
- Anti Aliasing (msaa, fxaa)
- GUI Support (Dear Imgui <3)
- Save System
- Input System

# Inputs
As you can see in the [diagram](https://miro.com/app/board/o9J_lXORBkk=/), input can be found on update function of app class. You can change the inputs to whatever you want just change the char characters on update function.
```c++
  if (keyboard.KeyIsPressed('W'))
	{
		//move forward when pressed W
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetForwardVector() * cameraSpeed * deltaTime);
	}
  //You can change 'W' to 'Up Arrow' button
  if (keyboard.KeyIsPressed(VK_UP)) 
	{
		//move forward when pressed Up Arrow (VK_UP)
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetForwardVector() * cameraSpeed * deltaTime);
        }
```
You can reach all virtual key codes on [microsoft site](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).
- 'W-A-S-D' for movement
- 'Left Shift' to 2x sprint
- 'F' for to 1.5x sprint
- 'Escape' to quit.
- 'H' to hide UI.
- 'V' to show UI.

# Gallery
Lighting looking
![Debug Screenshot 2021 06 18 - 22 16 56 67](https://user-images.githubusercontent.com/60492235/122608333-b53f0a00-d084-11eb-8280-2dada26e5668.png)

Wireframe looking
![Debug Screenshot 2021 06 18 - 22 20 13 65](https://user-images.githubusercontent.com/60492235/122608394-cbe56100-d084-11eb-9178-ce2ef3043cad.png)

Fog effect looking
![Debug Screenshot 2021 06 18 - 22 21 18 96](https://user-images.githubusercontent.com/60492235/122608416-d43d9c00-d084-11eb-9754-5af7420cdc67.png)
