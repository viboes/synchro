//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/synchro for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_SYNCHRO_THREAD_LOCKS__HPP
#define BOOST_SYNCHRO_THREAD_LOCKS__HPP


#include <boost/thread/locks.hpp>
#include <boost/synchro/lockable_traits.hpp>
#include <boost/synchro/lockers.hpp>
#include <boost/synchro/detail/deleted_functions.hpp>
//#include <boost/synchro/thread/thread_to_lockable_adapter.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/conversion/boost/chrono_time_point_to_posix_time_ptime.hpp>
#include <boost/conversion/boost/chrono_duration_to_posix_time_duration.hpp>
#include <boost/synchro/lockable/functions.hpp>

namespace boost { namespace synchro {

    template <typename T>
    struct lockable_type<boost::unique_lock<T> > {
        typedef T type;
    };
    template <typename T>
    struct lockable_type<boost::shared_lock<T> > {
        typedef T type;
    };
    template <typename T>
    struct lockable_type<boost::upgrade_lock<T> > {
        typedef T type;
    };

    template <typename T>
    struct lockable_type<boost::upgrade_to_unique_lock<T> > {
        typedef T type;
    };


    template<typename Mutex>
    class unique_locker<Mutex,multi_threaded_tag>: 
        public unique_lock_type<Mutex >::type
    {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename unique_lock_type<Mutex >::type base_type;
        typedef typename scope_tag<lockable_type>::type scope;
        typedef typename category_tag<lockable_type>::type category;
        typedef typename reentrancy_tag<lockable_type>::type reentrancy;
        typedef typename timed_interface_tag<lockable_type>::type timed_interface;
        typedef typename lifetime_tag<lockable_type>::type lifetime;
        typedef typename naming_tag<lockable_type>::type naming;

        BOOST_NON_CONST_COPY_CONSTRUCTOR_DELETE(unique_locker) /*< disable copy construction >*/
        BOOST_NON_CONST_COPY_ASSIGNEMENT_DELETE(unique_locker) /*< disable copy asignement >*/
        unique_locker(): base_type() {}

        explicit unique_locker(Mutex& m_): base_type(m_)
        {}
        unique_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        unique_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        unique_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}

        template <class Clock, class Duration>            
        unique_locker(Mutex& m_, chrono::time_point<Clock, Duration> const& target_time)
            : base_type(m_, boost::convert_to<posix_time::ptime>(target_time))
        {}
        template <typename Rep, typename Period>            
        unique_locker(Mutex& m_, chrono::duration<Rep, Period> const& target_time)
            : base_type(m_, boost::convert_to<posix_time::time_duration>(target_time))
        {}
            
        template <class Clock, class Duration>            
        unique_locker(nothrow_timeout_t, chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::convert_to<posix_time::ptime>(target_time))
        {}
        template <typename Rep, typename Period>            
        unique_locker(nothrow_timeout_t, chrono::duration<Rep, Period> const& target_time, Mutex& m_)
            : base_type(m_, boost::convert_to<posix_time::time_duration>(target_time))
        {}


        template <class Clock, class Duration>            
        unique_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time, throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_until(target_time);
        }
        template <typename Rep, typename Period>            
        unique_locker(Mutex& m_, chrono::duration<Rep, Period> const& target_time, throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_for(target_time);
        }

        template <class Clock, class Duration>            
        unique_locker(chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            lock_until(target_time);
        }
        template <typename Rep, typename Period>            
        unique_locker(chrono::duration<Rep, Period> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            lock_for(target_time);
        }
#ifdef BOOST_HAS_RVALUE_REFS
        unique_locker(unique_locker&& other): base_type(other)
        {}
        explicit unique_locker(upgrade_locker<Mutex,scope_tag_type>&& other) : base_type(other)
        {}

        unique_locker<Mutex, scope_tag_type>&& move()
        {
            return static_cast<unique_locker<Mutex, scope_tag_type>&&>(*this);
        }


        unique_locker& operator=(unique_locker<Mutex, scope_tag_type>&& other)
        {
            unique_locker temp(other);
            swap(temp);
            return *this;
        }

