//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/synchro for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONVERT_TO_OPTIONAL__HPP
#define BOOST_CONVERT_TO_OPTIONAL__HPP

#include <<boost/optional>
#include <boost/none.hpp>
#include <boost/conversion/convert_to.hpp>

namespace boost {

    namespace partial_specialization_workaround {
        template < class Target, class Source>
        struct convert_to< optional<Target>, optional<Source> > {
            inline static optional<Target> apply(optional<Source> const & from)
            {
                return (from?optional<Target>(convert_to<Target>(from)):optional<Target>());
            }
        };
        template < class Target, class Source>
        struct assign_to< optional<Target>, optional<Source> > {
            inline static optional<Target>& apply(const optional<Target>& from, optional<Target>& to)
            {
                to = from?convert_to<Target>(from):none;
            }
        };

    }
}

#endif

