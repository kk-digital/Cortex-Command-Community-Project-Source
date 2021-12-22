#include <SDL2/SDL.h>
#include "Managers/FrameMan.h"
#include "System/System.h"

#include "RTERenderer.h"
#include "GL/glew.h"
#include "ContentFile.h"


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
	tri.emplace_back(glm::vec2{0.0f, -1.0f}, glm::vec2(0.0f, 0.0f));
	tri.emplace_back(glm::vec2{1.0f, -1.0f}, glm::vec2(1.0f, 0.0f));
	tri.emplace_back(glm::vec2{0.0f, 0.0f}, glm::vec2(1.0f, 1.0f));
	va = std::make_shared<VertexArray>(std::move(tri));
	std::cout << va->GetVertexCount() << ": " << va->GetVAO() << std::endl;
	std::shared_ptr<Shader> color = std::make_shared<Shader>();
	if (!color->Compile("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32VertexColor.frag")) {
		quit = true;
	}
	color->Use();
	color->SetVector4f(color->GetColorUniform(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	color->SetMatrix4f(color->GetProjectionUniform(), glm::mat4(1));
	color->SetMatrix4f(color->GetTransformUniform(), glm::mat4(1));

	g_FrameMan.GetTextureShader(BitDepth::BPP32)->Use();
	g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetInt(g_FrameMan.GetTextureShader(BitDepth::BPP32)->GetTextureUniform(), 0);
	g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetVector4f(g_FrameMan.GetTextureShader(BitDepth::BPP32)->GetColorUniform(), glm::vec4(1.0f));

	std::shared_ptr<GLTexture> moo = ContentFile("Base.rte/Scenes/Terrains/Tutorial.png").GetAsTexture(ColorConvert::Preserve);

	std::cout << moo->GetBitDepth() << std::endl;
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Palette& pal = *g_FrameMan.GetDefaultPalette();
	for(size_t i = 0; i< Palette::PALETTESIZE; ++i) {
		std::cout << i << ": " << std::hex << (int)pal[i].r << " " <<(int) pal[i].g << " " << (int)pal[i].b << " " << (int)pal[i].a << std::endl;;
	}
	float time = 0.0;

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	g_FrameMan.GetDefaultPalette()->Bind();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_FrameMan.GetDefaultPalette()->GetTextureID(), 0);

	std::vector<uint32_t> pixels;
	pixels.resize(512);
	glReadPixels(0, 0, 256,1,GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for(size_t i = 0; i < 256; ++i){
		std::cout << std::dec << i << ": " << std::hex << pixels[i] << "\n";
	}
	std::cout << std::dec << std::endl;

	while (!quit) {
		SDL_PollEvent(&e);
		// color->Use();
		BlendModes::Blend.Enable();
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		g_FrameMan.GetTextureShader(BitDepth::Indexed8)->Use();
		moo->Bind();
		va->Bind();
		g_FrameMan.GetDefaultPalette()->Bind();
		glDrawArrays(GL_TRIANGLES, 0, va->GetVertexCount());
		glBindVertexArray(0);

		g_FrameMan.GetTextureShader(BitDepth::BPP32)->Use();
		g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetFloat("time",  time);
		time += 1;

		moo->setBlendMode(BlendModes::Blend);
		moo->render(g_FrameMan.GetRenderer(), {0,0});

		// RenderState state(glm::vec4(1.0f), va, glm::mat4(1.0f));
		// state.m_Shader = color;
		// state.m_BlendMode = BlendModes::None;
		// g_FrameMan.GetRenderer()->Draw(state);

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
