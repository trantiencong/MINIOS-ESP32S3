#include "display_profile.h"

static const DisplayProfile PROFILE_240X240 = {
    .status_bar_height = 120,
    .status_bar_offset_y = -8,
    .content_pad_top = 20,
    .content_pad_all = 16,
    .idle_location_offset_y = 8,
    .idle_metrics_offset_y = 40,
    .emoji_box_size = 80,
    .emoji_offset_y = 0,
    .multiline_text_offset_y = 0,
};

static const DisplayProfile PROFILE_172X320 = {
    .status_bar_height = 120,
    .status_bar_offset_y = -24,
    .content_pad_top = 48,
    .content_pad_all = 16,
    .idle_location_offset_y = -6,
    .idle_metrics_offset_y = 24,
    .emoji_box_size = 72,
    .emoji_offset_y = 0,
    .multiline_text_offset_y = 0,
};

static const DisplayProfile PROFILE_240X320 = {
    .status_bar_height = 120,
    .status_bar_offset_y = -24,
    .content_pad_top = 48,
    .content_pad_all = 16,
    .idle_location_offset_y = -6,
    .idle_metrics_offset_y = 28,
    .emoji_box_size = 80,
    .emoji_offset_y = 0,
    .multiline_text_offset_y = 0,
};

static const DisplayProfile PROFILE_320X240 = {
    .status_bar_height = 120,
    .status_bar_offset_y = -8,
    .content_pad_top = 20,
    .content_pad_all = 16,
    .idle_location_offset_y = 8,
    .idle_metrics_offset_y = 40,
    .emoji_box_size = 80,
    .emoji_offset_y = 0,
    .multiline_text_offset_y = 0,
};

const DisplayProfile& GetDisplayProfile(int width, int height) {
    if (width == 240 && height == 240) {
        return PROFILE_240X240;
    }

    if (width == 172 && height == 320) {
        return PROFILE_172X320;
    }

    if (width == 240 && height == 320) {
        return PROFILE_240X320;
    }

    if (width == 320 && height == 240) {
        return PROFILE_320X240;
    }

    return PROFILE_240X240;
}