        unique_locker& operator=(upgrade_locker<Mutex, multi_threaded_tag>&& other)
        {
            unique_locker temp(other);
            swap(temp);
            return *this;
        }
        void swap(unique_locker&& other)
        {
            this->base_type.swap(other);
        }
#else
        unique_locker(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<base_type>(other.t))
        {}
        unique_locker(detail::thread_move_t<upgrade_locker<Mutex,scope_tag_type> > other)
            : base_type(detail::thread_move_t<upgrade_locker<Mutex,scope_tag_type> >(other.t))
        {}

        operator detail::thread_move_t<unique_locker<Mutex, scope_tag_type> >()
        {
            return move();
        }

        detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > move()
        {
            return detail::thread_move_t<unique_locker<Mutex, scope_tag_type> >(*this);
        }

        unique_locker& operator=(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
        {
            unique_locker temp(other);
            swap(temp);
            return *this;
        }

        unique_locker& operator=(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
        {
            unique_locker temp(other);
            swap(temp);
            return *this;
        }
        void swap(unique_locker& other)
        {
            this->base_type.swap(other);
        }
        void swap(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
        {
            this->base_type.swap(detail::thread_move_t<base_type>(other.t));
        }
#endif

        ~unique_locker() {}

        Mutex* mutex() const
        {
            return static_cast<Mutex*>(this->base_type.mutex());
        }
        bool is_locking(lockable_type* l) const {
            return l==mutex();
        } /*< strict lockers specific function >*/

        template<typename Clock, typename Duration>
        bool try_lock_until(chrono::time_point<Clock, Duration> const & abs_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::ptime>(abs_time));
        }

        template <class Rep, class Period >
        bool try_lock_for(chrono::duration<Rep, Period> const& relative_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::time_duration>(relative_time));
        }

        template<typename Clock, typename Duration>
        void lock_until(chrono::time_point<Clock, Duration> const & abs_time)
        {
            if(!this->timed_lock(boost::convert_to<posix_time::ptime>(abs_time))) throw timeout_exception();
        }
        template<typename Rep, typename Period>
        void lock_for(chrono::duration<Rep, Period> const & rel_time)
        {
            if(!this->timed_lock(boost::convert_to<posix_time::time_duration>(rel_time))) throw timeout_exception();
        }
        
        friend class shared_locker<Mutex,scope_tag_type>;
        friend class upgrade_locker<Mutex,scope_tag_type>;
    };

    template<typename Mutex>
    class try_unique_locker<Mutex,multi_threaded_tag>:  public unique_locker<Mutex,multi_threaded_tag> {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef unique_locker<Mutex,multi_threaded_tag> base_type;

        try_unique_locker(): base_type() {}
        BOOST_NON_CONST_COPY_CONSTRUCTOR_DELETE(try_unique_locker) /*< disable copy construction >*/
        BOOST_NON_CONST_COPY_ASSIGNEMENT_DELETE(try_unique_locker) /*< disable copy asignement >*/

        explicit try_unique_locker(Mutex& m_): base_type(m_, defer_lock)
        { 
            this->try_lock(); 
        }
        try_unique_locker(Mutex& m_,force_lock_t): base_type(m_)
        {}
        try_unique_locker(Mutex& m_,adopt_lock_t tag): base_type(m_, tag)
        {}
        try_unique_locker(Mutex& m_,defer_lock_t tag): base_type(m_, tag)
        {}
        try_unique_locker(Mutex& m_,try_to_lock_t tag): base_type(m_, tag)
        {}
            
        template<typename Clock, typename Duration>
        try_unique_locker(Mutex& m_, chrono::time_point<Clock, Duration> const& abs_time)
            : base_type(m_, boost::convert_to<posix_time::ptime>(abs_time))
        {}
        template<typename Rep, typename Period>
        try_unique_locker(Mutex& m_,chrono::duration<Rep, Period> const& rel_time)
            : base_type(m_, boost::convert_to<posix_time::time_duration>(rel_time))
        {}
            
        template<typename Clock, typename Duration>
        try_unique_locker(nothrow_timeout_t, chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::convert_to<posix_time::ptime>(target_time))
        {}
        template<typename Rep, typename Period>
        try_unique_locker(nothrow_timeout_t, chrono::duration<Rep, Period> const& target_time, Mutex& m_)
            : base_type(m_, target_time)
        {}


        template<typename Clock, typename Duration>
        try_unique_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time, throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_until(target_time);
        }
        template<typename Rep, typename Period>
        try_unique_locker(Mutex& m_,chrono::duration<Rep, Period> const& target_time, throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_for(target_time);
        }
        
