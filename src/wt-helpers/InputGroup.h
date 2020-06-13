#ifndef INPUTGROUP_H
#define INPUTGROUP_H

#include <Wt/WCompositeWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>

#include <memory>
#include <utility>

namespace Wt {
    class WString;
}

// WT implementation of a Bootstrap input group as described at https://getbootstrap.com/docs/4.3/components/input-group/
// T is the main/central input, and you prepend/append other controls on either side.
template <typename T>
class InputGroup : public Wt::WCompositeWidget
{
public:
    template <typename ...Args>
    InputGroup(Args&& ...args)
    :
        Wt::WCompositeWidget(std::make_unique<Wt::WContainerWidget>())
    {
        setStyleClass("input-group");

        auto impl = static_cast<Wt::WContainerWidget*>(implementation());
        prependContainer = impl->addNew<Wt::WContainerWidget>();
        prependContainer->setStyleClass("input-group-prepend");

        mainInput = impl->addNew<T>(std::forward<Args>(args)...);
        mainInput->setStyleClass("form-control");

        appendContainer = impl->addNew<Wt::WContainerWidget>();
        appendContainer->setStyleClass("input-group-append");
    }

    template <typename Widget, typename ...Args, typename std::enable_if<!std::is_same<Widget, Wt::WText>::value, Widget>::type* = nullptr>
    Widget* prepend(Args&& ...args)
    {
        return prependContainer->addNew<Widget>(std::forward<Args>(args)...);
    }

    template <typename Widget, typename ...Args, typename std::enable_if<!std::is_same<Widget, Wt::WText>::value, Widget>::type* = nullptr>
    Widget* append(Args&& ...args)
    {
        return appendContainer->addNew<Widget>(std::forward<Args>(args)...);
    }

    // Overloads for WText prepend/append to add the input-group-text class
    template<typename Widget, typename std::enable_if<std::is_same<Widget, Wt::WText>::value, Widget>::type* = nullptr>
    Widget* prepend(const Wt::WString& text)
    {
        auto span = prependContainer->addNew<Widget>(text);
        span->setStyleClass("input-group-text");
        return span;
    }

    template<typename Widget, typename std::enable_if<std::is_same<Widget, Wt::WText>::value, Widget>::type* = nullptr>
    Widget* append(const Wt::WString& text)
    {
        auto span = appendContainer->addNew<Widget>(text);
        span->setStyleClass("input-group-text");
        return span;
    }

    T* getMainInput() { return mainInput; }

private:
    T* mainInput;
    Wt::WContainerWidget* prependContainer;
    Wt::WContainerWidget* appendContainer;
};


#endif  // INPUTGROUP_H
