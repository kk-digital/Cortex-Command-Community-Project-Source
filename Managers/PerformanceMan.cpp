#include "PerformanceMan.h"
#include "MovableMan.h"
#include "FrameMan.h"
#include "AudioMan.h"
#include "Timer.h"

#include "GUI.h"
#include "SDLGUITexture.h"

#include "imgui.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::Clear() {
		m_ShowPerfStats = false;
		m_AdvancedPerfStats = true;
		m_CurrentPing = 0;
		m_FrameTimer = nullptr;
		m_MSPFs.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PerformanceMan::Initialize() {
		m_FrameTimer = std::make_unique<Timer>();
		m_Sample = 0;

		for (int counter = 0; counter < PerformanceCounters::PerfCounterCount; ++counter) {
			m_PerfData.at(counter).fill(std::chrono::steady_clock::duration(0));
			m_PerfPercentages.at(counter).fill(0);
		}
		m_PerfMeasureStart.fill(std::chrono::steady_clock::now());
		m_PerfMeasureStop.fill(std::chrono::steady_clock::now());

		// Set up performance counter's names
		m_PerfCounterNames.at(PerformanceCounters::SimTotal) = "Total";
		m_PerfCounterNames.at(PerformanceCounters::ActorsTravel) = "Act Travel";
		m_PerfCounterNames.at(PerformanceCounters::ParticlesTravel) = "Prt Travel";
		m_PerfCounterNames.at(PerformanceCounters::ActorsUpdate) = "Act Update";
		m_PerfCounterNames.at(PerformanceCounters::ParticlesUpdate) = "Prt Update";
		m_PerfCounterNames.at(PerformanceCounters::ActorsAIUpdate) = "Act AI";
		m_PerfCounterNames.at(PerformanceCounters::ActivityUpdate) = "Activity";

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::Update() {
		// Time and store the milliseconds per frame reading of the sim update to the buffer, and trim the buffer as needed
		m_MSPFs.push_back(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_FrameTime));//(static_cast<int>(m_FrameTimer->GetElapsedRealTimeMS()));
		while (m_MSPFs.size() > c_MSPFAverageSampleSize) {
			m_MSPFs.pop_front();
		}

		// Calculate the average milliseconds per frame over the last sampleSize frames
		m_MSPFAverage = std::chrono::microseconds::zero();
		for (const auto &mspf : m_MSPFs) {
			m_MSPFAverage += mspf;
		}
		m_MSPFAverage /= m_MSPFs.size();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::ResetFrameTimer() const {
		m_FrameTimer->Reset();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::StartPerformanceMeasurement(PerformanceCounters counter) {
		m_PerfMeasureStart.at(counter) = std::chrono::steady_clock::now();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::StopPerformanceMeasurement(PerformanceCounters counter) {
		m_PerfMeasureStop.at(counter) = std::chrono::steady_clock::now();
		AddPerformanceSample(counter, m_PerfMeasureStop.at(counter) - m_PerfMeasureStart.at(counter));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::NewPerformanceSample() {
		m_Sample++;
		if (m_Sample >= c_MaxSamples) { m_Sample = 0; }

		for (int counter = 0; counter < PerformanceCounters::PerfCounterCount; ++counter) {
			m_PerfData.at(counter).at(m_Sample) = std::chrono::steady_clock::duration::zero();
			m_PerfPercentages.at(counter).at(m_Sample) = 0;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::CalculateSamplePercentages() {
		for (int counter = 0; counter < PerformanceCounters::PerfCounterCount; ++counter) {
			int samplePercentage = static_cast<int>(static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_PerfData.at(counter).at(m_Sample)).count()) / static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_PerfData.at(counter).at(PerformanceCounters::SimTotal)).count()) * 100);
			m_PerfPercentages.at(counter).at(m_Sample) = samplePercentage;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint64_t PerformanceMan::GetPerformanceCounterAverage(PerformanceCounters counter) const {
		uint64_t totalPerformanceMeasurement = 0;
		int sample = m_Sample;
		for (int i = 0; i < c_Average; ++i) {
			totalPerformanceMeasurement += m_PerfData.at(counter).at(sample).count();
			if (sample == 0) { sample = c_MaxSamples; }
			sample--;
		}
		return totalPerformanceMeasurement / c_Average;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::Draw(/* SDLGUITexture &bitmapToDrawTo */) {
		if (m_ShowPerfStats) {
			// Time and store the milliseconds per frame reading of the drawing frame to the buffer, and trim the buffer as needed

			// m_MSPFs.push_back(static_cast<int>(m_FrameTimer->GetElapsedRealTimeMS()));
			m_MSPFs.push_back(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_FrameTime));
			m_FrameTime = std::chrono::steady_clock::now();
			m_FrameTimer->Reset();
			while (m_MSPFs.size() > c_MSPFAverageSampleSize) {
				m_MSPFs.pop_front();
			}
			// Calculate the average milliseconds per frame over the last sampleSize frames
			for(const auto &mspf : m_MSPFs){
				m_MSPFAverage += mspf;
			}
			m_MSPFAverage /= m_MSPFs.size();

			char str[128];

			double fps = 1.0 / (static_cast<double>(m_MSPFAverage.count()) / 1000000.0);
			float deltaTime = g_TimerMan.GetDeltaTimeMS();
			ImGui::Begin("Performance");
			ImGui::Text("FPS: %.0f", fps);
			ImGui::Text("uiFPS: %.0f", ImGui::GetIO().Framerate);
			ImGui::Text("uSPF: %zi", m_MSPFAverage.count());
			ImGui::Text("Time Scale: x%.2f ([1]-, [2]+)", g_TimerMan.IsOneSimUpdatePerFrame() ? g_TimerMan.GetSimSpeed() : g_TimerMan.GetTimeScale());
			ImGui::Text("Real to Sim Cap: %.2f ms ([3]-, [4]+)", g_TimerMan.GetRealToSimCap() * 1000.0F);
			ImGui::Text("DeltaTime: %.2f ms ([5]-, [6]+)", deltaTime);
			ImGui::Text("Particles: %li", g_MovableMan.GetParticleCount());
			ImGui::Text("Objects: %i", g_MovableMan.GetKnownObjectsCount());
			ImGui::Text("MOIDs: %i", g_MovableMan.GetMOIDCount());
			ImGui::Text("Sim Updates Since Last Drawn: %i", g_TimerMan.SimUpdatesSinceDrawn());
			if (g_TimerMan.IsOneSimUpdatePerFrame()) {
				ImGui::Text("ONE Sim Update Per Frame!");
			}

			std::vector<float> perfCounters(c_MaxSamples);
			for (size_t pc = 0; pc < PerformanceCounters::PerfCounterCount; ++pc) {
				for (size_t sample = 0; sample < m_PerfData.size(); ++sample){
					perfCounters[sample] = m_PerfData.at(pc).at(sample).count();
				}
				ImGui::PlotLines(m_PerfCounterNames.at(pc).c_str(), perfCounters.data(), perfCounters.size());
			}

			ImGui::End();

			// int totalPlayingChannelCount;
			// int realPlayingChannelCount;
			// if (g_AudioMan.GetPlayingChannelCount(&totalPlayingChannelCount, &realPlayingChannelCount)) {
			// 	std::snprintf(str, sizeof(str), "Sound Channels: %d / %d Real | %d / %d Virtual", realPlayingChannelCount, g_AudioMan.GetTotalRealChannelCount(), totalPlayingChannelCount - realPlayingChannelCount, g_AudioMan.GetTotalVirtualChannelCount());
			// }
			// g_FrameMan.GetLargeFont()->DrawAligned(&bitmapToDrawTo, c_StatsOffsetX, c_StatsHeight + 100, str, GUIFont::Left);

			// // If in split screen mode don't draw graphs because they don't fit anyway.
			// if (m_AdvancedPerfStats && g_FrameMan.GetScreenCount() == 1) { DrawPeformanceGraphs(bitmapToDrawTo); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PerformanceMan::DrawPeformanceGraphs(SDLGUITexture &bitmapToDrawTo) {
		CalculateSamplePercentages();

		char str[128];

		for (int pc = 0; pc < PerformanceCounters::PerfCounterCount; ++pc) {
			int blockStart = c_GraphsStartOffsetY + pc * c_GraphBlockHeight;

			g_FrameMan.GetLargeFont()->DrawAligned(&bitmapToDrawTo, c_StatsOffsetX, blockStart, m_PerfCounterNames.at(pc), GUIFont::Left);

			// Print percentage from PerformanceCounters::SimTotal
			int perc = static_cast<int>((static_cast<float>(GetPerformanceCounterAverage(static_cast<PerformanceCounters>(pc))) / static_cast<float>(GetPerformanceCounterAverage(PerformanceCounters::SimTotal)) * 100));
			std::snprintf(str, sizeof(str), "%%: %u", perc);
			g_FrameMan.GetLargeFont()->DrawAligned(&bitmapToDrawTo, c_StatsOffsetX + 60, blockStart, str, GUIFont::Left);

			// Print average processing time in ms
			std::snprintf(str, sizeof(str), "T: %lli", GetPerformanceCounterAverage(static_cast<PerformanceCounters>(pc)) / 1000);
			g_FrameMan.GetLargeFont()->DrawAligned(&bitmapToDrawTo, c_StatsOffsetX + 96, blockStart, str, GUIFont::Left);

			int graphStart = blockStart + c_GraphsOffsetX;

			// Draw graph backgrounds
			bitmapToDrawTo.DrawRectangle(c_StatsOffsetX, graphStart, c_MaxSamples, c_GraphHeight, c_GUIColorGray, true);
			bitmapToDrawTo.DrawLine(c_StatsOffsetX, graphStart + c_GraphHeight / 2, c_StatsOffsetX - 1 + c_MaxSamples, graphStart + c_GraphHeight / 2, c_GUIColorYellow);

			// Draw sample dots
			int peak = 0;
			int sample = m_Sample;
			for (int i = 0; i < c_MaxSamples; ++i) {
				// Show microseconds in graphs, assume that 33333 microseconds (one frame of 30 fps) is the highest value on the graph
				int value = std::clamp(static_cast<int>(static_cast<float>(m_PerfData.at(pc).at(sample).count()) / (1000000.0F / 30.0F) * 100.0F), 0, 100);
				int dotHeight = static_cast<int>(static_cast<float>(c_GraphHeight) / 100.0F * static_cast<float>(value));

				bitmapToDrawTo.SetPixel(c_StatsOffsetX - 1 + c_MaxSamples - i, graphStart + c_GraphHeight - dotHeight, c_GUIColorRed);
				peak = std::clamp(peak, 0, static_cast<int>(m_PerfData.at(pc).at(sample).count()));

				if (sample == 0) { sample = c_MaxSamples; }
				sample--;
			}

			// Print peak values
			g_FrameMan.GetLargeFont()->DrawAligned(&bitmapToDrawTo, c_StatsOffsetX + 130, blockStart, "Peak: " + std::to_string(peak / 1000), GUIFont::Left);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void PerformanceMan::DrawCurrentPing() const {
#ifdef NETWORK_ENABLED
		AllegroBitmap allegroBitmap(g_FrameMan.GetBackBuffer8());
		g_FrameMan.GetLargeFont()->DrawAligned(&allegroBitmap, g_FrameMan.GetBackBuffer8()->w - 25, g_FrameMan.GetBackBuffer8()->h - 14, "PING: " + std::to_string(m_CurrentPing), GUIFont::Right);
#endif
	}
}
