#include <sqlpp11/thread_pool.h>
#include <cassert>
#include <iostream>

int ThreadPool(int /*argc*/, char** /*argv*/)
{
	auto& pool = sqlpp::detail::thread_pool;
	std::cout << "enqueue sleep(100ms)\n";
	pool.enqueue([](){std::this_thread::sleep_for(std::chrono::milliseconds{100});});
	std::cout << "enqueue sleep(100ms)\n";
	pool.enqueue([](){std::this_thread::sleep_for(std::chrono::milliseconds{100});});
	std::cout << "sleep(1s)\n";
	std::this_thread::sleep_for(std::chrono::seconds{1});
	std::cout << "enqueue sleep(100ms)\n";
	pool.enqueue([](){std::this_thread::sleep_for(std::chrono::milliseconds{100});});
	std::cout << "sleep(6s)\n";
	std::this_thread::sleep_for(std::chrono::seconds{6});
	std::cout << "enqueue empty lambda\n";
	pool.enqueue([](){});
	std::cout << "enqueue future<int> with return\n";
	auto f = pool.enqueue_future([](){ std::this_thread::sleep_for(std::chrono::milliseconds{100}); return 1; });
	std::cout << "future: " << f.get() << '\n';
	std::cout << "enqueue future<int> with exception\n";
	f = pool.enqueue_future([]() -> int { std::this_thread::sleep_for(std::chrono::milliseconds{100}); throw std::runtime_error{"throw test"}; });
	try{
		f.get();
		return 1;
	}catch(const std::exception& e){
		std::cout << "future: " << e.what() << '\n';
	}
	std::cout << "manually stopping thread_pool\n";
	pool.stop();
	std::cout << "enqueueing on stopped thread_pool (throws)\n";
	try{
		pool.enqueue([](){});
		return 1;
	}catch(const std::exception& e){}
	std::cout << "all tests successful\n";
	return 0;
}
