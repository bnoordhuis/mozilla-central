/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef jsnum_h___
#define jsnum_h___

#include <math.h>
#if defined(XP_WIN) || defined(XP_OS2)
#include <float.h>
#endif
#ifdef SOLARIS
#include <ieeefp.h>
#endif

#include "jsstdint.h"
#include "jsstr.h"

/*
 * JS number (IEEE double) interface.
 *
 * JS numbers are optimistically stored in the top 31 bits of 32-bit integers,
 * but floating point literals, results that overflow 31 bits, and division and
 * modulus operands and results require a 64-bit IEEE double.  These are GC'ed
 * and pointed to by 32-bit jsvals on the stack and in object properties.
 */

/*
 * The ARM architecture supports two floating point models: VFP and FPA. When
 * targetting FPA, doubles are mixed-endian on little endian ARMs (meaning that
 * the high and low words are in big endian order).
 */
#if defined(__arm) || defined(__arm32__) || defined(__arm26__) || defined(__arm__)
#if !defined(__VFP_FP__)
#define FPU_IS_ARM_FPA
#endif
#endif

typedef union jsdpun {
    struct {
#if defined(IS_LITTLE_ENDIAN) && !defined(FPU_IS_ARM_FPA)
        uint32 lo, hi;
#else
        uint32 hi, lo;
#endif
    } s;
    uint64   u64;
    jsdouble d;
} jsdpun;

static inline int
JSDOUBLE_IS_NaN(jsdouble d)
{
#ifdef WIN32
    return _isnan(d);
#else
    return isnan(d);
#endif
}

static inline int
JSDOUBLE_IS_FINITE(jsdouble d)
{
#ifdef WIN32
    return _finite(d);
#else
    return finite(d);
#endif
}

static inline int
JSDOUBLE_IS_INFINITE(jsdouble d)
{
#ifdef WIN32
    int c = _fpclass(d);
    return c == _FPCLASS_NINF || c == _FPCLASS_PINF;
#elif defined(SOLARIS)
    return !finite(d) && !isnan(d);
#else
    return isinf(d);
#endif
}

static inline int
JSDOUBLE_IS_NEGZERO(jsdouble d)
{
#ifdef WIN32
    return (d == 0 && (_fpclass(d) & _FPCLASS_NZ));
#elif defined(SOLARIS)
    return (d == 0 && copysign(1, d) < 0);
#else
    return (d == 0 && signbit(d));
#endif
}

#define JSDOUBLE_HI32_SIGNBIT   0x80000000
#define JSDOUBLE_HI32_EXPMASK   0x7ff00000
#define JSDOUBLE_HI32_MANTMASK  0x000fffff

static inline bool
JSDOUBLE_IS_INT32(jsdouble d, int32_t& i)
{
    if (JSDOUBLE_IS_NEGZERO(d))
        return false;
    return d == (i = int32_t(d));
}

static inline bool
JSDOUBLE_IS_NEG(jsdouble d)
{
#ifdef WIN32
    return JSDOUBLE_IS_NEGZERO(d) || d < 0;
#elif defined(SOLARIS)
    return copysign(1, d) < 0;
#else
    return signbit(d);
#endif
}

static inline uint32
JS_HASH_DOUBLE(jsdouble d)
{
    jsdpun u;
    u.d = d;
    return u.s.lo ^ u.s.hi;
}

#if defined(XP_WIN)
#define JSDOUBLE_COMPARE(LVAL, OP, RVAL, IFNAN)                               \
    ((JSDOUBLE_IS_NaN(LVAL) || JSDOUBLE_IS_NaN(RVAL))                         \
     ? (IFNAN)                                                                \
     : (LVAL) OP (RVAL))
#else
#define JSDOUBLE_COMPARE(LVAL, OP, RVAL, IFNAN) ((LVAL) OP (RVAL))
#endif

extern jsdouble js_NaN;
extern jsdouble js_PositiveInfinity;
extern jsdouble js_NegativeInfinity;

/* Initialize number constants and runtime state for the first context. */
extern JSBool
js_InitRuntimeNumberState(JSContext *cx);

extern void
js_TraceRuntimeNumberState(JSTracer *trc);

extern void
js_FinishRuntimeNumberState(JSContext *cx);

