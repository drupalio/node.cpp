clang++-7 test_co_await.cpp co_await_hierarchy.cpp ../../../safe_memory/library/gcc_lto_workaround/gcc_lto_workaround.cpp ../../../safe_memory/library/src/iibmalloc/src/iibmalloc_linux.cpp  ../../../safe_memory/library/src/iibmalloc/src/page_allocator_linux.cpp ../../../safe_memory/library/src/iibmalloc/src/foundation/src/log.cpp ../../../safe_memory/library/src/iibmalloc/src/foundation/3rdparty/fmt/src/format.cc -I../../../safe_memory/library/src/iibmalloc/src/foundation/include -I../../../safe_memory/library/src/iibmalloc/src/foundation/3rdparty/fmt/include -I../../../safe_memory/library/src/iibmalloc/src -I../../../safe_memory/library/src -I../../../include -I../../../src -std=gnu++17 -g -Wall -Wextra  -Wno-c++17-extensions -fcoroutines-ts -stdlib=libc++ -lc++experimental -Wno-unused-variable -Wno-unused-parameter -Wno-empty-body  -DNDEBUG -O3 -flto -lpthread -o test.bin