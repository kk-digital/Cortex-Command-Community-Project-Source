#include <SDL2/SDL.h>
#include "Managers/FrameMan.h"
#include "System/System.h"

#include "RTERenderer.h"
#include "GL/glew.h"
#include "ContentFile.h"
#include "MOSRotating.h"

#include "Constants.h"
#include "GraphicalPrimitive.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

using namespace RTE;

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
	g_FrameMan.CreateWindowAndRenderer();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(g_FrameMan.GetWindow(), g_FrameMan.GetContext());
	ImGui_ImplOpenGL3_Init("#version 330 core");

	System::Initialize();
	g_FrameMan.Initialize();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	std::shared_ptr<VertexArray> va;
	std::vector<Vertex> tri;
	tri.emplace_back(glm::vec2{0.0f, -1.0f}, glm::vec2(0.0f, 0.0f));
	tri.emplace_back(glm::vec2{1.0f, -1.0f}, glm::vec2(1.0f, 0.0f));
	tri.emplace_back(glm::vec2{0.0f, 0.0f}, glm::vec2(1.0f, 1.0f));
	va = std::make_shared<VertexArray>(std::move(tri));
	std::cout << va->GetVertexCount() << ": " << va->GetVAO() << std::endl;
	std::shared_ptr<Shader> color = std::make_shared<Shader>();
	if (!color->Compile("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32VertexColor.frag")) {
		std::exit(-1);
	}
	color->Use();
	color->SetVector4f(color->GetColorUniform(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	color->SetMatrix4f(color->GetProjectionUniform(), glm::mat4(1));
	color->SetMatrix4f(color->GetTransformUniform(), glm::mat4(1));

	g_FrameMan.GetTextureShader(BitDepth::BPP32)->Use();
	g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetInt(g_FrameMan.GetTextureShader(BitDepth::BPP32)->GetTextureUniform(), 0);
	g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetVector4f(g_FrameMan.GetTextureShader(BitDepth::BPP32)->GetColorUniform(), glm::vec4(1.0f));

	std::shared_ptr<GLTexture> moo = ContentFile("Base.rte/Scenes/Terrains/Tutorial.png").GetAsTexture(ColorConvert::Preserve);
	std::shared_ptr<GLTexture> bran = ContentFile("Base.rte/Actors/Brains/Brainbot/MobileBrain000.png").GetAsTexture();
	std::shared_ptr<GLTexture> booty = ContentFile("Base.rte/Actors/Wildlife/MegaCrab/BigCrabBodyA000.png").GetAsTexture();

	ContentFile mooove("Base.rte/Craft/Dropships/HullA.png");
	MOSRotating rotate{};
	rotate.Create(mooove, 1, 100.f, Vector(50.0f, 50.f), Vector(0.f, 0.f), 100000uL);

	std::cout << moo->GetBitDepth() << std::endl;
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Palette &pal = *g_FrameMan.GetDefaultPalette();
	for (size_t i = 0; i < Palette::PALETTESIZE; ++i) {
		std::cout << i << ": " << std::hex << (int)pal[i].r << " " << (int)pal[i].g << " " << (int)pal[i].b << " " << (int)pal[i].a << std::endl;
		;
	}
	float time = 0.0;

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	g_FrameMan.GetDefaultPalette()->Bind();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_FrameMan.GetDefaultPalette()->GetTextureID(), 0);

	std::vector<uint32_t> pixels;
	pixels.resize(512);
	glReadPixels(0, 0, 256, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (size_t i = 0; i < 256; ++i) {
		std::cout << std::dec << i << ": " << std::hex << pixels[i] << "\n";
	}
	std::cout << std::dec << std::endl;
	GLint tex1d{};

	g_FrameMan.GetDefaultPalette()->Bind();
	bool applyPal{false};
	GLint texMaxSize{};
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texMaxSize);
	std::cout << "Max texture size " << texMaxSize << std::endl;

	bool quit{false};
	bool showDemoWindow{true};
	int resolutionMultiplier { 50 };
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT)
				quit = true;
		}


		g_FrameMan.GetRenderer()->DrawClear(glm::vec4(0.0f));

		color->Use();
		BlendModes::Blend.Enable();
		// glBlendEquation(GL_FUNC_ADD);
		// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// g_FrameMan.GetTextureShader(BitDepth::Indexed8)->Use();
		// moo->Bind();
		// va->Bind();
		// glDrawArrays(GL_TRIANGLES, 0, va->GetVertexCount());
		// glBindVertexArray(0);

		g_FrameMan.GetTextureShader(BitDepth::BPP32)->Use();
		g_FrameMan.GetTextureShader(BitDepth::BPP32)->SetFloat("time", time);
		time += 1;
		bran->setColorMod(glm::vec3(glm::sin(glm::radians(time)), glm::cos(glm::radians(time)), glm::sin(glm::radians(time) + 0.5))); //, glm::cos(time), glm::sin(time + 3.141/2)));
		bran->render(g_FrameMan.GetRenderer(), glm::vec2{fmod(time * 0.5, g_FrameMan.GetResX()), 10});
		booty->setAlphaMod(fmod(time, 255));
		booty->render(g_FrameMan.GetRenderer(), {15 + booty->GetW() * 5, 20 + booty->GetH() * 5}, 8.0 * glm::radians(time), glm::vec2(0.0f), 5.0f * glm::vec2(glm::sin(glm::radians(time)), glm::cos(glm::radians(time))));

		moo->setBlendMode(BlendModes::Blend);
		moo->render(g_FrameMan.GetRenderer(), {0, 0});

		// RenderState state(glm::vec4(1.0f), va, glm::mat4(1.0f));
		// state.m_Shader = color;
		// state.m_BlendMode = BlendModes::None;
		// g_FrameMan.GetRenderer()->Draw(state);

		rotate.SetRotAngle(glm::radians(-time) * 5);
		rotate.SetPos(Vector(150.0f, 50.0f));
		rotate.Draw(g_FrameMan.GetRenderer(), Vector(), g_DrawTrans, true, 192);
		rotate.SetPos(Vector(150.0f, 150.0f));
		rotate.SetRotAngle(glm::radians(time) * 5);
		rotate.Draw(g_FrameMan.GetRenderer(), Vector(), g_DrawWhite, true);


		if (!applyPal) {
			g_FrameMan.GetDefaultPalette()->Bind();
			applyPal = true;
		}

		LinePrimitive(-1, {20.0f, 200.0f}, {150.0f, 200.0f}, g_WhiteColor).Draw(g_FrameMan.GetRenderer());
		CirclePrimitive(-1, {150.0f, 280.0f}, 50, g_WhiteColor).Draw(g_FrameMan.GetRenderer());
		CircleFillPrimitive(-1, {150.0f, 280.0f}, 30, g_YellowGlowColor).Draw(g_FrameMan.GetRenderer());
		CircleFillPrimitive(-1, {150.0f, 280.0f}, 5, 13).Draw(g_FrameMan.GetRenderer());
		BoxPrimitive(-1, {20.0f, 230.0f}, {130.0f, 280.0f}, 113).Draw(g_FrameMan.GetRenderer());
		BoxFillPrimitive(-1, {30.0f, 260.0f}, {100.0f, 250.0f}, g_WhiteColor).Draw(g_FrameMan.GetRenderer());

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << std::hex << err << std::dec << std::endl;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Info");

			ImGui::End();
		}

		if(showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		g_FrameMan.RenderPresent();
	}
	g_FrameMan.Destroy();
	SDL_Quit();
}
