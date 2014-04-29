/**
 Copyright (c) 2011-present, NimbusKit. All rights reserved.

 This source code is licensed under the BSD-style license found in the LICENSE file in the root
 directory of this source tree and at the http://nimbuskit.info/license url. An additional grant of
 patent rights can be found in the PATENTS file in the same directory and url.
 */

#import <Foundation/Foundation.h>

// All macros #ifndef'd so that they can be individually overwritten if necessary.

#ifndef _NIMBUSKIT_BASICS_H_
#define _NIMBUSKIT_BASICS_H_


#pragma mark Compiler Features


#ifndef NI_DEPRECATED_METHOD
#if __has_feature(attribute_deprecated_with_message)

#define NI_DEPRECATED_METHOD(_msg)  __attribute__((deprecated(_msg)))

// Example:
// - (void)yourDeprecatedMethod:(id)arg NI_DESIGNATED_INITIALIZER;

#else
#define NI_DEPRECATED_METHOD(_msg)  __attribute__((deprecated))
#endif // #if __has_feature
#endif // #ifndef NI_DEPRECATED_METHOD


#ifndef NI_DESIGNATED_INITIALIZER
#if __has_attribute(objc_designated_initializer)

#define NI_DESIGNATED_INITIALIZER __attribute((objc_designated_initializer))

// Example:
// - (instancetype)initWithArg:(id)arg NI_DESIGNATED_INITIALIZER;

#else
#define NI_DESIGNATED_INITIALIZER
#endif // #if __has_feature
#endif // #ifndef NI_DESIGNATED_INITIALIZER


// For use in sources which contain only categories. Removes need for -force_load -all_load when building libraries.
// Use once per source (.m) file (not per category).
// name must be globally unique. Generally a good idea to prefix it.
#ifndef NI_FIX_CATEGORY_BUG
#define NI_FIX_CATEGORY_BUG(name) @interface NI_FIX_CATEGORY_BUG_##name : NSObject @end \
                                  @implementation NI_FIX_CATEGORY_BUG_##name @end

// Example:
// NI_FIX_CATEGORY_BUG(NSMutableAttributedStringNimbusAttributedLabel)
// @implementation NSMutableAttributedString (NimbusAttributedLabel)

#endif


#ifndef NI_IS_FLAG_SET
#define NI_IS_FLAG_SET(value, flag) (((value) & (flag)) == (flag))

// Example:
// if (NI_IS_FLAG_SET(autoresizingMask, UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)
//   YES only if BOTH width and height are specified on the mask.

#endif


#pragma mark UIColor Generators


#ifndef NI_RGBCOLOR
#define NI_RGBCOLOR(r,g,b) [UIColor colorWithRed:(r)/255.0f green:(g)/255.0f blue:(b)/255.0f alpha:1]

// Example:
// NI_RGBCOLOR(255, 0, 255) for a vibrant debugging color

#endif


// `a` is a floating point value [0...1].
#ifndef NI_RGBACOLOR
#define NI_RGBACOLOR(r,g,b,a) [UIColor colorWithRed:(r)/255.0f green:(g)/255.0f blue:(b)/255.0f alpha:(a)]

// Example:
// NI_RGBACOLOR(255, 0, 255, 0.5) for a semi-translucently vibrant debugging color

#endif


#ifndef NI_HEXCOLOR
#define NI_HEXCOLOR(hex) RGBCOLOR(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), ((hex) & 0xFF))

// Example:
// NI_HEXCOLOR(0xFF00FF) for colors pasted from DigitalColor Meter (handy tool, use it!)

#endif


// `a` is a floating point value [0...1].
#ifndef NI_HEXACOLOR
#define NI_HEXACOLOR(hex,a) RGBACOLOR(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), ((hex) & 0xFF), (a))

// Example:
// NI_HEXACOLOR(0xFF00FF, 0.5) for colors pasted from DigitalColor Meter, but with alpha

#endif


#pragma mark Tools for Debugging


#if defined(DEBUG) && !defined(NI_DISABLE_DASSERT)

#import <TargetConditionals.h>
#import <unistd.h>
#import <sys/sysctl.h>

