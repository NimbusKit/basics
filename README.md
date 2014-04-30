<!--dox @defgroup NimbusKitBasics NimbusKit Basics -->

<div id="github" feature="Basics"></div>

![](https://github.com/NimbusKit/Basics/raw/master/docs/gfx/banner.gif "Basics")

The single header, `NimbusKitBasics.h`, is designed to be dropped into any source code or library. Within, you'll find plenty of common app-development goodies.

Adding it to your Project
=========================

Simply drag `NimbusKitBasics.h` from the `src` directory into your project and import it.

```objc
#import "NimbusKitBasics.h"
```

What's Included
===============

Compiler Features
-----------------

The following compiler features allow you to express more when you're designing Objective-C software.

- Use `NI_DEPRECATED_METHOD(msg)` to deprecate individual methods on classes with a message explaning a suggested alternative.
- Use `NI_DESIGNATED_INITIALIZER` to enforce correct initializer chaining.
- Use `NS_REQUIRES_SUPER` to enforce super calls when sub-classes override important methods.

Example use of compiler features:

```objc
@interface SomeClass

- (instancetype)initWithArg:(id)arg NI_DESIGNATED_INITIALIZER;

- (void)someMethod;
- (void)someDeprecatedMethod NI_DEPRECATED_METHOD("use someMethod instead");

@end

@interface SomeClass (Subclassing)

- (void)someMethodToBeOverwritten NS_REQUIRES_SUPER; // Ensures child implementations call super.

@end
```


Avoid requiring the -all_load and -force_load flags
---------------------------------------------------

Category-only compilation units can introduce the need for the `-all_load` and `-force_load` linker flags because the application will not load these category-only units without them.

To get around this you can provide an empty class implementation in the `.m` file. For example:

```objc
@interface BogusClass
@end
@implementation BogusClass
@end

@implementation UIViewController (MyCustomCategory)
...
@end
```

`MyCustomCategory` will now be linked as expected thanks to the BogusClass contained within the same compilation unit.

`NI_FIX_CATEGORY_BUG(name)` is a macro that you include once in your category-only `.m` file to save you the trouble of having to write a bogus class for every compilation unit. The provided name must be globally unique in order to avoid causing duplicate symbol errors at link time.

```objc
NI_FIX_CATEGORY_BUG(UIViewController_MyCustomCategory);

@implementation UIViewController (MyCustomCategory)
...
@end
```


Debugging Tools
---------------

The debugging tools provided here allow for **debug-build-only sanity checks and logs**. If you require production assertions then you should use the standard `NSAssert`.

Special consideration:

> All of these macros only do something if the `DEBUG` macro is defined. The recommended way to enable the debug tools is to specify DEBUG in the "Preprocessor Macros" field in your application's Debug target settings. Be careful not to set this for your release or app store builds because this will enable code that may cause your app to be rejected.

### Debug Assertions

Debug assertions are a light-weight "sanity check". They won't crash the app, nor will they be included in release builds. They *will* halt the app's execution when debugging (like a conditional breakpoint) so that you can inspect the values that caused the failure.

```objc
 NI_DASSERT(statement);
```

If `statement` is false, the statement will be written to the log and, if a debugger is attached, the app will break on the assertion line.

![](https://github.com/NimbusKit/Basics/raw/master/docs/gfx/NI_DASSERT.png "NI_DASSERT example")


### Debug Logging

```objc
 NI_DPRINT(@"formatted log text %d", param1);
```

Print the given formatted text to the log.

```objc
 NI_DPRINTMETHODNAME();
```

Print the current method name to the log.

```objc
 NI_DCONDITIONLOG(statement, @"formatted log text %d", param1);
```

Print the given formatted text to the log if `statement` is YES. This is effectively a combination of NI_DASSERT and NI_DPRINT.


Creating Byte- and Hex-based Colors
-----------------------------------

Nimbus provides the RGBCOLOR and RGBACOLOR macros for easily creating UIColor objects
with byte and hex values.

### Examples

```objc
UIColor* color = NI_RGBCOLOR(255, 128, 64); // Fully opaque orange
UIColor* color = NI_HEXCOLOR(0xFF8040);

UIColor* color = NI_RGBACOLOR(255, 128, 64, 0.5); // Orange with 50% transparency
UIColor* color = NI_HEXACOLOR(0xFF8040, 0.5);
```

Run-Time Checks
---------------

- `NIIsPad()` returns YES on iPads.
- `NIIsPhone()` returns YES on iPhones and iPod touches.
- `NIScreenScale()` returns the main screen's scale.
- `NIIsRetina()` returns YES if the main screen has a retina display.
- `NITintColorForViewWithFallback(view, fallbackColor)` pre-iOS 7-safe mechanism for getting the tint color from a view (uses fallbackColor on older devices).

SDK Availability
----------------

NI_IOS macros are defined in parallel to their __IPHONE_ counterparts as a consistently-defined
means of checking __IPHONE_OS_VERSION_MAX_ALLOWED.

For example:

```objc
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= NIIOS_3_2
  // This code will only compile on versions >= iOS 3.2
#endif
```

32/64 Bit Math
--------------

We'd all love to use tgmath.h for its lovely type-generic methods, but due to a bug in the way Xcode's new modules feature works you have to choose one or the other. [Relevant open radar](http://www.openradar.me/16744288).

In the meantime, all of the standard math functions are explicitly mapped to use the tgmath equivalents when you import NimbusKitBasics. Apple may fix the bug with modules/tgmath, at which point you can disable NimbusKit Basics' remapping by defining `NI_DISABLE_GENERIC_MATH` in your project's preprocessor macros.

Version History
===============

1.2.0 on Apr 30, 2014
-----

32/64 bit math using standard math function names (thanks to [@steipete](http://twitter.com/steipete) for [pointing this out](https://gist.github.com/steipete/11403178)!).

Bumping the version because of the API changes, even though this is a fairly minor change.

1.1.0 on Apr 30, 2014
-----

32/64 bit math!

- NI_DASSERT are now automatically disabled when running unit tests.

1.0.0 on Apr 29, 2014
-----

Initial release. Includes:

- Compiler features.
- Debugging tools.
- Runtime checks.
- iOS SDK versions.

Credits
=======

Basics was boiled down and trimmed from Nimbus 1.2.0 by [Jeff Verkoeyen](http://jeffverkoeyen.com/) ([@featherless](http://twitter.com/)).

Contributors
------------

You can be the first! [Open a pull request now](https://github.com/NimbusKit/Basics/compare/).

License
=======

NimbusKit's Basics is licensed under the BSD three-clause license. For a more permissive license (no redistribution of copyright notice, etc.), please contact Jeff at jverkoey@gmail.com for pricing.
