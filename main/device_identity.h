#ifndef _DEVICE_IDENTITY_H_
#define _DEVICE_IDENTITY_H_

#include <algorithm>
#include <cctype>
#include <string>

#include "system_info.h"

namespace DeviceIdentity {

inline std::string NormalizeDeviceId(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());
    for (unsigned char c : raw) {
        if (c == ':' || c == '-' || std::isspace(c)) {
            continue;
        }
        out.push_back(static_cast<char>(std::toupper(c)));
    }
    return out;
}

inline std::string GetNormalizedDeviceId() {
    return NormalizeDeviceId(SystemInfo::GetMacAddress());
}

}  // namespace DeviceIdentity

#endif  // _DEVICE_IDENTITY_H_
