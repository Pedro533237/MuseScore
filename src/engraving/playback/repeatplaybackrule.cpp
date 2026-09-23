#include "repeatplaybackrule.h"

#include <algorithm>

#include <QRegularExpression>
#include <QStringList>

using namespace mu::engraving;
using muse::String;

namespace {

void addPass(
    RepeatPlaybackRule& rule,
    const int pass)
{
    if (pass <= 0) {
        return;
    }

    const auto found =
        std::find(
            rule.passes.begin(),
            rule.passes.end(),
            pass);

    if (found == rule.passes.end()) {
        rule.passes.push_back(pass);
    }
}

bool containsAny(
    const QString& text,
    const QStringList& words)
{
    for (const QString& word : words) {
        if (text.contains(word)) {
            return true;
        }
    }

    return false;
}

}

bool RepeatPlaybackParser::parse(
    const String& source,
    RepeatPlaybackRule& rule)
{
    const QString text =
        source.toQString()
        .simplified()
        .toLower();

    const bool playOnly =
        containsAny(text, {
            QStringLiteral("play only"),
            QStringLiteral("play on"),
            QStringLiteral("only on pass"),
            QStringLiteral("only on repeat")
        });

    const bool mute =
        containsAny(text, {
            QStringLiteral("mute"),
            QStringLiteral("silent"),
            QStringLiteral("silence")
        });

    if (!playOnly && !mute) {
        return false;
    }

    rule = {};

    rule.mode =
        mute && !playOnly
            ? RepeatRuleMode::Mute
            : RepeatRuleMode::PlayOnly;

    const QRegularExpression numberExpression(
        QStringLiteral(
            R"((\d+)(?:\s*-\s*(\d+))?)"));

    QRegularExpressionMatchIterator iterator =
        numberExpression.globalMatch(text);

    while (iterator.hasNext()) {
        const QRegularExpressionMatch match =
            iterator.next();

        const int first =
            match.captured(1).toInt();

        if (!match.captured(2).isEmpty()) {
            const int last =
                match.captured(2).toInt();

            const int start =
                std::min(first, last);

            const int end =
                std::max(first, last);

            for (int pass = start;
                 pass <= end;
                 ++pass) {
                addPass(rule, pass);
            }
        } else {
            addPass(rule, first);
        }
    }

    return rule.isValid();
}
