#include <iostream>
#include <csetjmp>
#include <vector>
#include <memory>
#include <functional>


namespace {
class ContextHolder {
    std::jmp_buf _context;
    std::vector<std::function<void()>> _destructor_callers;

public:
    std::jmp_buf& operator*() {
        return _context;
    }

    void FreeStack() {
        for (auto& desctuctor_caller: _destructor_callers) {
            desctuctor_caller();
        }
        _destructor_callers.clear();
    }

    void ClearDestructorsList() {
        _destructor_callers.clear();
    }

    template <typename T>
    void RegisterVariable(std::shared_ptr<T> variable) {
        _destructor_callers.emplace_back([mvariable = std::move(variable)]() mutable {
            mvariable.reset();
        });
    }
};

class ContextManager {
    std::vector<ContextHolder> _contexts;

public:
    ContextManager() {
        _contexts.emplace_back();
        int status = setjmp(*_contexts.back());
        if (status) {
            std::cout << "uncaught exception with code " << status << "\n";
            exit(1);
        }
    }

    ContextHolder& Current() {
        return _contexts.back();
    }

    void AddContext() {
        _contexts.emplace_back();
    }

    void PopContext() {
        _contexts.back().FreeStack();
        _contexts.pop_back();
    }

    int size() const {
        return _contexts.size();
    }

    ~ContextManager() {
        for (auto& context: _contexts) {
            context.ClearDestructorsList();
        }
    }
};
ContextManager context_manager;
const int kContextInitStatus = 0;

int __status__;

template <int ExpectedStatus>
class TryCatchBlock {
    const int _status;
    size_t _iterations;

public:
    TryCatchBlock(): _status(__status__), _iterations(0) {
        if (_status != kContextInitStatus) {
            __status__ = kContextInitStatus;
        }
    }

    operator bool() const {
        return _status == ExpectedStatus && _iterations < 1;
    }

    TryCatchBlock& operator++() {
        ++_iterations;
        return *this;
    }

    ~TryCatchBlock() {}
};

template<>
TryCatchBlock<kContextInitStatus>::TryCatchBlock(): _status(__status__), _iterations(0) {}
template<>
TryCatchBlock<kContextInitStatus>::~TryCatchBlock() {
    if (_status != kContextInitStatus) {
        context_manager.PopContext();
    }
}


struct ThrowCaller {
    ThrowCaller& operator=(int status) {
        std::longjmp(*context_manager.Current(), status);
    }
};
ThrowCaller thrower;


}  // namespace


#define TRY \
    context_manager.AddContext();\
    __status__ = setjmp(*context_manager.Current());\
    for (TryCatchBlock<kContextInitStatus> try_block; try_block; ++try_block)


#define CATCH(error_code) \
    if (error_code == __status__) \
        for (TryCatchBlock<error_code> catch_block; catch_block; ++catch_block)


#define THROW thrower =


#define INIT(Type, variable) \
    auto variable_ptr = std::make_shared<Type>();\
    Type& variable = *variable_ptr;\
    context_manager.Current().RegisterVariable<Type>(std::move(variable_ptr));


#define FINISH \
    if (__status__ != kContextInitStatus) THROW __status__;