#ifndef DISPLAY_PROFILE_H
#define DISPLAY_PROFILE_H

struct DisplayProfile {
    int status_bar_height;
    int status_bar_offset_y;

    int content_pad_top;
    int content_pad_all;

    int idle_location_offset_y;
    int idle_metrics_offset_y;

    int top_bar_pad_left;
    int top_bar_pad_right;
    int top_bar_offset_y;

    int emoji_box_size;
    int emoji_offset_y;
    int multiline_text_offset_y;
};

const DisplayProfile& GetDisplayProfile(int width, int height);

#endif  // DISPLAY_PROFILE_H