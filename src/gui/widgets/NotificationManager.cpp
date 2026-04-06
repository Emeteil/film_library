/// @file NotificationManager.cpp
/// @brief Toast-notifications

#include "gui/widgets/NotificationManager.h"

#include <imgui.h>
#include <im_anim.h>
#include <imgui_internal.h>

#include <cmath>
#include <cstring>

namespace FilmLibrary
{
    NotificationManager& NotificationManager::Instance()
    {
        static NotificationManager inst;
        return inst;
    }

    void NotificationManager::Push(NotificationType type, const std::string& message, float durationSec)
    {
        Notification n;
        n.id = nextId++;
        n.type = type;
        n.message = message;
        n.duration = durationSec;
        n.remainingTime = durationSec;
        n.currentY = -1.0f;
        notifications.push_back(n);
    }

    void NotificationManager::Render()
    {
        if (notifications.empty()) return;

        float dt = ImGui::GetIO().DeltaTime;
        float dw = ImGui::GetIO().DisplaySize.x;
        float dh = ImGui::GetIO().DisplaySize.y;
        float toastW = 340.0f;
        float padR = 18.0f;
        float padB = 18.0f;
        float toastH  = 56.0f;
        float gap     = 8.0f;

        ImDrawList* dl = ImGui::GetForegroundDrawList();

        int alive = 0;
        for (int i = (int)notifications.size() - 1; i >= 0; --i)
        {
            Notification& n = notifications[i];
            n.remainingTime -= dt;
            if (n.remainingTime <= 0.0f)
            {
                notifications.erase(notifications.begin() + i);
                continue;
            }

            float fadeIn  = 0.35f;
            float fadeOut = 0.45f;
            float fade = 1.0f;
            
            if (n.remainingTime > (n.duration - fadeIn))
            {
                fade = (n.duration - n.remainingTime) / fadeIn;
            }
            else if (n.remainingTime < fadeOut)
            {
                fade = n.remainingTime / fadeOut;
            }
            
            if (fade < 0.0f) fade = 0.0f;
            if (fade > 1.0f) fade = 1.0f;

            float ease = 1.0f - (1.0f - fade) * (1.0f - fade);
            float slideX = (1.0f - ease) * (toastW + padR);

            float targetY = dh - padB - (toastH + gap) * (alive + 1);
            if (n.currentY < 0.0f)
            {
                n.currentY = targetY;
            }
            else
            {
                n.currentY += (targetY - n.currentY) * (1.0f - expf(-18.0f * dt));
            }

            float x = dw - toastW - padR + slideX;
            float y = n.currentY;

            ImU32 accent;
            switch (n.type)
            {
                case NotificationType::Success:
                    accent = IM_COL32(46, 204, 113, 255);
                    break;
                case NotificationType::Warning:
                    accent = IM_COL32(241, 196, 15, 255);
                    break;
                case NotificationType::Error:
                    accent = IM_COL32(231, 76, 60, 255);
                    break;
                default:
                    accent = IM_COL32(52, 152, 219, 255);
                    break;
            }

            int bgAlpha = (int)(250 * fade);
            int bordAlpha = (int)(150 * fade);
            int textAlpha = (int)(255 * fade);

            ImU32 bgCol = IM_COL32(40, 40, 40, bgAlpha);
            ImU32 bordCol = IM_COL32(60, 60, 60, bordAlpha);

            dl->AddRectFilled(
                {x + 4, y + 4}, {x + toastW + 4, y + toastH + 4},
                IM_COL32(0, 0, 0, (int)(100 * fade)), 8.0f);

            dl->AddRectFilled({x, y}, {x + toastW, y + toastH}, bgCol, 8.0f);
            dl->AddRect({x, y}, {x + toastW, y + toastH}, bordCol, 8.0f, 0, 1.0f);

            dl->AddRectFilled({x, y + 8}, {x + 4, y + toastH - 8},
                ImU32((accent & 0x00FFFFFF) | (textAlpha << 24)), 2.0f);

            const char* icon;
            switch (n.type)
            {
            case NotificationType::Success: icon = u8"\xef\x8c\xbd"; break;
            case NotificationType::Warning: icon = "[!]"; break;
            case NotificationType::Error:   icon = "[X]"; break;
            default:                        icon = "[i]"; break;
            }

            if (n.type == NotificationType::Success) icon = "[OK]";

            dl->AddText({x + 12, y + (toastH - 16) * 0.5f},
                ImU32((accent & 0x00FFFFFF) | (textAlpha << 24)), icon);

            std::string msg = n.message;
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            ImVec2 tsz = ImGui::CalcTextSize(msg.c_str());
            if (tsz.x > toastW - 48)
            {
                msg = n.message.substr(0, 42) + "...";
            }
            ImGui::PopFont();
            
            dl->AddText({x + 42, y + (toastH - 16) * 0.5f},
                IM_COL32(230, 230, 230, textAlpha), msg.c_str());

            float progress = n.remainingTime / (n.duration + 0.001f);
            dl->AddRectFilled({x, y + toastH - 3}, {x + toastW, y + toastH},
                IM_COL32(20, 20, 20, (int)(200 * fade)), 0.0f, ImDrawFlags_RoundCornersBottom);
            if (progress > 0.0f)
            {
                dl->AddRectFilled({x, y + toastH - 3},
                    {x + toastW * progress, y + toastH},
                    ImU32((accent & 0x00FFFFFF) | ((int)(255 * fade) << 24)),
                    0.0f, ImDrawFlags_RoundCornersBottom);
            }

            ++alive;
        }
    }
}
