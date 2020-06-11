#ifndef TRANSLATABLEMESSAGE_H
#define TRANSLATABLEMESSAGE_H

#include <Wt/WString.h>
#include <Wt/Json/Object.h>

#include <string>
#include <string_view>
#include <vector>

/**
 * A tree-like structure of messages that might need to be translated,
 * with arguments which themselves may need to be translated.
 *
 * I don't expect one of these will ever have more than 1 sub-level,
 * but it's useful for storing raw info that can be turned into a localized string.
 *
 * Converts back and forth to a json object that looks like:
 * {
 *      "message": string,
 *      "needsTranslating": boolean,
 *      "args": TranslatableMessage[]
 * }
 *
 * The idea being that you can translate a key like "${user} was logged out (${reason})"
 * where the root key has args, one arg needs translating and one does not.
 */
class TranslatableMessage
{
public:
    TranslatableMessage(std::string_view message, const bool needsTranslating = false);
    TranslatableMessage(const Wt::Json::Object& jsonObj);

    template<class... Args>
    void addArg(Args&&... _args)
    {
        args.emplace_back(std::forward<Args>(_args)...);
    }

    void addArg(TranslatableMessage&& arg)
    {
        args.emplace_back(std::move(arg));
    }

    void addArg(const TranslatableMessage& arg)
    {
        args.emplace_back(arg);
    }

    Wt::WString translate() const;
    Wt::Json::Object toJson() const;

    static constexpr const char* KEY_MESSAGE{"message"};
    static constexpr const char* KEY_NEEDSTRANSLATING{"needsTranslating"};
    static constexpr const char* KEY_ARGS{"args"};

private:
    std::string message;
    bool needsTranslating;
    std::vector<TranslatableMessage> args;
};

#endif  // TRANSLATABLEMESSAGE_H
