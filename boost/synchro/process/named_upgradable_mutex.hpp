//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/synchro for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_SYNCHRO_PROCESS_NAMED_UPGRADABLE_MUTEX__HPP
#define BOOST_SYNCHRO_PROCESS_NAMED_UPGRADABLE_MUTEX__HPP

#include <boost/interprocess/sync/named_upgradable_mutex.hpp>
#include <boost/synchro/lockable_traits.hpp>
#include <boost/synchro/process/lockable_scope_traits.hpp>
#include <boost/synchro/timeout_exception.hpp>
#include <boost/synchro/detail/deleted_functions.hpp>

namespace boost { namespace synchro {

#if 0
class interprocess_named_upgradable_mutex
    : public interprocess::named_upgradable_mutex,
      public lock_traits_base<
        multi_process_tag,
        upgradable_lock_tag,
        non_recursive_tag,
        has_timed_interface_tag,
        kernel_lifetime_tag,
        named_tag
    >     
{
public:
    typedef boost::interprocess::interprocess_condition  condition_type;
    typedef boost::interprocess::interprocess_condition  condition_any_type;

    BOOST_DEFAULT_CONSTRUCTOR_DELETE(interprocess_named_upgradable_mutex) /*< disable default construction >*/
    //Non-copyable
    BOOST_COPY_CONSTRUCTOR_DELETE(interprocess_named_upgradable_mutex) /*< disable copy construction >*/
    BOOST_COPY_ASSIGNEMENT_DELETE(interprocess_named_upgradable_mutex) /*< disable copy asignement >*/

    interprocess_named_upgradable_mutex(interprocess::create_only_t create_only, const char *name)
           : interprocess::named_upgradable_mutex(create_only, name) {};
       interprocess_named_upgradable_mutex(interprocess::open_or_create_t open_or_create, const char *name)
          : interprocess::named_upgradable_mutex(open_or_create, name) {};
      interprocess_named_upgradable_mutex(interprocess::open_only_t open_only, const char *name)
          : interprocess::named_upgradable_mutex(open_only, name) {};

    bool try_lock_until(system_time const & abs_time)
    {return timed_lock(abs_time);}
    template<typename TimeDuration>
    bool try_lock_for(TimeDuration const & relative_time)
    {return timed_lock(boost::get_system_time()+relative_time);}

    void lock_until(system_time const & abs_time)
    {if(!timed_lock(abs_time)) throw timeout_exception();}
    template<typename TimeDuration>
    void lock_for(TimeDuration const & relative_time)
    {if(!timed_lock(boost::get_system_time()+relative_time)) throw timeout_exception();}
    
    void lock_shared()
    {lock_sharable();}

    bool try_lock_shared()
    {return try_lock_sharable();}

    bool try_lock_shared_until(const boost::posix_time::ptime &abs_time)
    {return timed_lock_sharable(abs_time);}
    template<typename TimeDuration>
    bool try_lock_shared_for(const TimeDuration &rel_time)
    {return timed_lock_sharable(boost::get_system_time()+rel_time);}
    
    void lock_shared_until(const boost::posix_time::ptime &abs_time)
    {if(!timed_lock_sharable(abs_time)) throw timeout_exception();}
    template<typename TimeDuration>
    void lock_shared_for(const TimeDuration &rel_time)
    {if(!timed_lock_sharable(boost::get_system_time()+rel_time)) throw timeout_exception();}

    void unlock_shared()
    {unlock_sharable();}

    void lock_upgrade()
    {lock_upgradable();}

    bool try_lock_upgrade()
    {return try_lock_upgradable();}

    bool try_lock_upgrade_until(const boost::posix_time::ptime &abs_time)
    {return timed_lock_upgradable(abs_time);}
    template<typename TimeDuration>
    bool try_lock_upgrade_for(const TimeDuration &rel_time)
    {return timed_lock_upgradable(boost::get_system_time()+rel_time);}

    void lock_upgrade_until(const boost::posix_time::ptime &abs_time)
    {if(!timed_lock_upgradable(abs_time)) throw timeout_exception();}
    template<typename TimeDuration>
    bool lock_upgrade_for(const TimeDuration &rel_time)
    {if(!timed_lock_upgradable(boost::get_system_time()+rel_time)) throw timeout_exception();}
    
    void unlock_upgrade()
    {unlock_upgradable();}

    void unlock_and_lock_upgrade()
    {unlock_and_lock_upgrade();}

    void unlock_and_lock_shared()
    {unlock_and_lock_sharable();}

    void unlock_upgrade_and_lock_shared()
    {
        unlock_upgradable_and_lock_sharable();
    }
    void unlock_upgrade_and_lock()
    {unlock_upgrade_and_lock();}

    bool try_unlock_upgrade_and_lock()
    {return try_unlock_upgradable_and_lock();}

#if 0
    bool unlock_upgrade_and_lock_until(const boost::posix_time::ptime &abs_time);
    {return timed_unlock_upgradable_and_lock(abs_time);}

    bool try_unlock_share_and_lock();
    {return try_unlock_sharable_and_lock();}

    bool try_unlock_share_and_lock_upgrade();
    {return try_unlock_sharable_and_lock_upgradable();}
#endif

};
#endif

typedef interprocess::named_upgradable_mutex interprocess_named_upgradable_mutex;


template<>
struct timed_interface_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef has_timed_interface_tag type;
};
template<>
struct reentrancy_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef non_recursive_tag type;
};
template<>
struct category_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef upgradable_lock_tag type;
};
template<>
struct scope_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef multi_process_tag type;
};

template<>
struct lifetime_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef kernel_lifetime_tag type;
};

template<>
struct naming_tag<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef named_tag type;
};

template <>
struct best_condition<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef boost::interprocess::interprocess_condition type;
};

template <>
struct best_condition_any<boost::synchro::interprocess_named_upgradable_mutex> {
    typedef boost::interprocess::interprocess_condition type;
};

#if 0
template<>
struct syntactic_lock_traits<boost::synchro::named_upgradable_mutex>
    : syntactic_process_lock_traits<boost::synchro::named_upgradable_mutex> {};
#endif

}
}

#endif