// From: http://developer.apple.com/mac/library/qa/qa2004/qa1361.html
CG_INLINE int NIIsInDebugger(void) {
  int mib[4];
  struct kinfo_proc info;
  size_t size;

  // Initialize the flags so that, if sysctl fails for some bizarre reason, we get a predictable result.
  info.kp_proc.p_flag = 0;

  // Initialize mib, which tells sysctl the info we want, in this case we're looking for information
  // about a specific process ID.
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;
  mib[3] = getpid();

  size = sizeof(info);
  sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);

  // We're being debugged if the P_TRACED flag is set.
  return (info.kp_proc.p_flag & P_TRACED) != 0;
}

CG_INLINE BOOL NIIsRunningTests(void) {
  NSString* injectBundle = [[NSProcessInfo processInfo] environment][@"XCInjectBundle"];
  NSString* pathExtension = [injectBundle pathExtension];
  return ([pathExtension isEqualToString:@"octest"] || [pathExtension isEqualToString:@"xctest"]);
}

#if TARGET_IPHONE_SIMULATOR
// We use the __asm__ in this macro so that when a break occurs, we don't have to step out of
// a "breakInDebugger" function.
#define NI_DASSERT(xx) { if (!(xx)) { NI_DPRINT(@"NI_DASSERT failed: %s", #xx); \
                         if (NIIsInDebugger() && !NIIsRunningTests()) { __asm__("int $3\n" : : ); } } \
                       } ((void)0)
#else
#define NI_DASSERT(xx) { if (!(xx)) { NI_DPRINT(@"NI_DASSERT failed: %s", #xx); \
                         if (NIIsInDebugger() && !NIIsRunningTests()) { raise(SIGTRAP); } } \
                       } ((void)0)
#endif // #if TARGET_IPHONE_SIMULATOR

#else
// The ((void)0) syntax allows us force macros to be terminated with a `;` as though they were functions.
#define NI_DASSERT(xx) ((void)0)

#endif // #if defined(DEBUG) && !defined(NI_DISABLE_DASSERT)


#if defined(DEBUG)
#define NI_DPRINT(xx, ...) NSLog(@"%s(%d): " xx, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define NI_DPRINT(xx, ...) ((void)0)
#endif


#if defined(DEBUG)
#define NI_DCONDITIONLOG(condition, xx, ...) { if ((condition)) { NI_DPRINT(xx, ##__VA_ARGS__); } } ((void)0)
#else
#define NI_DCONDITIONLOG(condition, xx, ...) ((void)0)
#endif


#define NI_DPRINTMETHODNAME() NI_DPRINT(@"%s", __PRETTY_FUNCTION__)


#pragma mark Short-hand runtime checks.


