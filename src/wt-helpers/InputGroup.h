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

    template <typename Widget, typename ...Args>
    Widget* prepend(Args&& ...args)
    {
        return prependContainer->addNew<Widget>(std::forward<Args>(args)...);
    }

    template <typename Widget, typename ...Args>
    Widget* append(Args&& ...args)
    {
        return appendContainer->addNew<Widget>(std::forward<Args>(args)...);
    }

    Wt::WText* prependText(const Wt::WString& text)
    {
        auto span = prependContainer->addNew<Wt::WText>(text);
        span->setStyleClass("input-group-text");
        return span;
    }

    Wt::WText* appendText(const Wt::WString& text)
    {
        auto span = appendContainer->addNew<Wt::WText>(text);
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
