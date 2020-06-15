#include <Wt/Date/tz.h>
#include <Wt/WApplication.h>
#include <Wt/WDateTime.h>
#include <Wt/WEnvironment.h>
#include <Wt/WLocalDateTime.h>
#include <Wt/WLocale.h>

#include <string>

#include "I18nUtils.h"

Wt::WLocalDateTime localizeDate(const Wt::WDateTime& date)
{
    Wt::WLocale loc{Wt::WLocale::currentLocale()};

    // This is going to be empty sometimes (pre-login), in that case use UTC
    const std::string& tzName{Wt::WApplication::instance()->environment().timeZoneName()};

    // get in the zone
    auto zone = tzName.empty()
        ? date::zoned_traits<const date::time_zone*>::default_zone()
        : date::locate_zone(tzName);
    loc.setTimeZone(zone);

    return date.toLocalTime(loc);
}
