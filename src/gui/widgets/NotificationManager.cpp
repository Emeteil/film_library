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

    void NotificationManager::Push(NotificationType type,
                                   const std::string& message,
                                   float durationSec)
    {
        Notification n;
        n.type          = type;
        n.message       = message;
        n.remainingTime = durationSec;
        notifications.push_back(n);
    }

    void NotificationManager::Render()
    {
        if (notifications.empty()) return;

        float dt      = ImGui::GetIO().DeltaTime;
        float dw      = ImGui::GetIO().DisplaySize.x;
        float dh      = ImGui::GetIO().DisplaySize.y;
        float toastW  = 340.0f;
        float padR    = 18.0f;
        float padB    = 18.0f;
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

            float fade = 1.0f;
            float fadeIn  = 0.35f;
            float fadeOut = 0.45f;
            float total   = n.remainingTime + dt;
            if (n.remainingTime > (n.remainingTime + dt - fadeIn))
                fade = std::min(1.0f, (n.remainingTime + dt - n.remainingTime) / fadeIn);
            if (n.remainingTime < fadeOut)
                fade = n.remainingTime / fadeOut;
            if (fade < 0) fade = 0;
            if (fade > 1) fade = 1;

            ImGuiID nid = ImGui::GetID(n.message.c_str());
            ImGuiID ch  = ImHashStr("slide");
            iam_ease_desc ez = {iam_ease_out_back, 1.2f, 0, 0, 0};
            float slideX = iam_tween_float(nid, ch, 0.0f, 0.4f, ez,
                iam_policy_crossfade, dt, toastW + padR);

            float x = dw - toastW - padR + slideX;
            float y = dh - padB - (toastH + gap) * (alive + 1);

            ImU32 accent;
            switch (n.type)
            {
            case NotificationType::Success: accent = IM_COL32(60, 180, 80, 255);  break;
            case NotificationType::Warning: accent = IM_COL32(220, 160, 40, 255); break;
            case NotificationType::Error:   accent = IM_COL32(200, 60, 60, 255);  break;
            default:                        accent = IM_COL32(80, 140, 220, 255); break;
            }

            ImU32 bgCol  = IM_COL32(15, 14, 10, (int)(240 * fade));
            ImU32 bordCol = IM_COL32(60, 48, 16, (int)(200 * fade));

            dl->AddRectFilled(
                {x + 4, y + 4}, {x + toastW + 4, y + toastH + 4},
                IM_COL32(0, 0, 0, (int)(80 * fade)), 8.0f);

            dl->AddRectFilled({x, y}, {x + toastW, y + toastH}, bgCol, 8.0f);
            dl->AddRect({x, y}, {x + toastW, y + toastH}, bordCol, 8.0f, 0, 1.0f);

            dl->AddRectFilled({x, y + 6}, {x + 4, y + toastH - 6},
                ImU32((accent & 0x00FFFFFF) | ((int)(255 * fade) << 24)), 2.0f);

            const char* icon;
            switch (n.type)
            {
            case NotificationType::Success: icon = "✓"; break;
            case NotificationType::Warning: icon = "⚠"; break;
            case NotificationType::Error:   icon = "✗"; break;
            default:                        icon = "i"; break;
            }
            dl->AddText({x + 14, y + (toastH - 16) * 0.5f},
                ImU32((accent & 0x00FFFFFF) | ((int)(220 * fade) << 24)), icon);

            ImVec2 tsz = ImGui::CalcTextSize(n.message.c_str());

            std::string msg = n.message;
            if (tsz.x > toastW - 48)
                msg = n.message.substr(0, 42) + "…";
            dl->AddText({x + 32, y + (toastH - 16) * 0.5f},
                IM_COL32(210, 210, 215, (int)(240 * fade)), msg.c_str());

            float progress = n.remainingTime / (n.remainingTime + 0.001f);
            dl->AddRectFilled({x, y + toastH - 3}, {x + toastW, y + toastH},
                IM_COL32(30, 25, 12, (int)(180 * fade)), 0.0f, ImDrawFlags_RoundCornersBottom);
            dl->AddRectFilled({x, y + toastH - 3},
                {x + toastW * fade, y + toastH},
                ImU32((accent & 0x00FFFFFF) | ((int)(160 * fade) << 24)),
                0.0f, ImDrawFlags_RoundCornersBottom);

            ++alive;
        }
    }
}
