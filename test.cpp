#include <SDL2/SDL.h>
#include "Managers/FrameMan.h"
#include "System/System.h"

#include "RTERenderer.h"
#include "GL/glew.h"

using namespace RTE;

static bool quit = false;
int quit_handler(void *quit, SDL_Event *event) {
	if ((*event).type == SDL_QUIT) {
		*static_cast<int *>(quit) = true;
	}
	return 1;
}

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	    type, severity, message);
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_AddEventWatch(quit_handler, &quit);
	g_FrameMan.CreateWindowAndRenderer();
	System::Initialize();
	g_FrameMan.Initialize();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	SDL_Event e;
	std::shared_ptr<VertexArray> va;
	std::vector<Vertex> tri;
	tri.emplace_back(0.0f, 0.0f, glm::u8vec4(255));
	tri.emplace_back(1.0f, 0.0f, glm::u8vec4(255));
	tri.emplace_back(0.0f, 1.0f, glm::u8vec4(255, 255, 0, 255));
	va = std::make_shared<VertexArray>(std::move(tri));
	std::cout << va->GetVertexCount() << ": " << va->GetVAO() << std::endl;
	std::shared_ptr<Shader> color = std::make_shared<Shader>();
	if (!color->Compile("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32VertexColor.frag")) {
		quit = true;
	}
	color->Use();
	color->SetVector4f(color->GetColorUniform(), glm::vec4(1.0));
	color->SetMatrix4f(color->GetProjectionUniform(), glm::mat4(1));
	color->SetMatrix4f(color->GetTransformUniform(), glm::mat4(1));
	while (!quit) {
		SDL_PollEvent(&e);
		color->Use();
		va->Bind();
		glDrawArrays(GL_TRIANGLES, 0, va->GetVertexCount());
		glBindVertexArray(0);
		g_FrameMan.RenderPresent();
		g_FrameMan.RenderClear();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << std::hex << err << std::dec << std::endl;
		}
	}
	g_FrameMan.Destroy();
	SDL_Quit();
}
