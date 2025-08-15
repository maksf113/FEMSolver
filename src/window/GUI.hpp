#pragma once
#pragma once
#include <string>
#include <iomanip>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include "data_structures/Array.hpp"

#include "graphics/ColorMaps.hpp"
#include "graphics/Renderer.hpp"

class Camera;

class GUI
{
public:
	GUI(GLFWwindow* win);
	~GUI();
	GUI(const GUI&) = delete;
	GUI(GUI&&) = delete;
	GUI& operator=(const GUI&) = delete;
	GUI& operator=(GUI&&) = delete;
	void createFrame(Renderer& renderer);
	void draw();
};

inline GUI::GUI(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	const char* glslVersion = "#version 450 core";
	ImGui_ImplOpenGL3_Init(glslVersion);
	ImGui::StyleColorsDark();
}

inline GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

inline void GUI::createFrame(Renderer& renderer)
{
	// init
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// window positioning
	const float PADDING = 10.0f;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 workPos = viewport->WorkPos;
	ImVec2 workSize = viewport->WorkSize;
	ImVec2 windowPos, windowPosPivot;
	windowPos.x = workPos.x + workSize.x - PADDING;
	windowPos.y = workPos.y + PADDING;
	windowPosPivot.x = 1.0f; // right-align
	windowPosPivot.y = 0.0f; // top-align
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
	ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Once); // auto resize
	ImGui::SetNextWindowBgAlpha(0.8f);
	// imgui window
	ImGui::Begin("Scale");
	// color scale
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	float barWidth = 20.0f;
	int barHeight = 300; // in pexels

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	float valueRangle = renderer.scaleMax() - renderer.scaleMin();
	// calculate all label strings and find max width for allignment
	Array<std::string> labels;
	float maxTextWidth = 0.0f;
	for (int i = 0; i <= renderer.labelCount(); i++)
	{
		float t = static_cast<float>(i) / renderer.labelCount();
		double value = renderer.scaleMin() + (1.0f - t) * valueRangle;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << value;
		labels.pushBack(ss.str());
		float currentWidth = ImGui::CalcTextSize(labels.back().c_str()).x;
		if (currentWidth > maxTextWidth)
		{
			maxTextWidth = currentWidth;
		}
	}
	// draw color scale bar 
	ImVec2 barStart = ImVec2(cursorPos.x + maxTextWidth + PADDING, cursorPos.y);
	ImVec2 barEnd = ImVec2(barStart.x + barWidth, barStart.y + barHeight);
	int divisions = 100;
	float step = (barEnd.y - barStart.y) / divisions;
	for (int i = 0; i < divisions; i++)
	{
		float t = static_cast<float>(i) / (divisions - 1);
		Color color = ColorMaps::getColor(1.0f - t, renderer.getPalette());
		ImVec4 col = ImVec4(color.r, color.g, color.b, 1.0f);
		
		drawList->AddRectFilled(
			ImVec2(barStart.x, barStart.y + i * step), 
			ImVec2(barEnd.x, barStart.y + (i + 1) * step),
			ImGui::GetColorU32(col)
		);
	}
	// draw value labels and tick marks
	for (int i = 0; i <= renderer.labelCount(); i++)
	{
		float t = static_cast<float>(i) / renderer.labelCount();
		float textWidth = ImGui::CalcTextSize(labels[i].c_str()).x;
		float yPos = barStart.y + t * barHeight;
		// calculate x position to right-allign the text
		float xPos = cursorPos.x + maxTextWidth - textWidth;
		// calculate y position
		if (i == 0)
		{
			yPos = barStart.y;
		}
		else if (i == renderer.labelCount())
		{
			yPos = barEnd.y - ImGui::GetTextLineHeight();
		}
		else
		{
			yPos -= ImGui::GetTextLineHeight() / 2.0f;
		}
		// draw tex and tick mark
		drawList->AddText(
			ImVec2(xPos, yPos), 
			IM_COL32(255, 255, 255, 255),
			labels[i].c_str()
		);
		drawList->AddLine(
			ImVec2(barStart.x - 5, barStart.y + t * barHeight), 
			ImVec2(barEnd.x, barStart.y + t * barHeight),
			IM_COL32(128, 128, 128, 64), 
			1.0f
		);
	}
	
	// advance the cursor past the color bar
	ImGui::Dummy(ImVec2(maxTextWidth + PADDING + barWidth, barHeight));

	// 2D/3D button
	ImGui::Separator();
	ImGui::Text("Camera Mode");
	if (ImGui::Button("2D / 3D", ImVec2(-1, 0)))
		renderer.getCamera().toggleMode();
	// scale divisions slider
	ImGui::Separator();
	ImGui::Text("Scale Divisions");
	if (ImGui::SliderInt("##Scale Divisions", &renderer.getLabelCount(), 1, 10))
	{
		renderer.createGrid();
	}
	// display options
	ImGui::Separator();
	ImGui::Text("Display Options");
	ImGui::Checkbox("Draw Plot", &renderer.getDrawPlot());
	if (renderer.getDrawPlot())
	{
		ImGui::Indent(10.0f); // move to the righ
		ImGui::Checkbox("Wireframe", &renderer.getDrawWireframe());
		ImGui::Checkbox("Isolines", &renderer.getDrawIsolines());
		ImGui::Unindent(10.0f);
	}
	ImGui::Checkbox("Draw Mesh", &renderer.getDrawMesh());
	ImGui::Checkbox("Draw Grid", &renderer.getDrawGrid());
	// color palette
	ImGui::Separator();

	const char* paletteNames[] = { "Viridis", "Inferno", "Magma", "Plasma", "Cividis", "Coolwarm", "Jet" };
	int currentItem = static_cast<int>(renderer.getPalette());
	// combobox
	if(ImGui::Combo("##ColorPalette", &currentItem, paletteNames, IM_ARRAYSIZE(paletteNames)))
	{
		renderer.getPalette() = static_cast<ColorPalette>(currentItem);
	}
	ImGui::End();
}

void GUI::draw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}