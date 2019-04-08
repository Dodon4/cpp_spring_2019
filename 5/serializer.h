#include <iostream>

enum class Error
{
    NoError,
    CorruptedArchive
};


class Serializer
{
private:
    static constexpr char Separator = ' ';
    std::ostream& out_;

    template <class T,  class... ArgsT>
	Error process(T&& value, ArgsT&& ... args)
	{
		if (process(std::forward<T>(value)) == Error::NoError)
			return process(std::forward<ArgsT>(args)...);
		else
			return Error::CorruptedArchive;
	}
    Error process(uint64_t value)
    {
    	auto& text = out_ << value << Separator;
		if (text)
			return Error::NoError;
		else
			return Error::CorruptedArchive;
    }
    Error process(bool value)
    {
    	auto& text = out_ << (value ? "true" : "false") << Separator;

		if (text)
			return Error::NoError;
		else
			return Error::CorruptedArchive;
    }
    Error process()
    {
    	return Error::NoError;
    }
public:
    explicit Serializer(std::ostream& out) : out_(out)
    {
    }
    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }
    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
	return process(std::forward<ArgsT>(args)...);
    }
};



class Deserializer
{
private:
    std::istream& in_;

	 template <class T,  class... ArgsT>
	 Error process(T&& value, ArgsT&& ... args)
	{
		if (process(std::forward<T>(value)) == Error::NoError)
			return process(std::forward<ArgsT>(args)...);
		else
			return Error::CorruptedArchive;
	}
	Error process(uint64_t& value)
	{
		std::string text;
		in_ >> text;
		for (auto t : text)
			if (t >= '0' && t <= '9') {
				value = value * 10 + t - '0';
			}
			else
				return Error::CorruptedArchive;
		return Error::NoError;
	}
    	Error process(bool& value)
	{
	    std::string text;
	    in_ >> text;

	    if (text == "true")
	        value = true;
	    else if (text == "false")
	        value = false;
	    else
	        return Error::CorruptedArchive;
	    return Error::NoError;
	}
	template <class... ArgsT>
	Error process()
	{
		return Error::CorruptedArchive;
	}
public:
	explicit Deserializer(std::istream& in) : in_(in)
	{
	}
	template <class T>
	Error load(T& object)
	{
		return object.serialize(*this);
	}
	template <class... ArgsT>
	Error operator()(ArgsT&&... args)
	{
		return process(std::forward<ArgsT>(args)...);
	}
};
