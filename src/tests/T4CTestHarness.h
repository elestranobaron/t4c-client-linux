#pragma once

#include <cmath>
#include <cstdio>

namespace t4c_test {

inline int g_run = 0;
inline int g_failed = 0;

inline void registerFailure(const char *file, const int line, const char *expr) {
    std::fprintf(stderr, "FAIL %s:%d: %s\n", file, line, expr);
    ++g_failed;
}

template <typename A, typename B>
inline bool eq(const A &a, const B &b) {
    return a == b;
}

inline bool nearEq(const float a, const float b, const float eps = 0.0002f) {
    return std::fabs(a - b) <= eps;
}

}  // namespace t4c_test

#define T4C_ASSERT(expr)                                                                          \
    do {                                                                                          \
        ++t4c_test::g_run;                                                                        \
        if (!(expr)) {                                                                            \
            t4c_test::registerFailure(__FILE__, __LINE__, #expr);                                 \
        }                                                                                         \
    } while (0)

#define T4C_ASSERT_EQ(a, b) T4C_ASSERT(t4c_test::eq((a), (b)))
#define T4C_ASSERT_NEAR(a, b) T4C_ASSERT(t4c_test::nearEq((a), (b)))

#define T4C_TEST(name) void t4c_test_##name()

#define T4C_RUN(name)                                                                               \
    do {                                                                                          \
        std::fprintf(stderr, "  %s\n", #name);                                                    \
        t4c_test_##name();                                                                        \
    } while (0)

inline int t4c_test_main_finish() {
    std::fprintf(stderr, "\n%d checks, %d failures\n", t4c_test::g_run, t4c_test::g_failed);
    return t4c_test::g_failed == 0 ? 0 : 1;
}
