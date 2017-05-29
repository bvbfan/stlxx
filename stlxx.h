///////////////////////////////////////////////////////////////////////////////
/// \author (c) Anthony Fieroni (bvbfan@abv.bg)
///             2017, Plovdiv, Bulgaria
///
/// \license The MIT License (MIT)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

#ifndef __STLXXX__
#define __STLXXX__

#include <mutex>

namespace stlxx {

template<class T, class mutex = std::recursive_mutex>
class atomic {
    T *m_obj = nullptr;
    size_t *m_cnt = nullptr;
    mutex *m_mutex = nullptr;

    void release() {
        if (!m_mutex) return;
        m_mutex->lock();
        auto cnt = --(*m_cnt);
        m_mutex->unlock();
        if (cnt == 0) {
            delete m_mutex;
            delete m_cnt;
            delete m_obj;
        }
        m_obj = nullptr;
        m_cnt = nullptr;
        m_mutex = nullptr;
    }

    class lock_proxy {
        T *m_obj;
        mutex &m_mutex;
    public:
        lock_proxy(mutex &m, T *o) : m_obj(o), m_mutex(m) { m_mutex.lock(); }
        ~lock_proxy() { m_mutex.unlock(); }
        T* operator->() { return m_obj; }
    };

public:
    atomic() {
        m_obj = new T;
        m_mutex = new mutex;
        m_cnt = new size_t{ 1 };
    }

    atomic(T &&o) { operator=(std::move(o)); }
    atomic(atomic &&o) { operator=(std::move(o)); }
    atomic(const atomic &o) { operator=(o); }
    ~atomic() { release(); }

    atomic& operator=(atomic &&o) {
        if (this == &o) return *this;
        release();
        m_obj = o.m_obj;
        m_cnt = o.m_cnt;
        m_mutex = o.m_mutex;
        o.m_obj = nullptr;
        o.m_cnt = nullptr;
        o.m_mutex = nullptr;
        return *this;
    }

    atomic& operator=(const atomic &o) {
        if (this == &o) return *this;
        release();
        m_obj = o.m_obj;
        m_cnt = o.m_cnt;
        m_mutex = o.m_mutex;
        std::lock_guard<mutex> guard(*m_mutex);
        ++(*m_cnt);
        return *this;
    }

    atomic& operator=(T &&o) {
        release();
        m_mutex = new mutex;
        m_cnt = new size_t{ 1 };
        m_obj = new T{ std::move(o) };
        return *this;
    }

    lock_proxy operator->() const {
        return { *m_mutex, m_obj };
    }

    operator mutex&() const { return *m_mutex; }
    operator const T&() const { return *m_obj; }
    operator T&&() && { return std::move(*m_obj); };
};

class synchronize {
    template<class T> static
    void helper(std::function<void()> &func, T &mutex) {
        std::lock_guard<T> lock(mutex, std::adopt_lock);
        func();
    }

    template<class T1, class... TN> static
    void helper(std::function<void()> &func, T1& mutex1, TN&... mutexN) {
        std::lock_guard<T1> lock(mutex1, std::adopt_lock);
        helper(func, mutexN...);
    }

    template<class T1, class... TN> friend
    void synchronized(std::function<void()> func, T1& mutex1, TN&... mutexN);
};

template<class T> inline
void synchronized(std::function<void()> func, T &mutex) {
    std::lock_guard<T> lock(mutex, std::adopt_lock);
    func();
}

template<class T1, class... TN> inline
void synchronized(std::function<void()> func, T1& mutex1, TN&... mutexN) {
    std::lock(mutex1, mutexN...);
    synchronize::helper(func, mutex1, mutexN...);
}

}

#endif // __STLXXX__