/* Initialize the Number class, returning its prototype object. */
extern js::Class js_NumberClass;

extern JSObject *
js_InitNumberClass(JSContext *cx, JSObject *obj);

/*
 * String constants for global function names, used in jsapi.c and jsnum.c.
 */
extern const char js_Infinity_str[];
extern const char js_NaN_str[];
extern const char js_isNaN_str[];
extern const char js_isFinite_str[];
extern const char js_parseFloat_str[];
extern const char js_parseInt_str[];

/* Convert a number to a GC'ed string. */
extern JSString * JS_FASTCALL
js_NumberToString(JSContext *cx, jsdouble d);

/*
 * Convert an integer or double (contained in the given jsval) to a string and
 * append to the given buffer.
 */
extern JSBool JS_FASTCALL
js_NumberValueToCharBuffer(JSContext *cx, jsval v, JSCharBuffer &cb);

namespace js {

/*
 * Convert a value to a number, returning the converted value in 'out' if the
 * conversion succeeds. As a side effect, *vp will be mutated to match *out.
 */
JS_ALWAYS_INLINE bool
ValueToNumber(JSContext *cx, js::Value *vp, double *out)
{
    if (vp->isInt32()) {
        *out = vp->asInt32();
        return true;
    }
    if (vp->isDouble()) {
        *out = vp->asDouble();
        return true;
    }
    extern bool ValueToNumberSlow(JSContext *, js::Value *, double *);
    return ValueToNumberSlow(cx, vp, out);
}

/* Convert a value to a number, replacing 'vp' with the converted value. */
JS_ALWAYS_INLINE bool
ValueToNumber(JSContext *cx, js::Value *vp)
{
    if (vp->isInt32())
        return true;
    if (vp->isDouble())
        return true;
    double _;
    extern bool ValueToNumberSlow(JSContext *, js::Value *, double *);
    return ValueToNumberSlow(cx, vp, &_);
}

/*
 * Convert a value to an int32 or uint32, according to the ECMA rules for
 * ToInt32 and ToUint32. Return converted value in *out on success, !ok on
 * failure. As a side effect, *vp will be mutated to match *out.
 */
JS_ALWAYS_INLINE bool
ValueToECMAInt32(JSContext *cx, js::Value *vp, int32_t *out)
{
    if (vp->isInt32()) {
        *out = vp->asInt32();
        return true;
    }
    extern bool ValueToECMAInt32Slow(JSContext *, js::Value *, int32_t *);
    return ValueToECMAInt32Slow(cx, vp, out);
}

JS_ALWAYS_INLINE bool
ValueToECMAUint32(JSContext *cx, js::Value *vp, uint32_t *out)
{
    if (vp->isInt32()) {
        *out = (uint32_t)vp->asInt32();
        return true;
    }
    extern bool ValueToECMAUint32Slow(JSContext *, js::Value *, uint32_t *);
    return ValueToECMAUint32Slow(cx, vp, out);
}

/*
 * Convert a value to a number, then to an int32 if it fits by rounding to
 * nearest. Return converted value in *out on success, !ok on failure. As a
 * side effect, *vp will be mutated to match *out.
 */
JS_ALWAYS_INLINE bool
ValueToInt32(JSContext *cx, js::Value *vp, int32_t *out)
{
    if (vp->isInt32()) {
        *out = vp->asInt32();
        return true;
    }
    extern bool ValueToInt32Slow(JSContext *, js::Value *, int32_t *);
    return ValueToInt32Slow(cx, vp, out);
}

/*
 * Convert a value to a number, then to a uint16 according to the ECMA rules
 * for ToUint16. Return converted value on success, !ok on failure. v must be a
 * copy of a rooted jsval.
 */
JS_ALWAYS_INLINE bool
ValueToUint16(JSContext *cx, js::Value *vp, uint16_t *out)
{
    if (vp->isInt32()) {
        *out = (uint16_t)vp->asInt32();
        return true;
    }
    extern bool ValueToUint16Slow(JSContext *, js::Value *, uint16_t *);
    return ValueToUint16Slow(cx, vp, out);
}

}  /* namespace js */

/*
 * Specialized ToInt32 and ToUint32 converters for doubles.
 */
