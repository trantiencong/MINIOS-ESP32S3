#include "display_profile.h"

static const DisplayProfile PROFILE_240X240 = {
    .status_bar_height =
        120,  // Chiều cao vùng status/idle ở giữa màn hình (chứa đồng hồ, địa điểm, thời tiết)

    .status_bar_offset_y = -8,  // Dịch toàn bộ cụm status/idle theo trục dọc Y
                                // Số âm = kéo lên trên
                                // Số dương = đẩy xuống dưới

    .content_pad_top = 20,  // Khoảng đệm phía trên cho vùng nội dung chính bên dưới topbar

    .content_pad_all = 16,  // Khoảng đệm chung 4 phía cho vùng nội dung chính

    .idle_location_offset_y =
        8,  // Độ lệch dọc của dòng địa điểm (ví dụ: Hà Nội) so với tâm cụm idle
            // Tăng số = dòng địa điểm xuống thấp hơn
            // Giảm số = dòng địa điểm lên cao hơn

    .idle_metrics_offset_y = 40,  // Độ lệch dọc của cụm thời tiết / nhiệt độ / độ ẩm
                                  // Tăng số = cụm thời tiết xuống thấp hơn
                                  // Giảm số = cụm thời tiết lên cao hơn

    .top_bar_pad_left = 4,  // Khoảng cách lề trái bên trong topbar
                            // Dùng để kéo icon Wi-Fi tránh sát mép trái

    .top_bar_pad_right = 4,  // Khoảng cách lề phải bên trong topbar
                             // Dùng để kéo icon pin tránh sát mép phải

    .top_bar_offset_y = -2,  // Dịch toàn bộ topbar theo trục dọc Y
                             // Số âm = topbar lên cao hơn
                             // Số dương = topbar xuống thấp hơn

    .emoji_box_size = 80,  // Kích thước vùng chứa emoji / icon lớn ở giữa màn hình

    .emoji_offset_y = 0,  // Độ lệch dọc của emoji / icon lớn
                          // Âm = lên trên, dương = xuống dưới

    .multiline_text_offset_y = 0,  // Độ lệch dọc của text nhiều dòng trong các màn nội dung/chat
                                   // Dùng để cân lại vị trí khối chữ nếu cần
};

static const DisplayProfile PROFILE_172X320 = {
    .status_bar_height = 120,
    .status_bar_offset_y = -24,
    .content_pad_top = 48,
    .content_pad_all = 16,
    .idle_location_offset_y = -6,
    .idle_metrics_offset_y = 24,
    .top_bar_pad_left = 4,
    .top_bar_pad_right = 4,
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
    .top_bar_pad_left = 4,
    .top_bar_pad_right = 4,
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
    .top_bar_pad_left = 4,
    .top_bar_pad_right = 4,
    .emoji_box_size = 96,
    .emoji_offset_y = 18,
    .multiline_text_offset_y = 0,
};

static const DisplayProfile PROFILE_368X448 = {
    .status_bar_height = 176,
    .status_bar_offset_y = -8,
    .content_pad_top = 32,
    .content_pad_all = 20,
    .idle_location_offset_y = 16,
    .idle_metrics_offset_y = 58,
    .top_bar_pad_left = 20,
    .top_bar_pad_right = 20,
    .top_bar_offset_y = 10,
    .emoji_box_size = 112,
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

    if (width == 368 && height == 448) {
        return PROFILE_368X448;
    }
    return PROFILE_240X240;
}