CG_INLINE BOOL NIIsPad(void) {
  return [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad;
}

CG_INLINE BOOL NIIsPhone(void) {
  return [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone;
}

CG_INLINE CGFloat NIScreenScale(void) {
  return [[UIScreen mainScreen] scale];
}

CG_INLINE BOOL NIIsRetina(void) {
  return [[UIScreen mainScreen] scale] == 2.f;
}

// Pre-iOS 7-safe mechanism for accessing UIView's tintColor.
CG_INLINE UIColor* NITintColorForViewWithFallback(UIView* view, UIColor* fallbackColor) {
  return [view respondsToSelector:@selector(tintColor)] ? view.tintColor : fallbackColor;
}

CG_INLINE BOOL NIDeviceOSVersionIsAtLeast(double versionNumber) {
  return kCFCoreFoundationVersionNumber >= versionNumber;
}

#pragma mark iOS Version Numbers

/** Released on July 11, 2008 */
#define NI_IOS_2_0     20000

/** Released on September 9, 2008 */
#define NI_IOS_2_1     20100

/** Released on November 21, 2008 */
#define NI_IOS_2_2     20200

/** Released on June 17, 2009 */
#define NI_IOS_3_0     30000

/** Released on September 9, 2009 */
#define NI_IOS_3_1     30100

/** Released on April 3, 2010 */
#define NI_IOS_3_2     30200

/** Released on June 21, 2010 */
#define NI_IOS_4_0     40000

/** Released on September 8, 2010 */
#define NI_IOS_4_1     40100

/** Released on November 22, 2010 */
#define NI_IOS_4_2     40200

/** Released on March 9, 2011 */
#define NI_IOS_4_3     40300

/** Released on October 12, 2011. */
#define NI_IOS_5_0     50000

/** Released on March 7, 2012. */
#define NI_IOS_5_1     50100

/** Released on September 19, 2012. */
#define NI_IOS_6_0     60000

/** Released on January 28, 2013. */
#define NI_IOS_6_1     60100

/** Released on September 18, 2013 */
#define NI_IOS_7_0     70000

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_0
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_0 478.23
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_1
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_1 478.26
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_2
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_2 478.29
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_0
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_0 478.47
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_1
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_1 478.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_2
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_2 478.61
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_0
#define kCFCoreFoundationVersionNumber_iOS_4_0 550.32
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_1
#define kCFCoreFoundationVersionNumber_iOS_4_1 550.38
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_2
#define kCFCoreFoundationVersionNumber_iOS_4_2 550.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_3
#define kCFCoreFoundationVersionNumber_iOS_4_3 550.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_0
#define kCFCoreFoundationVersionNumber_iOS_5_0 675.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_1
#define kCFCoreFoundationVersionNumber_iOS_5_1 690.10
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_0
#define kCFCoreFoundationVersionNumber_iOS_6_0 793.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_1
#define kCFCoreFoundationVersionNumber_iOS_6_1 793.00
#endif


#pragma mark 32/64 Bit Support

// Until tgmath.h is able to work with modules enabled, you may use the following CGFloat math
// functions to support 32/64 bit code.
// http://stackoverflow.com/questions/23333287/tgmath-h-doesnt-work-if-modules-are-enabled
// http://www.openradar.me/16744288

#if CGFLOAT_IS_DOUBLE
  #define NI_CGFLOAT_EPSILON DBL_EPSILON
#else
  #define NI_CGFLOAT_EPSILON FLT_EPSILON
#endif

CG_INLINE CGFloat NICGFloat_acos(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return acos(x);
#else
  return acosf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_asin(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return asin(x);
#else
  return asinf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_atan(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return atan(x);
#else
  return atanf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_atan2(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return atan2(x, y);
#else
  return atan2f(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_cos(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return cos(x);
#else
  return cosf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_sin(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return sin(x);
#else
  return sinf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_tan(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return tan(x);
#else
  return tanf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_acosh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return acosh(x);
#else
  return acoshf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_asinh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return asinh(x);
#else
  return asinhf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_atanh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return atanh(x);
#else
  return atanhf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_cosh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return cosh(x);
#else
  return coshf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_sinh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return sinh(x);
#else
  return sinhf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_tanh(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return tanh(x);
#else
  return tanhf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_exp(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return exp(x);
#else
  return expf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_exp2(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return exp2(x);
#else
  return exp2f(x);
#endif
}

CG_INLINE CGFloat NICGFloat_expm1(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return expm1(x);
#else
  return expm1f(x);
#endif
}

CG_INLINE CGFloat NICGFloat_log(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return log(x);
#else
  return logf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_log10(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return log10(x);
#else
  return log10f(x);
#endif
}

CG_INLINE CGFloat NICGFloat_log2(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return log2(x);
#else
  return log2f(x);
#endif
}

CG_INLINE CGFloat NICGFloat_log1p(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return log1p(x);
#else
  return log1pf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_logb(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return logb(x);
#else
  return logbf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_fabs(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return fabs(x);
#else
  return fabsf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_cbrt(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return cbrt(x);
#else
  return cbrtf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_hypot(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return hypot(x, y);
#else
  return hypotf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_pow(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return pow(x, y);
#else
  return powf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_sqrt(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return sqrt(x);
#else
  return sqrtf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_erf(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return erf(x);
#else
  return erff(x);
#endif
}

CG_INLINE CGFloat NICGFloat_erfc(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return erfc(x);
#else
  return erfcf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_lgamma(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return lgamma(x);
#else
  return lgammaf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_tgamma(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return tgamma(x);
#else
  return tgammaf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_ceil(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return ceil(x);
#else
  return ceilf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_floor(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return floor(x);
#else
  return floorf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_nearbyint(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return nearbyint(x);
#else
  return nearbyintf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_rint(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return rint(x);
#else
  return rintf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_round(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return round(x);
#else
  return roundf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_trunc(CGFloat x) {
#if CGFLOAT_IS_DOUBLE
  return trunc(x);
#else
  return truncf(x);
#endif
}

CG_INLINE CGFloat NICGFloat_fmod(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return fmod(x, y);
#else
  return fmodf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_remainder(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return remainder(x, y);
#else
  return remainderf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_copysign(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return copysign(x, y);
#else
  return copysignf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_nextafter(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return nextafter(x, y);
#else
  return nextafterf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_fdim(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return fdim(x, y);
#else
  return fdimf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_fmax(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return fmax(x, y);
#else
  return fmaxf(x, y);
#endif
}

CG_INLINE CGFloat NICGFloat_fmin(CGFloat x, CGFloat y) {
#if CGFLOAT_IS_DOUBLE
  return fmin(x, y);
#else
  return fminf(x, y);
#endif
}

#pragma mark Current Version

#ifndef NIMBUSKIT_BASICS_VERSION
#define NIMBUSKIT_BASICS_VERSION NIMBUSKIT_BASICS_1_1_0
#endif

#endif // #ifndef _NIMBUSKIT_BASICS_H_

#pragma mark All Known Versions

#ifndef NIMBUSKIT_BASICS_1_0_0
#define NIMBUSKIT_BASICS_1_0_0 10000
#endif

#ifndef NIMBUSKIT_BASICS_1_1_0
#define NIMBUSKIT_BASICS_1_1_0 10100
#endif

#pragma mark Version Check

#ifndef NI_SUPPRESS_VERSION_WARNINGS

  #if NIMBUSKIT_BASICS_VERSION < NIMBUSKIT_BASICS_1_1_0

    // These macros allow us to inline C-strings with macro values.
    #ifndef NI_MACRO_DEFER
    #define NI_MACRO_DEFER(M,...) M(__VA_ARGS__)
    #endif
    #ifndef NI_MACRO_STR
    #define NI_MACRO_STR(X) #X
    #endif
    #ifndef NI_MACRO_INLINE_STR
    #define NI_MACRO_INLINE_STR(str) NI_MACRO_DEFER(NI_MACRO_STR, str)
    #endif

    #pragma message "An older version (" NI_MACRO_INLINE_STR(NIMBUSKIT_BASICS_VERSION) ") of NimbusKit's Basics was imported prior to this version (" NI_MACRO_INLINE_STR(NIMBUSKIT_BASICS_1_1_0) "). This may cause unexpected behavior. You may suppress this warning by defining NI_SUPPRESS_VERSION_WARNINGS"

  #endif // NIMBUSKIT_BASICS_VERSION check

#endif // #ifndef NI_SUPPRESS_VERSION_WARNINGS

#pragma mark - ~~~ Docs ~~~

/** @name Macros */

/**
 * Marks a method or property as deprecated to the compiler.
 *
 * To be used like so:
 *
 *     - (void)someMethod NI_DEPRECATED_METHOD("use someOtherMethod instead");
 *
 * Note that the macro expects a C-string (no @-prefix), not an Objective-C NSString.
 *
 * Any use of a deprecated method or property will flag a warning when compiling.
 *
 * @param msg A C-string explaining the deprecation. Used in the message
 *            "<selector> is deprecated: %s".
 * @fn #NI_DEPRECATED_METHOD(msg)
 * @ingroup Macros
 */

/**
 * Marks an initializer method as the designated initializer for a class.
 *
 * Causes Xcode to throw warnings if the initializer chain is not implemented correctly.
 * This macro can only be specified on a single initializer.
 *
 * @fn #NI_DESIGNATED_INITIALIZER
 * @ingroup Macros
 */

/**
 * Force a category to be loaded when an app starts up.
 *
 * Add this macro in every source file that only contains a category implementation.
 *
 * When linking to a library, Xcode will NOT link symbols from .m source that only contain
 * categories. In order to force Xcode to do this you must use the -all_load or -force_load linker
 * flags.
 *
 * By placing this macro in any category .m source you generate an empty class that will cause Xcode
 * to link all of the contents of the .m without requiring the -all_load or -force_load flags.
 *
 * See http://developer.apple.com/library/mac/#qa/qa2006/qa1490.html for more info.
 *
 * @fn #NI_FIX_CATEGORY_BUG(name)
 * @ingroup Macros
 */

/**
 * Checks whether a \p flag is set on \p value.
 *
 * This macro may be used to correctly check if a mask has a complex flag (more than one bit in the 
 * flag) enabled.
 *
 * It is a common error to check for a flag by simply using the & operator, but this only checks if
 * ANY subset of the flag's bits are set, not that ALL of them are set.
 *
 * By using the & operator and then comparing the result to the original \p flag, we ensure that all
 * bits in \p flag are set on \p value. This macro simplifies that check.
 *
 * @fn #NI_IS_FLAG_SET(value, flag)
 * @ingroup Macros
 */

/**
 * Creates an opaque UIColor object from a byte-value color definition.
 *
 * @fn #NI_RGBCOLOR(r,g,b)
 * @ingroup Macros
 */

/**
 * Creates a UIColor object from a byte-value color definition and alpha transparency.
 *
 * @fn #NI_RGBACOLOR(r,g,b,a)
 * @ingroup Macros
 */

/**
 * Creates an opaque UIColor object from a hex color definition of the form 0xRRGGBB.
 *
 * @fn #NI_HEXCOLOR(hex)
 * @ingroup Macros
 */

/**
 * Creates a UIColor object from a hex color definition of the form 0xRRGGBB with alpha
 * transparency.
 *
 * @fn #NI_HEXACOLOR(hex,a)
 * @ingroup Macros
 */

/** @name Querying the Debugger State */

/**
 * Returns a Boolean value indicating whether or not a debugger is attached to the process.
 *
 * @fn NIIsInDebugger()
 * @ingroup DebuggingTools
 */

/** @name Debug Assertions */

/**
 * If this assertion fails then this macro mimics a breakpoint when a debugger is attached.
 *
 * This macro may be used to safely pause execution of a program before it enters crashland.
 *
 * The source for this macro is only compiler when the DEBUG flag is defined.
 * If you wish to explicitly disable NI_DASSERT from being compiled, define NI_DISABLE_DASSERT in
 * your target's preprocessor macros.
 *
 * @fn #NI_DASSERT(xx)
 * @ingroup DebuggingTools
 */

/** @name Debug Logging */

/**
 * Only writes to the log when `DEBUG` is defined.
 *
 * When `DEBUG` is defined, this log method will always write to the log, regardless of log levels.
 * It is used by all of the other logging methods in Nimbus' debugging library.
 *
 * @fn #NI_DPRINT(xx, ...)
 * @ingroup DebuggingTools
 */

/**
 * Write the containing method's name to the log using NI_DPRINT.
 *
 * @fn #NI_DPRINTMETHODNAME()
 * @ingroup DebuggingTools
 */

/**
 * Only writes to the log if \p condition is satisified.
 *
 * This macro powers the level-based loggers. It can also be used for conditionally enabling
 * families of logs.
 *
 * @fn #NI_DCONDITIONLOG(condition, xx, ...)
 * @ingroup DebuggingTools
 */

/** @name Querying the Hardware */

/**
 * Checks whether the device the app is currently running on is an iPad or not.
 *
 * @returns YES if the device is an iPad.
 * @fn NIIsPad()
 * @ingroup SDKAvailability
 */

/**
 * Checks whether the device the app is currently running on is an
 * iPhone/iPod touch or not.
 *
 * @returns YES if the device is an iPhone or iPod touch.
 * @fn NIIsPhone()
 * @ingroup SDKAvailability
 */

/**
 * Returns the screen's scale.
 *
 * @fn NIScreenScale()
 * @ingroup SDKAvailability
 */

/**
 * Returns YES if the screen is a retina display, NO otherwise.
 *
 * @fn NIIsRetina()
 * @ingroup SDKAvailability
 */

/** @name Determining Feature Support */

/**
 * An SDK-agnostic mechanism for getting the tint color of a view.
 *
 * On pre-iOS 7 devices, will always return \p fallbackColor.
 * On devices that support -tintColor on UIView, returns `view.tintColor`.
 *
 * tintColor was introduced in iOS 7 as a global mechanism for changing tint color in an app.
 *
 * @fn NITintColorForViewWithFallback(UIView* view, UIColor* fallbackColor)
 * @ingroup SDKAvailability
 */

/**
 * Checks whether the device's OS version is at least the given version number.
 *
 * Useful for runtime checks of the device's version number.
 *
 * @attention Apple recommends using respondsToSelector where possible to check for
 *                 feature support. Use this method as a last resort.
 *
 * @param versionNumber  Any value of kCFCoreFoundationVersionNumber.
 * @fn NIDeviceOSVersionIsAtLeast(double versionNumber)
 * @ingroup SDKAvailability
 */
