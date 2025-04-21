#include <cassert>
#include <iostream>

// Custom assert with message macro
#define ASSERT_MSG(expr, msg) \
    if (!(expr)) { \
        std::cerr << "Assertion failed: " << #expr << "\nMessage: " << msg << "\n"; \
        assert(expr); \
    }