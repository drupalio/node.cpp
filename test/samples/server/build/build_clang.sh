clang++-7 ../../../../src/infra_main.cpp ../user_code/NetSocket.cpp ../../../../src/net.cpp ../../../../src/infrastructure.cpp ../../../../src/tcp_socket/tcp_socket.cpp ../../../../safe_memory/library/gcc_lto_workaround/gcc_lto_workaround.cpp ../../../../safe_memory/library/src/iibmalloc/src/iibmalloc_linux.cpp  ../../../../safe_memory/library/src/iibmalloc/src/page_allocator_linux.cpp ../../../../safe_memory/library/src/iibmalloc/src/foundation/src/log.cpp ../../../../safe_memory/library/src/iibmalloc/src/foundation/src/std_error.cpp ../../../../safe_memory/library/src/iibmalloc/src/foundation/src/safe_memory_error.cpp ../../../../safe_memory/library/src/iibmalloc/src/foundation/src/tagged_ptr_impl.cpp ../../../../safe_memory/library/src/iibmalloc/src/foundation/3rdparty/fmt/src/format.cc -I../../../../safe_memory/library/src/iibmalloc/src/foundation/include -I../../../../safe_memory/library/src/iibmalloc/src/foundation/3rdparty/fmt/include -I../../../../safe_memory/library/src/iibmalloc/src -I../../../../safe_memory/library/src -I../../../../include -I../../../../src -std=c++17 -g -Wall -Wextra -Wno-unknown-attributes -Wno-c++17-extensions -fcoroutines-ts -stdlib=libc++ -lc++experimental -Wno-unused-variable -Wno-unused-parameter -Wno-empty-body -DNDEBUG -O3 -flto -lpthread -o server.bin