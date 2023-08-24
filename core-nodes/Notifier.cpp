/******************************************************************************************
*                                                                                         *
*    Notifier                                                                             *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "Notifier.hpp"

Notif::Status Notif::GetStatus() const
{
    const auto elapsedTime = ImGui::GetTime() - notifCreateTime; // [s]
    if (elapsedTime > fadeInTime + onTime + fadeOutTime) { return Status::OFF; }
    else if (elapsedTime > fadeInTime + onTime) { return Status::FADEOUT; }
    else if (elapsedTime > fadeInTime) { return Status::ON; }
    else { return Status::FADEIN; }
}

std::string Notif::GetTypeName() const
{
    if (type == Type::SUCCESS) { return "Success"; }
    else if (type == Type::WARNING) { return "Warning"; }
    else if (type == Type::ERROR) { return "Error"; }
    else if (type == Type::INFO) { return "Info"; }
    else { return ""; }
}

float Notif::GetFadeValue() const
{
    const auto status = GetStatus();
    const auto elapsedTime = static_cast<float>(ImGui::GetTime() - notifCreateTime); // [s]
    if (status == Status::FADEIN)
    {
        return (elapsedTime / fadeInTime) * opacity;
    }
    else if (status == Status::FADEOUT)
    {
        return (1.0f - ((elapsedTime - fadeInTime - onTime) / fadeOutTime)) * opacity;
    }
    return 1.0f * opacity;
}

ImColor Notif::GetColor() const
{
    if (type == Type::SUCCESS) { return ImColor(0.0f, 0.5f, 0.0f, 1.0f); }
    else if (type == Type::WARNING) { return ImColor(0.5f, 0.5f, 0.0f, 1.0f); }
    else if (type == Type::ERROR) { return ImColor(0.5f, 0.0f, 0.0f, 1.0f); }
    else if (type == Type::INFO) { return ImColor(0.0f, 0.3f, 0.5f, 1.0f); }
    else { return ImColor(1.0f, 1.0f, 1.0f, 1.0f); }
}

Notif::Notif(Type type, const std::string& title, const std::string& content, float onTime) :
    type(type), title(title), content(content), onTime(onTime)
{
    notifCreateTime = static_cast<float>(ImGui::GetTime());
}

void Notifier::DrawNotifications()
{
    float height = 0.0f;
    for (int i = 0; i < notifs.size(); i++)
    {
        auto notif = &notifs[i];
        if (notif->GetStatus() == Notif::Status::OFF)
        {
            RemoveNotif(i);
            continue;
        }
        std::string notifName = "Notif" + std::to_string(i);
        const auto fadeValue = notif->GetFadeValue();
        ImGui::SetNextWindowBgAlpha(fadeValue);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, backgroundColor);
        const auto rightCorner = ImVec2(ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y);
        ImGui::SetNextWindowPos(ImVec2(rightCorner.x - xPadding, rightCorner.y - yPadding - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        ImGui::Begin(notifName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::PushTextWrapPos(ImGui::GetMainViewport()->Size.x * wrapRatio);
        if (const auto icon = notif->GetIcon(); icon.empty() == false)
        {
            auto iconColor = notif->GetColor();
            iconColor.Value.w = fadeValue;
            ImGui::TextColored(iconColor, icon.c_str());
        }
        if (const auto title = notif->GetTitle(); title.empty() == false)
        {
            //if (icon.empty() == false) { ImGui::SameLine(); }
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), title.c_str());
        }
        else if (const auto typeName = notif->GetTypeName(); typeName.empty() == false)
        {
            //if (icon.empty() == false) { ImGui::SameLine(); }
            ImGui::TextColored(notif->GetColor(), typeName.c_str());
        }
        if (const auto content = notif->GetContent(); content.empty() == false)
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), content.c_str());
        }
        ImGui::PopTextWrapPos();
        height += ImGui::GetWindowHeight() + yMessagePadding;
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
    heightNotifs = height;
}

void Notifier::AddNotif(const Notif & notif)
{
    if (heightNotifs >= ImGui::GetMainViewport()->Size.y * 0.8f)
    {
        RemoveNotif(0);
    }
    notifs.push_back(notif);
}
