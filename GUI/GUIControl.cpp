#include "GUI.h"

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIControl::GUIControl() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Create(const std::string &Name, int X, int Y, int Width, int Height) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Create(GUIProperties *Props) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Destroy() {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Activate() {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::ChangeSkin(GUISkin *Skin) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::AddEvent(int Type, int Msg, int Data) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GUIControl::GetName() {
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GUIControl::GetToolTip() {
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GUIControl::GetID() const {
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIPanel * GUIControl::GetPanel() {
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::AddChild(GUIControl *Control) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<GUIControl *> * GUIControl::GetChildren() {
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GUIControl::Save(GUIWriter *W) {
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Move(int X, int Y) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::Resize(int Width, int Height) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::GetControlRect(int *X, int *Y, int *Width, int *Height) {

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GUIControl::GetAnchor() {
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::StoreProperties() {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::SetVisible(bool Visible) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GUIControl::GetVisible() {
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::SetEnabled(bool Enabled) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GUIControl::GetEnabled() {
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIControl * GUIControl::GetParent() {
	return m_ControlParent;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUIProperties * GUIControl::GetProperties() {
	return &m_Properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::ApplyProperties(GUIProperties *Props) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GUIControl::IsContainer() {
	return m_IsContainer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::RemoveChild(const std::string Name) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GUIControl::RemoveChildren() {
}