        template<typename Clock, typename Duration>
        try_unique_locker(chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_until(target_time);
        }
        template<typename Rep, typename Period>
        try_unique_locker(chrono::duration<Rep, Period> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_for(target_time);
        }

#ifdef BOOST_HAS_RVALUE_REFS
        try_unique_locker(try_unique_locker&& other): base_type(other)
        {}
        explicit try_unique_locker(upgrade_locker<Mutex,scope_tag_type>&& other) : base_type(other)
        {}

        try_unique_locker<Mutex, scope_tag_type>&& move()
        {
            return static_cast<try_unique_locker<Mutex, scope_tag_type>&&>(*this);
        }


        try_unique_locker& operator=(try_unique_locker<Mutex, scope_tag_type>&& other)
        {
            try_unique_locker temp(other);
            swap(temp);
            return *this;
        }

        try_unique_locker& operator=(upgrade_locker<Mutex, multi_threaded_tag>&& other)
        {
            try_unique_locker temp(other);
            swap(temp);
            return *this;
        }
        void swap(try_unique_locker&& other)
        {
            this->base_type.swap(other);
        }
#else
        try_unique_locker(detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<base_type>(other.t))
        {}
        try_unique_locker(detail::thread_move_t<upgrade_locker<Mutex,scope_tag_type> > other)
            : base_type(detail::thread_move_t<upgrade_locker<Mutex,scope_tag_type> >(other.t))
        {}

        operator detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> >()
        {
            return move();
        }

        detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> > move()
        {
            return detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> >(*this);
        }

        try_unique_locker& operator=(detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> > other)
        {
            try_unique_locker temp(other);
            swap(temp);
            return *this;
        }

        try_unique_locker& operator=(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
        {
            try_unique_locker temp(other);
            swap(temp);
            return *this;
        }
        void swap(try_unique_locker& other)
        {
            this->base_type.swap(other);
        }
        void swap(detail::thread_move_t<try_unique_locker<Mutex, scope_tag_type> > other)
        {
            this->base_type.swap(detail::thread_move_t<base_type>(other.t));
        }
#endif

        ~try_unique_locker() {}

        friend class shared_locker<Mutex,scope_tag_type>;
        friend class upgrade_locker<Mutex,scope_tag_type>;
    };

    template<typename Mutex>
    class shared_locker<Mutex,multi_threaded_tag>:
        public shared_lock_type<Mutex >::type 
    {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename shared_lock_type<Mutex>::type base_type;

        shared_locker(): base_type() {}
        BOOST_NON_CONST_COPY_CONSTRUCTOR_DELETE(shared_locker) /*< disable copy construction >*/
        BOOST_NON_CONST_COPY_ASSIGNEMENT_DELETE(shared_locker) /*< disable copy asignement >*/

        explicit shared_locker(Mutex& m_): base_type(m_)
        {}
        shared_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        shared_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        shared_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}
            
