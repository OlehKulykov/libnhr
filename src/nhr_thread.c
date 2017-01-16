/*
 *   Copyright (c) 2016 - 2017 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "../libnhr.h"
#include "nhr_thread.h"
#include "nhr_memory.h"
#include "nhr_common.h"

#include <assert.h>

#if defined(NHR_OS_WINDOWS)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

typedef struct _nhr_thread_struct {
	nhr_thread_funct thread_function;
	void * user_object;

#if defined(NHR_OS_WINDOWS)
	HANDLE thread;
#else
	pthread_t thread;
#endif
} _nhr_thread;

typedef struct _nhr_threads_joiner_struct {
	_nhr_thread * thread;
	nhr_mutex mutex;
} _nhr_threads_joiner;

static _nhr_threads_joiner * _threads_joiner = NULL;

static void nhr_threads_joiner_clean(void) { // private
	_nhr_thread * t = _threads_joiner->thread;
#if defined(NHR_OS_WINDOWS)
	DWORD dwExitCode = 0;
#else
	void * r = NULL;
#endif

	if (!t) {
		return;
	}
	
	_threads_joiner->thread = NULL;

#if defined(NHR_OS_WINDOWS)
	do {
		if (GetExitCodeThread(t->thread, &dwExitCode) == 0) {
			break; // fail
		}
	} while (dwExitCode == STILL_ACTIVE);
	if (dwExitCode == STILL_ACTIVE) {
		TerminateThread(t->thread, 0);
	}
	if (CloseHandle(t->thread)) {
		t->thread = NULL;
	}
#else
	pthread_join(t->thread, &r);
	assert(r == NULL);
#endif
	nhr_free(t);
}

static void nhr_threads_joiner_add(_nhr_thread * thread) { // public
	nhr_mutex_lock(_threads_joiner->mutex);
	nhr_threads_joiner_clean();
	_threads_joiner->thread = thread;
	nhr_mutex_unlock(_threads_joiner->mutex);
}

static void nhr_threads_joiner_create_ifneed(void) {
	if (_threads_joiner) {
		return;
	}
	_threads_joiner = (_nhr_threads_joiner *)nhr_malloc_zero(sizeof(_nhr_threads_joiner));
	_threads_joiner->mutex = nhr_mutex_create_recursive();
}

#if defined(NHR_OS_WINDOWS)
static DWORD WINAPI nhr_thread_func_priv(LPVOID some_pointer) {
#else
static void * nhr_thread_func_priv(void * some_pointer) {
#endif
	_nhr_thread * t = (_nhr_thread *)some_pointer;
	t->thread_function(t->user_object);
	nhr_threads_joiner_add(t);

#if  defined(NHR_OS_WINDOWS)
	return 0;
#else
	return NULL;
#endif
}

nhr_thread nhr_thread_create(nhr_thread_funct thread_function, void * user_object) {
	_nhr_thread * t = NULL;
	int res = -1;
#if !defined(NHR_OS_WINDOWS)
	pthread_attr_t attr;
#endif

	if (!thread_function) {
		return NULL;
	}
	nhr_threads_joiner_create_ifneed();
	t = (_nhr_thread *)nhr_malloc_zero(sizeof(_nhr_thread));
	t->user_object = user_object;
	t->thread_function = thread_function;
#if defined(NHR_OS_WINDOWS)
	t->thread = CreateThread(NULL, 0, &nhr_thread_func_priv, (LPVOID)t, 0, NULL);
	assert(t->thread);
#else
	if (pthread_attr_init(&attr) == 0) {
		if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0) {
			if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) {
				res = pthread_create(&t->thread, &attr, &nhr_thread_func_priv, (void *)t);
			}
		}
		pthread_attr_destroy(&attr);
	}
	assert(res == 0);
#endif
	return t;
}

void nhr_thread_sleep(const unsigned int millisec) {
#if defined(NHR_OS_WINDOWS)
	Sleep(millisec); // 1s = 1'000 millisec.
#else
	usleep(millisec * 1000); // 1s = 1'000'000 microsec.
#endif
}

nhr_mutex nhr_mutex_create_recursive(void) {
#if defined(NHR_OS_WINDOWS)
	CRITICAL_SECTION * mutex = (CRITICAL_SECTION *)nhr_malloc_zero(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)mutex);
	return mutex;
#else
	pthread_mutex_t * mutex = (pthread_mutex_t *)nhr_malloc_zero(sizeof(pthread_mutex_t));
	int res = -1;
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr) == 0) {
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0) {
			res = pthread_mutex_init(mutex, &attr);
		}
		pthread_mutexattr_destroy(&attr);
	}
	assert(res == 0);
	return mutex;
#endif
}

void nhr_mutex_lock(nhr_mutex mutex) {
	if (mutex)  {
#if defined(NHR_OS_WINDOWS)
		EnterCriticalSection((LPCRITICAL_SECTION)mutex);
#else
		pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
	}
}

void nhr_mutex_unlock(nhr_mutex mutex) {
	if (mutex) {
#if defined(NHR_OS_WINDOWS)
		LeaveCriticalSection((LPCRITICAL_SECTION)mutex);
#else
		pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
	}
}

void nhr_mutex_delete(nhr_mutex mutex) {
	if (mutex) {
#if defined(NHR_OS_WINDOWS)
		DeleteCriticalSection((LPCRITICAL_SECTION)mutex);
#else
		pthread_mutex_destroy((pthread_mutex_t *)mutex);
#endif
		nhr_free(mutex);
	}
}

