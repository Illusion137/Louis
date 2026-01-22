#pragma once

#include <cstdio>
#include <print>
#include <expected>

// sll -> Standard Louie Library

/*
DEFINE-OPTIONS:
__LL_DEBUG: Enables Louis Debug Mode
__LL_NOLOG: Disables all Louis logging
*/

#define LL_SUCCESS 0

#define LL_NAMESPACE_BEGIN namespace ll {
#define LL_NAMESPACE_END };

#define LL_STRINGIFY(x) #x
#define LL_TOSTRING(x) LL_STRINGIFY(x)
#define LL_UNIMPLEMENTED static_assert(false, "UNIMPLEMENTED FUNCTION AT: (" __FILE__ ", " LL_TOSTRING(__LINE__) ")")

#define LL_TEST(__test_name, __func_call, __expected) \
auto test_value = __func_call;\
if( test_value == __expected ) fputs(std::format("TEST: [{}] => SUCCESS", __test_name), stdout); \
else fputs(std::format("TEST: [{}] => FAILURE w/ value ({})", __test_name, test_value).c_str(), stderr)

#define LL_INTERFACE template<class _C>
#define LL_ICAST static_cast<_C*>(this)
#define LL_IARGS(...) __VA_ARGS__
#define LL_MAKE_IFUNC(__type, __func_name, __args, __values) __type __func_name(__args){ return LL_ICAST->__func_name(__values); }
#define LL_USE_INTERFACE(__class, __interface) class __class final: public __interface<__class>

/// GCC ONLY
//#define _LL_ATTR_(...)      __attribute__((__VA_ARGS__))
//#define _LL_PURE            pure
//#define _LL_NO_SIDE_EFFECT  const
//#define _LL_CLASS_MODULE    visibility("internal")
//#define _LL_FASTCALL        fastcall
//#define _LL_FORCE_INLINE    always_inline
//#define _LL_LIKELY_EXEC     hot
//#define _LL_UNLIKELY_EXEC   cold
//#define _LL_FLATTEN         flatten
//#define _LL_NO_INLINE       noinline
//#define _LL_WARN_UNUSED     warn_unused_result
//#define _LL_NOPADDING       packed

#define LL_INTRINSIC_FORCE_INLINE __forceinline
#define LL_INTRINSIC_STDCALL      __stdcall
#define LL_INTRINSIC_FASTCALL     __fastcall
#define LL_INTRINSIC_VECTORCALL   __vectorcall
#define LL_INTRINSIC_RESTRICT     __restrict
#define LL_INTRINSIC_CDECL        __cdecl
#define LL_INTRINSIC_CALLBACK     __callback

#define LL_FORCE_INLINE           LL_INTRINSIC_FORCE_INLINE inline
#define LL_ALIGNAS(x)             alignas(x)
#define LL_MODULE                 static inline
#define LL_UNREACHABLE            __assume(false)
#define LL_NORETURN               [[noreturn]]
#define LL_CARRIES_DEP            [[carries_dependency]]
#define LL_FALLTHROUGH            [[fallthrough]]
#define LL_MAYBE_UNUSED           [[maybe_unused]]
#define LL_LIKELY                 [[likely]]
#define LL_UNLIKELY               [[unlikely]]
#define LL_DEPRECATED             [[deprecated]]
#define LL_NO_UNIQUE_ADDRESS      [[no_unique_address]]
#define LL_DEPRECATED_CAUSE(x)    [[deprecated(x)]]
#define LL_ASSUME(expr)           [[assume(expression)]]
#define LL_API extern
#define LL_CAPI extern "C"
#define LL_DLLEXPORT LL_CAPI  __declspec(dllexport)
#define LL_DLLIMPORT __declspec(dllimport)

#ifdef _DEBUG
    #define LL_NOEXCEPT noexcept(false)
#else
    #define LL_NOEXCEPT noexcept(true)
#endif

#ifdef LL_ENABLE_KW
#define llinterface   LL_INTERFACE class
#define forceinline  LL_FORCE_INLINE
#define stdcall      LL_INTRINSIC_STDCALL
#define fastcall     LL_INTRINSIC_FASTCALL
#define vectorcall   LL_INTRINSIC_VECTORCALL
#define unreachable  LL_UNREACHABLE
#endif // LL_ENABLE_KW

#define LL_INFO(fmt, ...) std::print("\033[0;36m[Info]: " fmt "\n\033[0m" __VA_OPT__(,) __VA_ARGS__)
#define LL_WARN(fmt, ...) std::print("\033[33m[Info]: " fmt "\n\033[0m" __VA_OPT__(,) __VA_ARGS__)

#ifdef LL_NOLOG
    #define LL_LOG(stream, log_type, msg, file, function, line)
    #define LL_LOGF(stream, format, ...) 
    #define LL_LOG_INFO(msg)
    #define LL_LOG_WARNING(msg)
    #define LL_LOG_ERROR(msg)
    #define LL_LOG_FATAL(msg) exit(1);
    #define LL_IF_LOG_INFO(expr, msg)
    #define LL_IF_LOG_WARNING(expr, msg)
    #define LL_IF_LOG_ERROR(expr, msg)
    #define LL_IF_LOG_FATAL(expr, msg)
#else // DO_LOGGING
    #define LL_ERR(stream, log_type, msg, file, function, line, ...) std::print(stream, "\033[0;31m[{}]: (FILE: [{}] : FUNCTION: [{}] : LINE: [{}])\n\033[0m", log_type __VA_OPT__(,) __VA_ARGS__, msg, file, function, line)
    #define LL_LOG(stream, log_type, msg, file, function, line, ...) std::print(stream, "[{}]: (FILE: [{}] : FUNCTION: [{}] : LINE: [{}])\n", log_type __VA_OPT__(,) __VA_ARGS__, msg, file, function, line)
    #define LL_LOG_INFO(msg, ...)                      LL_LOG(stdout, "INFO",  msg, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define LL_LOG_WARNING(msg, ...)                   LL_LOG(stdout, "WARN",  msg, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define LL_LOG_ERROR(msg, ...)                     LL_ERR(stderr, "ERRR", msg, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #define LL_LOG_FATAL(msg, ...)                     { LL_ERR(stderr, "FATAL", msg, __FILE__, __FUNCTION__, __LINE__); exit(1); }
    #define LL_IF_LOG_INFO(expr, msg, ...)    if(expr) LL_LOG_INFO(msg, __VA_ARGS__)
    #define LL_IF_LOG_WARNING(expr, msg, ...) if(expr) LL_LOG_WARNING(msg, __VA_ARGS__)
    #define LL_IF_LOG_ERROR(expr, msg, ...)   if(expr) LL_LOG_ERROR(msg, __VA_ARGS__)
    #define LL_IF_LOG_FATAL(expr, msg, ...)   if(expr) LL_LOG_FATAL(msg, __VA_ARGS__)
#endif // LL_NOLOG

#ifdef _DEBUG 
#define LL_STATIC_ASSERT(expr, msg) static_assert(expr, msg);
#define LL_CHECK(expr, msg) \
if(! (expr) ) {\
    LL_LOG_FATAL(msg);\
    exit(EXIT_FAILURE);\
}
#else
#define LL_CHECK(result, msg)
#define LL_STATIC_ASSERT(expr, msg)
#endif //_DEBUG

#define LL_ISOK(result) result == LL_SUCCESS