        template<typename Clock, typename Duration>
        shared_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time)
            : base_type(m_, boost::defer_lock)
        {
            try_lock_until(target_time);
        }
        template<typename Rep, typename Period>
        shared_locker(Mutex& m_,chrono::duration<Rep, Period> const& target_time)
            : base_type(m_, boost::defer_lock)
        {
            try_lock_for(target_time);
        }
            
        template<typename Clock, typename Duration>
        shared_locker(nothrow_timeout_t, chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::convert_to<posix_time::ptime>(target_time))
        {}
        template<typename Rep, typename Period>
        shared_locker(nothrow_timeout_t, chrono::duration<Rep, Period> const& target_time, Mutex& m_)
        {
            try_lock_for(target_time);
        }
            
        template<typename Clock, typename Duration>
        shared_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time,throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_until(target_time);
        }
        template<typename Rep, typename Period>
        shared_locker(Mutex& m_,chrono::duration<Rep, Period> const& target_time,throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_for(target_time);
        }
        
        template<typename Rep, typename Period>
        shared_locker(chrono::duration<Rep, Period> const& target_time, Mutex& m_)
        {
            lock_for(target_time);
        }
        template<typename Clock, typename Duration>
        shared_locker(chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            lock_until(target_time);
        }

        shared_locker(detail::thread_move_t<shared_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<base_type>(other.t))
        {}

        shared_locker(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<typename unique_lock_type<Mutex>::type>(other.t))
        {}

        shared_locker(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<typename upgrade_lock_type<Mutex>::type>(other.t))
        {}

        operator detail::thread_move_t<shared_locker<Mutex,scope_tag_type> >()
        {
            return move();
        }

        detail::thread_move_t<shared_locker<Mutex, scope_tag_type> > move()
        {
            return detail::thread_move_t<shared_locker<Mutex, scope_tag_type> >(*this);
        }


        shared_locker& operator=(detail::thread_move_t<shared_locker<Mutex, scope_tag_type> > other)
        {
            shared_locker temp(other);
            swap(temp);
            return *this;
        }

        shared_locker& operator=(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
        {
            shared_locker temp(other);
            swap(temp);
            return *this;
        }

        shared_locker& operator=(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
        {
            shared_locker temp(other);
            swap(temp);
            return *this;
        }

#ifdef BOOST_HAS_RVALUE_REFS
        void swap(shared_locker&& other)
        {
            this->base_type.swap(other);
        }
#else
        void swap(shared_locker& other)
        {
            this->base_type.swap(other);
        }
        void swap(boost::detail::thread_move_t<shared_locker<Mutex, scope_tag_type> > other)
        {
            this->base_type.swap(detail::thread_move_t<base_type>(other.t));
        }
#endif

        Mutex* mutex() const
        {
            return static_cast<Mutex*>(this->base_type.mutex());
        }

        bool is_locking(lockable_type* l) const {
            return l==mutex();
        } /*< strict lockers specific function >*/

        template<typename Clock, typename Duration>
        bool try_lock_until(boost::chrono::time_point<Clock, Duration> const& absolute_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::ptime>(absolute_time));
        }
        template<typename Rep, typename Period>
        bool try_lock_for(chrono::duration<Rep, Period> const& relative_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::time_duration>(relative_time));
        }

        template<typename Rep, typename Period>
        void lock_for(chrono::duration<Rep, Period> const& relative_time)
        {
            if(!this->try_lock_for(relative_time)) throw timeout_exception();
        }

        template<typename Clock, typename Duration>
        void lock_until(::boost::chrono::time_point<Clock, Duration> const& absolute_time)
        {
            if(!this->try_lock_until(absolute_time)) throw timeout_exception();
        }

    };

#if 0
#ifdef BOOST_HAS_RVALUE_REFS
    template<typename Mutex, typename ScopeTag>
    void swap(shared_locker<Mutex, ScopeTag>&& lhs,shared_locker<Mutex, ScopeTag>&& rhs)
    {
        lhs.swap(rhs);
    }
#else
    template<typename Mutex, typename ScopeTag>
    void swap(shared_locker<Mutex, ScopeTag>& lhs,shared_locker<Mutex, ScopeTag>& rhs)
    {
        lhs.swap(rhs);
    }
