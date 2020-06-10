#ifndef I18NUTILS_H
#define I18NUTILS_H

#include <Wt/WLocalDateTime.h>

namespace Wt {
    class WDateTime;
}

Wt::WLocalDateTime localizeDate(const Wt::WDateTime& date);

#endif  // I18NUTILS_H
