#ifndef TRANSIENTWIDGET_H
#define TRANSIENTWIDGET_H

#include <Wt/WContainerWidget.h> // IWYU pragma: keep
#include <Wt/WWidget.h> // IWYU pragma: keep

#include <memory>
#include <type_traits>
#include <utility>

/**
 * A widget that may or may not be actually present in the DOM at any given time,
 * but that we still want to keep track of.
 */
template<class W>
class TransientWidget
{
    static_assert(std::is_base_of<Wt::WWidget, W>::value, "Transient widget must be a WWidget");

public:
    template<class... Args>
    inline TransientWidget(Wt::WContainerWidget* parent, Args&&... args)
      : parent{parent},
        uniquePtr{std::make_unique<W>(std::forward<Args>(args)...)},
        ptr{nullptr},
        alwaysInDom{false},
        currentlyInDom{false},
        shown{false}
    {}

    inline W* operator->() const
    {
        return ptr ? ptr : uniquePtr.get();
    }

    inline void setShown(bool shown)
    {
        if (alwaysInDom)
            ptr->setHidden(!shown);
        else
            setInDom(shown);

        this->shown = shown;
    }

    inline void setAlwaysInDom(bool alwaysInDom)
    {
        this->alwaysInDom = alwaysInDom;
        setInDom(shown || alwaysInDom);
    }

private:
    Wt::WContainerWidget* parent;

    std::unique_ptr<W> uniquePtr;
    W* ptr;

    bool alwaysInDom;
    bool currentlyInDom;
    bool shown;

    inline void setInDom(bool inDom)
    {
        if (inDom)
        {
            if (uniquePtr)
                ptr = parent->addWidget(std::move(uniquePtr));
        }
        else if (ptr)
        {
            Wt::WWidget* removed = ptr->removeFromParent().release();
            uniquePtr.reset(static_cast<W*>(removed));
            ptr = nullptr;
        }

        currentlyInDom = inDom;
    }
};

#endif  // TRANSIENTWIDGET_H
