#pragma once
#ifdef ASSERT_ENABLED
	#define __ASSERT_USE_STDERR
	#include <assert.h>
	void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp);
#else
#define assert(ASSERTION) {ASSERTION;}	// disabled assertion should execute nothing
#endif