#endif
#endif
    template<typename Mutex>
    class upgrade_locker<Mutex,multi_threaded_tag>:  
        public upgrade_lock_type<Mutex>::type 
    {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename upgrade_lock_type<Mutex>::type base_type;

        upgrade_locker(): base_type() {}
        BOOST_NON_CONST_COPY_CONSTRUCTOR_DELETE(upgrade_locker) /*< disable copy construction >*/
        BOOST_NON_CONST_COPY_ASSIGNEMENT_DELETE(upgrade_locker) /*< disable copy asignement >*/

        explicit upgrade_locker(Mutex& m_): base_type(m_)
        {
        }
        upgrade_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        upgrade_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        upgrade_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}

        template<typename Clock, typename Duration>
        upgrade_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time)
            : base_type(m_, boost::defer_lock)
        {
            this->try_lock_until(target_time);
        }
        template<typename Rep, typename Period>
        upgrade_locker(Mutex& m_,chrono::duration<Rep, Period> const& target_time)
            : base_type(m_, boost::defer_lock)
        {
            this->try_lock_for(target_time);
        }
            
        template<typename Clock, typename Duration>
        upgrade_locker(nothrow_timeout_t, chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            this->try_lock_until(target_time);
        }
        template<typename Rep, typename Period>
        upgrade_locker(nothrow_timeout_t, chrono::duration<Rep, Period> const& target_time, Mutex& m_)
        {
            this->try_lock_for(target_time);
        }
            
        template<typename Clock, typename Duration>
        upgrade_locker(Mutex& m_,chrono::time_point<Clock, Duration> const& target_time,throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_until(target_time);
        }
        template<typename Rep, typename Period>
        upgrade_locker(Mutex& m_,chrono::duration<Rep, Period> const& target_time,throw_timeout_t)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_for(target_time);
        }
        
        template<typename Rep, typename Period>
        upgrade_locker(chrono::duration<Rep, Period> const& target_time, Mutex& m_)
        {
            this->lock_for(target_time);
        }
        template<typename Clock, typename Duration>
        upgrade_locker(chrono::time_point<Clock, Duration> const& target_time, Mutex& m_)
            : base_type(m_, boost::defer_lock)
        {
            this->lock_until(target_time);
        }
            
        upgrade_locker(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<base_type>(other.t))
        {}

        upgrade_locker(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<typename unique_lock_type<Mutex>::type>(other.t))
        {}

        operator detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> >()
        {
            return move();
        }

        detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > move()
        {
            return detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> >(*this);
        }

        upgrade_locker& operator=(detail::thread_move_t<upgrade_locker<Mutex, scope_tag_type> > other)
        {
            upgrade_locker temp(other);
            swap(temp);
            return *this;
        }

        upgrade_locker& operator=(detail::thread_move_t<unique_locker<Mutex, scope_tag_type> > other)
        {
            upgrade_locker temp(other);
            swap(temp);
            return *this;
        }

        void swap(upgrade_locker& other)
        {
            this->base_type.swap(other);
        }

        ~upgrade_locker()
        {}

        Mutex* mutex() const
        {
            return static_cast<Mutex*>(this->base_type.mutex());
        }

        bool is_locking(lockable_type* l) const {
            return l==mutex();
        } /*< strict lockers specific function >*/

        template<typename Clock, typename Duration>
        bool try_lock_until(boost::chrono::time_point<Clock, Duration> const& absolute_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::ptime>(absolute_time));
        }
        template<typename Rep, typename Period>
        bool try_lock_for(chrono::duration<Rep, Period> const& relative_time)
        {
            return this->timed_lock(boost::convert_to<posix_time::time_duration>(relative_time));
        }

        template<typename Rep, typename Period>
        void lock_for(chrono::duration<Rep, Period> const& relative_time)
        {
            if(!this->try_lock_for(relative_time)) throw timeout_exception();
        }

        template<typename Clock, typename Duration>
        void lock_until(::boost::chrono::time_point<Clock, Duration> const& absolute_time)
        {
            if(!this->try_lock_until(absolute_time)) throw timeout_exception();
        }

        friend class shared_locker<Mutex, scope_tag_type>;
        friend class unique_locker<Mutex, scope_tag_type>;
    };


    template<typename Mutex>
    class upgrade_to_unique_locker<Mutex,multi_threaded_tag>:  
        public upgrade_to_unique_locker_type<Mutex>::type 
    {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename upgrade_to_unique_locker_type<Mutex>::type base_type;

        BOOST_NON_CONST_COPY_CONSTRUCTOR_DELETE(upgrade_to_unique_locker) /*< disable copy construction >*/
        BOOST_NON_CONST_COPY_ASSIGNEMENT_DELETE(upgrade_to_unique_locker) /*< disable copy asignement >*/
        
        explicit upgrade_to_unique_locker(upgrade_locker<Mutex, multi_threaded_tag>& m_): base_type(m_)
        {}
        ~upgrade_to_unique_locker()
        {}

        upgrade_to_unique_locker(detail::thread_move_t<upgrade_to_unique_locker<Mutex, scope_tag_type> > other)
            : base_type(detail::thread_move_t<base_type>(other.t))
        {}

        upgrade_to_unique_locker& operator=(detail::thread_move_t<upgrade_to_unique_locker<Mutex, scope_tag_type> > other)
        {
            upgrade_to_unique_locker temp(other);
            swap(temp);
            return *this;
        }
        void swap(upgrade_to_unique_locker& other)
        {
            this->base_type.swap(other);
        }
        Mutex* mutex() const
        {
            return this->base_type.mutex()->mutex();
        }

        bool is_locking(lockable_type* l) const {
            return l==mutex();
        } /*< strict lockers specific function >*/
    };

}
}

#endif
