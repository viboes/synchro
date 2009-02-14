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
    class unique_locker<Mutex,multi_threaded_tag>:  public unique_lock_type<Mutex>::type {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    private:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename unique_lock_type<Mutex>::type base_type;
        //unique_locker(unique_locker&);
        explicit unique_locker(unique_locker<Mutex, scope_tag_type>&);
        //unique_locker& operator=(unique_locker&);
        unique_locker& operator=(unique_locker<Mutex, scope_tag_type>& other);
    public:
        unique_locker(): base_type()
        {}

        explicit unique_locker(Mutex& m_): base_type(m_)
        {}
        unique_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        unique_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        unique_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}
        template<typename TimeDuration>
        unique_locker(Mutex& m_,TimeDuration const& target_time): base_type(m_, target_time)
        {}
        unique_locker(Mutex& m_,system_time const& target_time): base_type(m_, target_time)
        {}
        template<typename TimeDuration>
        unique_locker(Mutex& m_,TimeDuration const& target_time, throw_lock_t)
            : base_type(m_, defer_lock)
        {
            lock_for(target_time);
        }
        unique_locker(Mutex& m_,system_time const& target_time, throw_lock_t)
            : base_type(m_, defer_lock)
        {
            lock_until(target_time);
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

        unique_locker& operator=(upgrade_locker<Mutex>&& other)
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

        template<typename TimeDuration>
        bool try_lock_for(TimeDuration const& relative_time)
        {
            return this->timed_lock(relative_time);
        }

        bool try_lock_until(::boost::system_time const& absolute_time)
        {
            return this->timed_lock(absolute_time);
        }

        template<typename TimeDuration>
        void lock_for(TimeDuration const& relative_time)
        {
            if(!try_lock_for(relative_time)) throw timeout_exception();
        }

        void lock_until(::boost::system_time const& absolute_time)
        {
            if(!try_lock_until(absolute_time)) throw timeout_exception();
        }

        friend class shared_locker<Mutex,scope_tag_type>;
        friend class upgrade_locker<Mutex,scope_tag_type>;
    };
#if 0
#ifdef BOOST_HAS_RVALUE_REFS
    template<typename Mutex, typename ScopeTag>
    void swap(unique_locker<Mutex, ScopeTag>&& lhs, unique_locker<Mutex, ScopeTag>&& rhs)
    {
        lhs.swap(rhs);
    }
#else
    template<typename Mutex, typename ScopeTag>
    void swap(unique_locker<Mutex, ScopeTag>& lhs, unique_locker<Mutex, ScopeTag>& rhs)
    {
        lhs.swap(rhs);
    }
#endif

#ifdef BOOST_HAS_RVALUE_REFS
    template<typename Mutex, typename ScopeTag>
    inline unique_locker<Mutex, ScopeTag>&& move(unique_locker<Mutex, ScopeTag>&& ul)
    {
        return ul;
    }
#endif
#endif
    template<typename Mutex>
    class shared_locker<Mutex,multi_threaded_tag>:  public shared_lock_type<Mutex>::type {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename shared_lock_type<Mutex>::type base_type;

    private:
        explicit shared_locker(shared_locker&);
        shared_locker& operator=(shared_locker&);
    public:
        shared_locker(): base_type()
        {}

        explicit shared_locker(Mutex& m_): base_type(m_)
        {}
        shared_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        shared_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        shared_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}
        shared_locker(Mutex& m_,system_time const& target_time):  base_type(m_, target_time)
        {}
        template<typename TimeDuration>
        shared_locker(Mutex& m_,TimeDuration const& target_time): base_type(m_, boost::get_system_time()+target_time)
        {}
        shared_locker(Mutex& m_,system_time const& target_time,throw_lock_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_until(target_time);
        }
        template<typename TimeDuration>
        shared_locker(Mutex& m_,system_time const& target_time,throw_lock_t)
            : base_type(m_, boost::defer_lock)
        {
            lock_for(target_time);
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
        
        bool try_lock_until(boost::system_time const& absolute_time)
        {
            return this->timed_lock(absolute_time);
        }
        template<typename Duration>
        bool try_lock_for(Duration const& relative_time)
        {
            return this->timed_lock(relative_time);
        }

        template<typename TimeDuration>
        void lock_for(TimeDuration const& relative_time)
        {
            if(!try_lock_for(relative_time)) throw timeout_exception();
        }

        void lock_until(::boost::system_time const& absolute_time)
        {
            if(!try_lock_until(absolute_time)) throw timeout_exception();
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
    class upgrade_locker<Mutex,multi_threaded_tag>:  public upgrade_lock_type<Mutex>::type {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename upgrade_lock_type<Mutex>::type base_type;
    private:
        explicit upgrade_locker(upgrade_locker&);
        upgrade_locker& operator=(upgrade_locker&);
    public:
        upgrade_locker(): base_type()
        {}

        explicit upgrade_locker(Mutex& m_): base_type(m_)
        {
        }
        upgrade_locker(Mutex& m_,adopt_lock_t): base_type(m_, boost::adopt_lock)
        {}
        upgrade_locker(Mutex& m_,defer_lock_t): base_type(m_, boost::defer_lock)
        {}
        upgrade_locker(Mutex& m_,try_to_lock_t): base_type(m_, boost::try_to_lock)
        {}

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

        friend class shared_locker<Mutex, scope_tag_type>;
        friend class unique_locker<Mutex, scope_tag_type>;
    };


    template<typename Mutex>
    class upgrade_to_unique_locker<Mutex,multi_threaded_tag>:  public upgrade_to_unique_locker_type<Mutex>::type {
        //typename scope_tag<Mutex>::type == multi_threaded_tag
    public:
        typedef Mutex lockable_type;
        typedef multi_threaded_tag scope_tag_type;
        typedef typename upgrade_to_unique_locker_type<Mutex>::type base_type;
    private:
        explicit upgrade_to_unique_locker(upgrade_to_unique_locker&);
        upgrade_to_unique_locker& operator=(upgrade_to_unique_locker&);
    public:
        explicit upgrade_to_unique_locker(upgrade_locker<Mutex>& m_): base_type(m_)
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
            return static_cast<Mutex*>(this->base_type.mutex());
        }

        bool is_locking(lockable_type* l) const { 
            return l==mutex(); 
        } /*< strict lockers specific function >*/
    };

}
}

#endif
