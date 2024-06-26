#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <time.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#pragma comment(lib, "opengl32.lib")

#include "ImFileDialog.h"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

void glfwErrorCallback(int error, const char* description)
{
    const std::string message = "Glfw Error " + std::to_string(error) + ": " + description;
    throw std::runtime_error(message);
}

int main(int argc, char* argv[])
{
	glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw.");
    }
    
    const auto glslVersion = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Create window with graphics context
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Historical Map", NULL, NULL);
    if (window == NULL) {
        throw std::runtime_error("Failed to create window.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return 0;
	}

	// imgui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

	// ImFileDialog requires you to set the CreateTexture and DeleteTexture
	ifd::FileDialog::getInstance().createTexture = [](const uint8_t* data, int w, int h, ifd::Format fmt) -> void* {
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

		return reinterpret_cast<void*>(tex);
	};

	ifd::FileDialog::getInstance().deleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
	};

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Simple window
		ImGui::Begin("Control Panel");
	
		if (ImGui::Button("Open file")) {
			ifd::FileDialog::getInstance().open("ShaderOpenDialog", "Open a shader", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
		}
	
		if (ImGui::Button("Open directory")) {
			ifd::FileDialog::getInstance().open("DirectoryOpenDialog", "Open a directory", "");
		}

		if (ImGui::Button("Save file")) {
			ifd::FileDialog::getInstance().save("ShaderSaveDialog", "Save a shader", "*.sprj {.sprj}");
		}

		ImGui::End();

		// file dialogs
		if (ifd::FileDialog::getInstance().isDone("ShaderOpenDialog")) {
			if (ifd::FileDialog::getInstance().hasResult()) {
				const std::vector<std::filesystem::path>& res = ifd::FileDialog::getInstance().getResults();
				for (const auto& r : res) {// ShaderOpenDialog supports multiselection
					printf("OPEN[%s]\n", r.u8string().c_str());
				}
			}
			ifd::FileDialog::getInstance().close();
		}

		if (ifd::FileDialog::getInstance().isDone("DirectoryOpenDialog")) {
			if (ifd::FileDialog::getInstance().hasResult()) {
				std::string res = ifd::FileDialog::getInstance().getResult().u8string();
				printf("DIRECTORY[%s]\n", res.c_str());
			}
			ifd::FileDialog::getInstance().close();
		}

		if (ifd::FileDialog::getInstance().isDone("ShaderSaveDialog")) {
			if (ifd::FileDialog::getInstance().hasResult()) {
				std::string res = ifd::FileDialog::getInstance().getResult().u8string();
				printf("SAVE[%s]\n", res.c_str());
			}
			ifd::FileDialog::getInstance().close();
		}



		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		// render Dear ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Dear ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}
