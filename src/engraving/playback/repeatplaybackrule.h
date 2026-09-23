#pragma once

#include <vector>

#include "global/types/string.h"

namespace mu::engraving {

enum class RepeatRuleMode {
    PlayOnly,
    Mute
};

struct RepeatPlaybackRule
{
    RepeatRuleMode mode = RepeatRuleMode::PlayOnly;

    std::vector<int> passes;

    bool isValid() const
    {
        return !passes.empty();
    }

    bool containsPass(const int pass) const
    {
        for (const int allowedPass : passes) {
            if (allowedPass == pass) {
                return true;
            }
        }

        return false;
    }

    bool shouldPlay(const int pass) const
    {
        const bool listed =
            containsPass(pass);

        if (mode == RepeatRuleMode::PlayOnly) {
            return listed;
        }

        return !listed;
    }
};

class RepeatPlaybackParser
{
public:
    static bool parse(
        const muse::String& text,
        RepeatPlaybackRule& rule);
};

}
