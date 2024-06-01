# ImFileDialog

Some improvements from [dfranx/ImFileDialog](https://github.com/dfranx/ImFileDialog) based on my own needs, such as  

- render item icon follows the system theme
- ask confirmation if file exits in save mode
- replaced the SDL2 with glfw for the example
- upgrade to C++20
- remove the need of GTK-3 on Linux
- better support on Windows, Linux, Macos

## Dependencies

 * [Dear ImGui](https://github.com/ocornut/imgui/)
 * [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)
 * [magic_enum](https://github.com/Neargye/magic_enum)

## Compile example

### Windows

#### Install dependencies

The example requires OpenGL, GLFW3 and GLEW, on Windows, these could be installed via [vcpkg](https://github.com/microsoft/vcpkg?tab=readme-ov-file#using-vcpkg-with-cmake).

#### Build

1. `git submodule init && git submodule update`
2. `mkdir build && cd build`
3. `cmake -B . -S .. "-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]\scripts/buildsystems/vcpkg.cmake"`
4. `cmake --build .`
5. `cd Debug`
6. `./ImFileDialogExample.exe`

### Linux

#### Install dependencies

`sudo apt-get install libgl1-mesa-dev libglew-dev libglfw3-dev -y`

#### Build

1. `git submodule init && git submodule update`
2. `mkdir build && cd build`
3. `cmake ..`
4. `make -j`
5. `./ImFileDialogExample`

### MacOS

#### Install dependencies

`brew install glew glfw`

#### Build

1. `git submodule init && git submodule update`
2. `mkdir build && cd build`
3. `cmake ..`
4. `make -j`
5. `./ImFileDialogExample`

## Usage
To use ImFileDialog in your project, just add `ImFileDialog.h`, `ImFileDialog.cpp` and `StbImpl.cpp` to it.

Please note if you already use `stb_image` library in your project, just exculde the `StbImpl.cpp`,
otherwise you will have multiple definition of methods from the `stb_image` library.

According to https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1423r2.html,
to make it compile with C++20, the simplest solution to upgrade to c++20 with char8_t,
so add `/Zc:char8_t-` compiler flag to MSVC or `-fno-char8_t` if you are using `Clang`/`g++`.

And also add `-ftemplate-depth=2048` compiler flag if you are using `clang`/`g++`, this is to 
`constexpr std::array` compile.

Here's an example on how to use ImFileDialog:

1. You need to set the CreateTexture and DeleteTexture function
```c++
ifd::FileDialog::getInstance().createTexture = [](uint8_t* data, int w, int h, ifd::Format fmt) -> void* {
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (fmt == ifd::Format::BGRA) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	} else if (fmt == ifd::Format::RGBA) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return (void*)tex;
};
ifd::FileDialog::getInstance().deleteTexture = [](void* tex) {
	GLuint texID = (GLuint)tex;
	glDeleteTextures(1, &texID);
};
```

Where `ifd::Format` is defined as:

```c++
enum class Format: char{
	BGRA,
	RGBA,
	RGB
};
```

2. Open a file dialog on button press (just an example):
```c++
if (ImGui::Button("Open a texture"))
	ifd::FileDialog::getInstance().open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");
```

3. Render and check if done:
```c++
if (ifd::FileDialog::getInstance().isDone("TextureOpenDialog")) {
	if (ifd::FileDialog::getInstance().hasResult()) {
		std::string res = ifd::FileDialog::getInstance().getResult().u8string();
		printf("OPEN[%s]\n", res.c_str());
	}
	ifd::FileDialog::getInstance().close();
}
```

File filter syntax:
```
Name1 {.ext1,.ext2}, Name2 {.ext3,.ext4},.*
```

## Screenshots
**1. Table view:**

![Table view](https://user-images.githubusercontent.com/30801537/107225799-8e5b3200-6a19-11eb-9847-ca2606205402.png)

**2. Icon view:**

![Icon view](https://user-images.githubusercontent.com/30801537/107225812-92874f80-6a19-11eb-9946-e7f1a183ce9b.png)

**3. Zooming in:**

![Zooming in](https://user-images.githubusercontent.com/30801537/107225830-9a46f400-6a19-11eb-8649-06de6287fdca.gif)

**4. Favorites:**

![Favorites](https://user-images.githubusercontent.com/30801537/107225862-a5018900-6a19-11eb-9bab-c6c928eab4af.gif)

**5. Image preview + threading (CTRL + scroll):**

![Table view](https://user-images.githubusercontent.com/30801537/107225891-afbc1e00-6a19-11eb-8551-6caa4c2173d1.gif)


## LICENSE
ImFileDialog is licensed under MIT license. See [LICENSE](./LICENSE) for more details. 