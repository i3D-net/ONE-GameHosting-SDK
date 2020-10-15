#include <catch.hpp>
#include <one/arcus/allocator.h>
#include <one/arcus/types.h>

using namespace i3d::one;

namespace {

// Simple wrapper around an int value to allow confirmation of allocator
// behavior.
struct IntWrapper {
    IntWrapper(int val_in, bool &destruct_flag_in)
        : val(val_in), destruct_flag(destruct_flag_in) {}
    ~IntWrapper() {
        destruct_flag = true;
    }
    int val;
    bool &destruct_flag;
};

size_t _allocated_size = 0;
void *_last_allocated = nullptr;
void *_last_freed = nullptr;

// Set the one allocator overrides.
void init_allocator() {
    _allocated_size = 0;
    _last_allocated = nullptr;
    _last_freed = nullptr;
    auto alloc = [](size_t bytes) -> void * {
        _allocated_size += bytes;
        _last_allocated = ::operator new(bytes);
        return _last_allocated;
    };
    allocator::set_alloc(alloc);

    auto free = [](void *p) {
        _last_freed = p;
        ::operator delete(p);
    };
    allocator::set_free(free);
}

// Util class to init the test allocator overrides and reset them to defaults
// upon destruction.
class ScopedAllocationSetter final {
public:
    ScopedAllocationSetter() {
        init_allocator();
    }
    ~ScopedAllocationSetter() {
        allocator::reset_overrides();
        _allocated_size = 0;
        _last_allocated = nullptr;
        _last_freed = nullptr;
    }
};

}  // namespace

TEST_CASE("allocator basics", "[arcus]") {
    const size_t expected_size = sizeof(IntWrapper);

    SECTION("Alloc/free") {
        ScopedAllocationSetter setter;

        void *p = allocator::alloc(expected_size);
        REQUIRE(p != nullptr);
        REQUIRE(_allocated_size == expected_size);

        allocator::free(p);
        REQUIRE(_last_freed == _last_allocated);
        REQUIRE(_last_freed == p);
    }

    SECTION("Create/Destroy (new/delete equivalents)") {
        ScopedAllocationSetter setter;

        bool destruct_flag = false;
        IntWrapper *p = allocator::create<IntWrapper>(1, destruct_flag);
        REQUIRE(p != nullptr);
        REQUIRE(_allocated_size == expected_size);
        REQUIRE(p->val == 1);

        allocator::destroy(p);
        REQUIRE(_last_freed == _last_allocated);
        REQUIRE(_last_freed == p);
        REQUIRE(destruct_flag == true);
    }
}

TEST_CASE("custom string", "[arcus]") {
    SECTION("default allocation") {
        {
            String val("test");
            REQUIRE(val == "test");
            REQUIRE(_allocated_size == 0);
            REQUIRE(_last_allocated == nullptr);
        }
        REQUIRE(_last_freed == nullptr);
    }

// Linux appears to do complete stack allocation for small strings, while
// windows appears to do both stack + a proxy.
#ifdef WINDOWS
    SECTION("overridden allocation") {
        ScopedAllocationSetter setter;
        {
            String val("test");
            REQUIRE(val == "test");
            REQUIRE(_allocated_size > 0);
        }
        REQUIRE(_last_freed == _last_allocated);
        // Unfortunately not easy to test the _last_freed value against the
        // string val directly. std::basic_string has a private _Myproxy member
        // variable that stores the allocated data. At runtime however during
        // observed testing here, the actual private _Buf value that is returned
        // by functions like data or c_str refers to a different address
        // (possibly a duplicated stack value address for optimization).
    }
#endif
}
