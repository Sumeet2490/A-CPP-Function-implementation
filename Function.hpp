#include <stdexcept>

#ifndef FUNCTION_HPP_
#define FUNCTION_HPP_

namespace cs540 {

class BadFunctionCall: public std::exception {
public:
	virtual const char* What() const throw () {
		return "Bad Function Call";
	}
};

template<typename ResultType, typename ... ArgumentTypes>
class Functor_base {
public:
	virtual ResultType operator()(ArgumentTypes ... args) = 0;
	virtual Functor_base *construct() const = 0;
	virtual ~Functor_base() {
	}
};

template<typename FunctionType, typename ResultType, typename ... ArgumentTypes>
class Functor: public Functor_base<ResultType, ArgumentTypes...> {
public:
	FunctionType func;
	Functor(const FunctionType &f) :
			func(f) {
	}

	ResultType operator()(ArgumentTypes ... args) {
		return func(args...);
	}

	Functor *construct() const {
		return new Functor { func };
	}

};

template<typename FunctionType>
struct typeset {
	typedef FunctionType type;
};

template<typename ResultType, typename ...ArgumentTypes>
struct typeset<ResultType(ArgumentTypes...)> {
	typedef ResultType (*type)(ArgumentTypes...);
};

template<typename >
class Function;

template<typename ResultType, typename ... ArgumentTypes>
class Function<ResultType(ArgumentTypes...)> {

private:

	Functor_base<ResultType, ArgumentTypes...> *functor;

public:

	Function();
	template<typename FunctionType> Function(FunctionType);
	Function(const Function &);
	Function &operator=(const Function &);
	template<typename FunctionType> Function &operator=(const FunctionType &);
	~Function();
	ResultType operator()(ArgumentTypes...);
	explicit operator bool() const;

	friend bool operator==(const Function<ResultType(ArgumentTypes...)> &f,
			std::nullptr_t) {
		return !f;
	}

	friend bool operator==(std::nullptr_t,
			const Function<ResultType(ArgumentTypes...)> & f) {
		return !f;
	}

	friend bool operator!=(const Function<ResultType(ArgumentTypes...)> &f,
			std::nullptr_t) {
		return bool(f);
	}

	friend bool operator!=(std::nullptr_t,
			const Function<ResultType(ArgumentTypes...)> &f) {
		return bool(f);
	}
};

template<typename ResultType, typename ... ArgumentTypes>
Function<ResultType(ArgumentTypes...)>::Function() {
	functor = nullptr;
}

template<typename ResultType, typename ... ArgumentTypes>
template<typename FunctionType>
Function<ResultType(ArgumentTypes...)>::Function(FunctionType func) {

	functor = new Functor<typename typeset<FunctionType>::type, ResultType,
			ArgumentTypes...>(func);
}

template<typename ResultType, typename ... ArgumentTypes>
Function<ResultType(ArgumentTypes...)>::Function(const Function &f) {

	if (f.functor == nullptr) {
		functor = nullptr;
	} else {
		functor = f.functor->construct();
	}

}

template<typename ResultType, typename ... ArgumentTypes>
Function<ResultType(ArgumentTypes...)>& Function<ResultType(ArgumentTypes...)>::operator=(
		const Function &f) {
	if (this == &f) {
		return *this;
	}
	delete functor;
	functor = f.functor->construct();
	return *this;
}

template<typename ResultType, typename ... ArgumentTypes>
template<typename FunctionType>
Function<ResultType(ArgumentTypes...)>& Function<ResultType(ArgumentTypes...)>::operator=(
		const FunctionType &f) {
	auto *temp = new Functor<typename typeset<FunctionType>::type, ResultType,
			ArgumentTypes...>(f);
	delete functor;
	functor = temp;
	return *this;
}

template<typename ResultType, typename ... ArgumentTypes>
Function<ResultType(ArgumentTypes...)>::~Function() {
	delete functor;
}

template<typename ResultType, typename ... ArgumentTypes>
ResultType Function<ResultType(ArgumentTypes...)>::operator()(
		ArgumentTypes ... args) {
	if (functor == nullptr) {
		throw cs540::BadFunctionCall();
	}
	return (*functor)(args...);
}

template<typename ResultType, typename ... ArgumentTypes>
Function<ResultType(ArgumentTypes...)>::operator bool() const {
	if (functor == nullptr) {
		return false;
	}
	try {
		(*functor)();
	} catch (...) {
		return false;
	}
	return true;
}

}
#endif