/*
 * From the ES3 spec, 9.5
 *  2.  If Result(1) is NaN, +0, -0, +Inf, or -Inf, return +0.
 *  3.  Compute sign(Result(1)) * floor(abs(Result(1))).
 *  4.  Compute Result(3) modulo 2^32; that is, a finite integer value k of Number
 *      type with positive sign and less than 2^32 in magnitude such the mathematical
 *      difference of Result(3) and k is mathematically an integer multiple of 2^32.
 *  5.  If Result(4) is greater than or equal to 2^31, return Result(4)- 2^32,
 *  otherwise return Result(4).
 */
static inline int32
js_DoubleToECMAInt32(jsdouble d)
{
#if defined(__i386__) || defined(__i386)
    jsdpun du, duh, two32;
    uint32 di_h, u_tmp, expon, shift_amount;
    int32 mask32;

    /*
     * Algorithm Outline
     *  Step 1. If d is NaN, +/-Inf or |d|>=2^84 or |d|<1, then return 0
     *          All of this is implemented based on an exponent comparison.
     *  Step 2. If |d|<2^31, then return (int)d
     *          The cast to integer (conversion in RZ mode) returns the correct result.
     *  Step 3. If |d|>=2^32, d:=fmod(d, 2^32) is taken  -- but without a call
     *  Step 4. If |d|>=2^31, then the fractional bits are cleared before
     *          applying the correction by 2^32:  d - sign(d)*2^32
     *  Step 5. Return (int)d
     */

    du.d = d;
    di_h = du.s.hi;

    u_tmp = (di_h & 0x7ff00000) - 0x3ff00000;
    if (u_tmp >= (0x45300000-0x3ff00000)) {
        // d is Nan, +/-Inf or +/-0, or |d|>=2^(32+52) or |d|<1, in which case result=0
        return 0;
    }

    if (u_tmp < 0x01f00000) {
        // |d|<2^31
        return int32_t(d);
    }

    if (u_tmp > 0x01f00000) {
        // |d|>=2^32
        expon = u_tmp >> 20;
        shift_amount = expon - 21;
        duh.u64 = du.u64;
        mask32 = 0x80000000;
        if (shift_amount < 32) {
            mask32 >>= shift_amount;
            duh.s.hi = du.s.hi & mask32;
            duh.s.lo = 0;
        } else {
            mask32 >>= (shift_amount-32);
            duh.s.hi = du.s.hi;
            duh.s.lo = du.s.lo & mask32;
        }
        du.d -= duh.d;
    }

    di_h = du.s.hi;

    // eliminate fractional bits
    u_tmp = (di_h & 0x7ff00000);
    if (u_tmp >= 0x41e00000) {
        // |d|>=2^31
        expon = u_tmp >> 20;
        shift_amount = expon - (0x3ff - 11);
        mask32 = 0x80000000;
        if (shift_amount < 32) {
            mask32 >>= shift_amount;
            du.s.hi &= mask32;
            du.s.lo = 0;
        } else {
            mask32 >>= (shift_amount-32);
            du.s.lo &= mask32;
        }
        two32.s.hi = 0x41f00000 ^ (du.s.hi & 0x80000000);
        two32.s.lo = 0;
        du.d -= two32.d;
    }

    return int32(du.d);
#else
    int32 i;
    jsdouble two32, two31;

    if (!JSDOUBLE_IS_FINITE(d))
        return 0;

    i = (int32) d;
    if ((jsdouble) i == d)
        return i;

    two32 = 4294967296.0;
    two31 = 2147483648.0;
    d = fmod(d, two32);
    d = (d >= 0) ? floor(d) : ceil(d) + two32;
    return (int32) (d >= two31 ? d - two32 : d);
#endif
}

uint32
js_DoubleToECMAUint32(jsdouble d);

/*
 * Convert a jsdouble to an integral number, stored in a jsdouble.
 * If d is NaN, return 0.  If d is an infinity, return it without conversion.
 */
static inline jsdouble
js_DoubleToInteger(jsdouble d)
{
    if (d == 0)
        return d;

    if (!JSDOUBLE_IS_FINITE(d)) {
        if (JSDOUBLE_IS_NaN(d))
            return 0;
        return d;
    }

    JSBool neg = (d < 0);
    d = floor(neg ? -d : d);

    return neg ? -d : d;
}

