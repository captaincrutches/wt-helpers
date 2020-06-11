#ifndef TRANSIENTWIDGET_H
#define TRANSIENTWIDGET_H

#include <Wt/WContainerWidget.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace Wt {
    class WWidget;
}

// A widget that may or may not be actually present in the DOM at any given time, but that we still want to keep track of.
template <class T>
class TransientWidget final
{
    static_assert(std::is_convertible<T, Wt::WWidget>::value, "Transient widget must be a WWidget");

public:
    template<class... Args>
    TransientWidget(Wt::WContainerWidget* _parent, Args&&... args)
      :
        parent {_parent},
        uniquePtr {std::make_unique<T>(std::forward<Args>(args)...)}
    {}

    TransientWidget(Wt::WContainerWidget* _parent, std::unique_ptr<T> ptr)
    :
        parent {_parent},
        uniquePtr {std::move(ptr)}
    {}

    T* operator->() const
    {
        return ptr ? ptr : uniquePtr.get();
    }

    void setShown(bool _shown)
    {
        if (alwaysInDom)
            ptr->setHidden(!shown);
        else
            setInDom(shown);

        shown = _shown;
    }

    void setAlwaysInDom(bool _alwaysInDom)
    {
        alwaysInDom = _alwaysInDom;
        setInDom(shown || alwaysInDom);
    }

private:
    Wt::WContainerWidget* parent;

    std::unique_ptr<T> uniquePtr;
    T* ptr = nullptr;

    bool alwaysInDom = false;
    bool currentlyInDom = false;
    bool shown = false;

    void setInDom(bool inDom);
};

template <class T>
inline void TransientWidget<T>::setInDom(bool inDom)
{
    if (inDom && uniquePtr)
    {
        ptr = parent->addWidget(uniquePtr);
    }
    else if (ptr)
    {
        uniquePtr.set(parent->removeChild(ptr));
        ptr = nullptr;
    }

    currentlyInDom = inDom;
}

#endif  // TRANSIENTWIDGET_H
