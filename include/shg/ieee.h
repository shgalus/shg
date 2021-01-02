/**
 * \file include/shg/ieee.h
 * IEEE exception handling.
 * \date Created on 20 July 2014.
 */

#ifndef SHG_IEEE_EXCEPTIONS_H
#define SHG_IEEE_EXCEPTIONS_H

#if defined SHG_IEEE_EXCEPTIONS && defined __GNUG__

namespace SHG {

void set_ieee_exception_handler();
void enable_ieee_exceptions();
void disable_ieee_exceptions();

}  // namespace SHG

#define SHG_SET_IEEE_EXCEPTION_HANDLER() \
     SHG::set_ieee_exception_handler()
#define SHG_ENABLE_IEEE_EXCEPTIONS() SHG::enable_ieee_exceptions()
#define SHG_DISABLE_IEEE_EXCEPTIONS() SHG::disable_ieee_exceptions()

#else

#define SHG_SET_IEEE_EXCEPTION_HANDLER()
#define SHG_ENABLE_IEEE_EXCEPTIONS()
#define SHG_DISABLE_IEEE_EXCEPTIONS()

#endif

#endif