/*
 * Similar to strtod except that it replaces overflows with infinities of the
 * correct sign, and underflows with zeros of the correct sign.  Guaranteed to
 * return the closest double number to the given input in dp.
 *
 * Also allows inputs of the form [+|-]Infinity, which produce an infinity of
 * the appropriate sign.  The case of the "Infinity" string must match exactly.
 * If the string does not contain a number, set *ep to s and return 0.0 in dp.
 * Return false if out of memory.
 */
extern JSBool
js_strtod(JSContext *cx, const jschar *s, const jschar *send,
          const jschar **ep, jsdouble *dp);

/*
 * Similar to strtol except that it handles integers of arbitrary size.
 * Guaranteed to return the closest double number to the given input when radix
 * is 10 or a power of 2.  Callers may see round-off errors for very large
 * numbers of a different radix than 10 or a power of 2.
 *
 * If the string does not contain a number, set *ep to s and return 0.0 in dp.
 * Return false if out of memory.
 */
extern JSBool
js_strtointeger(JSContext *cx, const jschar *s, const jschar *send,
                const jschar **ep, jsint radix, jsdouble *dp);

namespace js {

static JS_ALWAYS_INLINE bool
ValueFitsInInt32(const Value &v, int32_t *pi)
{
    if (v.isInt32()) {
        *pi = v.asInt32();
        return true;
    }
    return v.isDouble() && JSDOUBLE_IS_INT32(v.asDouble(), *pi);
}

static JS_ALWAYS_INLINE void
Uint32ToValue(uint32_t u, Value *vp)
{
    if (JS_UNLIKELY(u > INT32_MAX))
        vp->setDouble(u);
    else
        vp->setInt32((int32_t)u);
}

template<typename T> struct NumberTraits { };
template<> struct NumberTraits<int32> {
  static JS_ALWAYS_INLINE int32 NaN() { return 0; }
  static JS_ALWAYS_INLINE int32 toSelfType(int32 i) { return i; }
  static JS_ALWAYS_INLINE int32 toSelfType(jsdouble d) { return js_DoubleToECMAUint32(d); }
};
template<> struct NumberTraits<jsdouble> {
  static JS_ALWAYS_INLINE jsdouble NaN() { return js_NaN; }
  static JS_ALWAYS_INLINE jsdouble toSelfType(int32 i) { return i; }
  static JS_ALWAYS_INLINE jsdouble toSelfType(jsdouble d) { return d; }
};

template<typename T>
static JS_ALWAYS_INLINE T
StringToNumberType(JSContext *cx, JSString *str)
{
    if (str->length() == 1) {
        jschar c = str->chars()[0];
        if ('0' <= c && c <= '9')
            return NumberTraits<T>::toSelfType(T(c - '0'));
        if (JS_ISSPACE(c))
            return NumberTraits<T>::toSelfType(T(0));
        return NumberTraits<T>::NaN();
    }

    const jschar* bp;
    const jschar* end;
    const jschar* ep;
    jsdouble d;

    str->getCharsAndEnd(bp, end);
    bp = js_SkipWhiteSpace(bp, end);

    /* ECMA doesn't allow signed hex numbers (bug 273467). */
    if (end - bp >= 2 && bp[0] == '0' && (bp[1] == 'x' || bp[1] == 'X')) {
        /* Looks like a hex number. */
        if (!js_strtointeger(cx, bp, end, &ep, 16, &d) ||
            js_SkipWhiteSpace(ep, end) != end) {
            return NumberTraits<T>::NaN();
        }
        return NumberTraits<T>::toSelfType(d);
    }

    /*
     * Note that ECMA doesn't treat a string beginning with a '0' as
     * an octal number here. This works because all such numbers will
     * be interpreted as decimal by js_strtod.  Also, any hex numbers
     * that have made it here (which can only be negative ones) will
     * be treated as 0 without consuming the 'x' by js_strtod.
     */
    if (!js_strtod(cx, bp, end, &ep, &d) ||
        js_SkipWhiteSpace(ep, end) != end) {
        return NumberTraits<T>::NaN();
    }

    return NumberTraits<T>::toSelfType(d);
}
}

#endif /* jsnum_h___ */
