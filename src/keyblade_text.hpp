#pragma once

#include <mods/svc/message.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace kingdom::text {

inline constexpr std::array<uint16_t, 2> messageIds{0x018d, 0x008d};

// dMeter2Info's menu string helpers read BMG bytes directly and bypass
// MessageService. Post-hook their native output without touching the archive.
// Match the complete English title, including NUL, and never expand its buffer.
inline bool replacePlainTitle(uint32_t messageId, char* buffer, size_t capacity) noexcept {
    constexpr char original[] = "Ordon Sword";
    constexpr char replacement[] = "Kingdom Key";
    if (messageId != messageIds[0] || !buffer || capacity < sizeof(replacement) ||
        std::memcmp(buffer, original, sizeof(original)) != 0) {
        return false;
    }
    std::memcpy(buffer, replacement, sizeof(replacement));
    return true;
}

// Verified in the US common BMG: menu title and item acquisition message.
// Work on encoded bytes, not C strings: tags can contain embedded NUL bytes.
// The English phrases occur between tags; no tag argument is ever rewritten.
inline bool rewrite(const MessageOverrideContext& message, std::vector<uint8_t>& output) {
    output.clear();
    if (message.group != 0 || message.language != MESSAGE_LANGUAGE_ENGLISH ||
        (message.message_id != messageIds[0] && message.message_id != messageIds[1]) ||
        !message.original_text || !message.original_text_size ||
        message.original_text_size > 65536) {
        return false;
    }

    const auto* bytes = message.original_text;
    const size_t size = message.original_text_size;
    constexpr std::string_view oldTitle = "Ordon Sword";
    constexpr std::string_view oldPickup = "Ordon sword";
    constexpr std::string_view newName = "Kingdom Key";
    constexpr std::string_view oldDescription1 = "Rusl crafted this fine sword.";
    constexpr std::string_view newDescription1 = "A weapon to fight the darkness.";
    constexpr std::string_view oldDescription2 = "It is inlaid with Ordon goat horns.";
    constexpr std::string_view newDescription2 = "The Heartless fear it but are also drawn to it.";

    bool changed = false;
    size_t offset = 0;
    while (offset < size) {
        if (bytes[offset] == 0) {
            // There must be exactly one terminator outside any tag.
            if (offset + 1 != size || !changed) break;
            output.push_back(0);
            return true;
        }
        if (bytes[offset] == 0x1a) {
            if (size - offset < 2) break;
            const size_t tagSize = bytes[offset + 1];
            if (tagSize < 5 || tagSize > size - offset) break;
            // Match MessageService's color-tag validation as well as its bounds.
            if (bytes[offset + 2] == 0xff && bytes[offset + 3] == 0 &&
                bytes[offset + 4] == 0 && tagSize != 6 && tagSize != 9 && tagSize != 13) {
                break;
            }
            output.insert(output.end(), bytes + offset, bytes + offset + tagSize);
            offset += tagSize;
            continue;
        }

        // Each phrase contains no control bytes, so a full match cannot cross a tag.
        if (size - offset >= oldTitle.size() &&
            std::memcmp(bytes + offset, oldTitle.data(), oldTitle.size()) == 0) {
            output.insert(output.end(), newName.begin(), newName.end());
            offset += oldTitle.size();
            changed = true;
        } else if (size - offset >= oldPickup.size() &&
                   std::memcmp(bytes + offset, oldPickup.data(), oldPickup.size()) == 0) {
            output.insert(output.end(), newName.begin(), newName.end());
            offset += oldPickup.size();
            changed = true;
        } else if (size - offset >= oldDescription1.size() &&
                   std::memcmp(bytes + offset, oldDescription1.data(), oldDescription1.size()) == 0) {
            output.insert(output.end(), newDescription1.begin(), newDescription1.end());
            offset += oldDescription1.size();
            changed = true;
        } else if (size - offset >= oldDescription2.size() &&
                   std::memcmp(bytes + offset, oldDescription2.data(), oldDescription2.size()) == 0) {
            output.insert(output.end(), newDescription2.begin(), newDescription2.end());
            offset += oldDescription2.size();
            changed = true;
        } else {
            output.push_back(bytes[offset++]);
        }
    }
    output.clear();
    return false;
}

class System {
public:
    System() = default;
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) = delete;
    System& operator=(System&&) = delete;

    ModResult initialize(ModContext* context, const MessageService* service) {
        shutdown();
        if (!context || !service || !service->override_message_fn || !service->remove_override)
            return MOD_INVALID_ARGUMENT;
        context_ = context;
        service_ = service;
        for (size_t index = 0; index < messageIds.size(); ++index) {
            const ModResult result = service_->override_message_fn(context_, 0, messageIds[index],
                MESSAGE_LANGUAGE_ENGLISH, callback, this, &handles_[index]);
            if (result != MOD_OK || !handles_[index]) {
                shutdown();
                return result == MOD_OK ? MOD_ERROR : result;
            }
        }
        return MOD_OK;
    }

    void shutdown() {
        if (context_ && service_) {
            for (auto& handle : handles_) {
                if (handle) service_->remove_override(context_, handle);
                handle = 0;
            }
        }
        context_ = nullptr;
        service_ = nullptr;
        scratch_.clear();
    }

private:
    static bool callback(ModContext*, const MessageOverrideContext* message,
                         MessageTextData* output, void* userData) noexcept {
        if (!message || !output || !userData) return false;
        auto& self = *static_cast<System*>(userData);
        try {
            if (!rewrite(*message, self.scratch_)) return false;
            // MessageService copies this buffer before invoking another callback.
            *output = {self.scratch_.data(), self.scratch_.size()};
            return true;
        } catch (...) {
            // A failed allocation must not escape the host's C callback boundary.
            self.scratch_.clear();
            return false;
        }
    }

    ModContext* context_ = nullptr;
    const MessageService* service_ = nullptr;
    std::array<MessageOverrideHandle, messageIds.size()> handles_{};
    std::vector<uint8_t> scratch_;
};

} // namespace kingdom::text
