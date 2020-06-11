#include <Wt/Json/Array.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Value.h>
#include <Wt/WString.h>

#include <string>
#include <string_view>

#include "TranslatableMessage.h"

TranslatableMessage::TranslatableMessage(std::string_view message, const bool needsTranslating)
:
    message{message},
    needsTranslating{needsTranslating}
{}

TranslatableMessage::TranslatableMessage(const Wt::Json::Object& jsonObj)
:
    message{jsonObj.get(KEY_MESSAGE).toString().orIfNull("")},
    needsTranslating{jsonObj.get(KEY_NEEDSTRANSLATING).toBool().orIfNull(false)}
{
    // Take the "args" array from the object and pull our args off it
    const auto& rawArgs = jsonObj.get(KEY_ARGS).orIfNull(Wt::Json::Array());
    args.reserve(rawArgs.size());
    for (const auto& value : rawArgs)
    {
        // We expect the value to be an object we can use to build another TranslatableMessage
        const auto& obj = value.orIfNull(Wt::Json::Object({}));
        args.emplace_back(obj);
    }
}

Wt::WString TranslatableMessage::translate() const
{
    if (!needsTranslating)
        return message;

    auto translated = Wt::WString::tr(message);
    for (const auto& arg : args)
        translated.arg(arg.translate());

    return translated;
}

Wt::Json::Object TranslatableMessage::toJson() const
{
    // Build out json objects for all our children
    Wt::Json::Array jsonArgs;
    for (const auto& child : args)
        jsonArgs.push_back(Wt::Json::Value(child.toJson()));

    return {
        { KEY_MESSAGE, Wt::Json::Value(message) },
        { KEY_NEEDSTRANSLATING, Wt::Json::Value(needsTranslating) },
        { KEY_ARGS, Wt::Json::Value(jsonArgs) }
    };
}
