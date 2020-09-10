//根据opencv 4.4版本中的CommandLineParser修改而来

/** Designed for command line parsing

The sample below demonstrates how to use CommandLineParser:
@code
	CommandLineParser parser(argc, argv, keys);
	parser.about("Application name v1.0.0");

	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	int N = parser.get<int>("N");
	double fps = parser.get<double>("fps");
	String path = parser.get<String>("path");

	use_time_stamp = parser.has("timestamp");

	String img1 = parser.get<String>(0);
	String img2 = parser.get<String>(1);

	int repeat = parser.get<int>(2);

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}
@endcode

### Keys syntax

The keys parameter is a string containing several blocks, each one is enclosed in curly braces and
describes one argument. Each argument contains three parts separated by the `|` symbol:

-# argument names is a space-separated list of option synonyms (to mark argument as positional, prefix it with the `@` symbol)
-# default value will be used if the argument was not provided (can be empty)
-# help message (can be empty)

For example:

@code{.cpp}
	const String keys =
		"{help h usage ? |      | print this message   }"
		"{@image1        |      | image1 for compare   }"
		"{@image2        |<none>| image2 for compare   }"
		"{@repeat        |1     | number               }"
		"{path           |.     | path to file         }"
		"{fps            | -1.0 | fps for output video }"
		"{N count        |100   | count of objects     }"
		"{ts timestamp   |      | use time stamp       }"
		;
}
@endcode

Note that there are no default values for `help` and `timestamp` so we can check their presence using the `has()` method.
Arguments with default values are considered to be always present. Use the `get()` method in these cases to check their
actual value instead.

String keys like `get<String>("@image1")` return the empty string `""` by default - even with an empty default value.
Use the special `<none>` default value to enforce that the returned string must not be empty. (like in `get<String>("@image2")`)

### Usage

For the described keys:

@code{.sh}
	# Good call (3 positional parameters: image1, image2 and repeat; N is 200, ts is true)
	$ ./app -N=200 1.png 2.jpg 19 -ts

	# Bad call
	$ ./app -fps=aaa
	ERRORS:
	Parameter 'fps': can not convert: [aaa] to [double]
@endcode
 */
#pragma once
#include <string>
#include <vector>

typedef std::string String;

enum struct Param {
	BOOLEAN = 0,
	CHAR,
	UCHAR,
	INT,
	UINT,
	INT64,
	UINT64,
	FLOAT,
	REAL,
	STRING,
	
};

template<typename _Tp, typename _EnumTp = void> struct ParamType {};

template<> struct ParamType<bool>
{
	static const Param type = Param::BOOLEAN;
};

template<> struct ParamType<char>
{
	static const Param type = Param::CHAR;
};

template<> struct ParamType<unsigned char>
{
	static const Param type = Param::UCHAR;
};
template<> struct ParamType<int>
{
	static const Param type = Param::INT;
};

template<> struct ParamType<unsigned int>
{
	static const Param type = Param::UINT;
};

template<> struct ParamType<long long>
{
	static const Param type = Param::INT64;
};

template<> struct ParamType<unsigned long long>
{
	static const Param type = Param::UINT64;
};

template<> struct ParamType<float>
{
	static const Param type = Param::FLOAT;
};

template<> struct ParamType<double>
{
	static const Param type = Param::REAL;
};

template<> struct ParamType<String>
{
	static const Param type = Param::STRING;
};

struct CommandLineParserParams
{
public:
	String help_message;
	String def_value;
	std::vector<String> keys;
	int number;
};

//CommandLineParser Impl
struct Impl
{
	bool error;
	String error_message;
	String about_message;

	String path_to_app;
	String app_name;

	std::vector<CommandLineParserParams> data;

	std::vector<String> split_range_string(const String& str, char fs, char ss) const;
	std::vector<String> split_string(const String& str, char symbol = ' ', bool create_empty_item = false) const;

	void apply_params(const String& key, const String& value);
	void apply_params(int i, String value);

	void sort_params();
};

class CommandLineParser
{
public:

	/** @brief Constructor

	Initializes command line parser object

	@param argc number of command line arguments (from main())
	@param argv array of command line arguments (from main())
	@param keys string describing acceptable command line parameters (see class description for syntax)
	*/
	CommandLineParser(int argc, const char* const argv[], const String& keys);

	/** @brief Destructor */
	~CommandLineParser();

	/** @brief Returns application path

	This method returns the path to the executable from the command line (`argv[0]`).

	For example, if the application has been started with such a command:
	@code{.sh}
	$ ./bin/my-executable
	@endcode
	this method will return `./bin`.
	*/
	String getPathToApplication() const;

	/** @brief Access arguments by name

	Returns argument converted to selected type. If the argument is not known or can not be
	converted to selected type, the error flag is set (can be checked with @ref check).

	For example, define:
	@code{.cpp}
	String keys = "{N count||}";
	@endcode

	Call:
	@code{.sh}
	$ ./my-app -N=20
	# or
	$ ./my-app --count=20
	@endcode

	Access:
	@code{.cpp}
	int N = parser.get<int>("N");
	@endcode

	@param name name of the argument
	@param space_delete remove spaces from the left and right of the string
	@tparam T the argument will be converted to this type if possible

	@note You can access positional arguments by their `@`-prefixed name:
	@code{.cpp}
	parser.get<String>("@image");
	@endcode
	 */
	template <typename T>
	T get(const String& name, bool space_delete = true)
	{
		T val = T();
		getByName(name, space_delete, ParamType<T>::type, (void*)&val);
		return val;
	}

	/** @brief Access positional arguments by index

	Returns argument converted to selected type. Indexes are counted from zero.

	For example, define:
	@code{.cpp}
	String keys = "{@arg1||}{@arg2||}"
	@endcode

	Call:
	@code{.sh}
	./my-app abc qwe
	@endcode

	Access arguments:
	@code{.cpp}
	String val_1 = parser.get<String>(0); // returns "abc", arg1
	String val_2 = parser.get<String>(1); // returns "qwe", arg2
	@endcode

	@param index index of the argument
	@param space_delete remove spaces from the left and right of the string
	@tparam T the argument will be converted to this type if possible
	 */
	template <typename T>
	T get(int index, bool space_delete = true)
	{
		T val = T();
		getByIndex(index, space_delete, ParamType<T>::type, (void*)&val);
		return val;
	}

	/** @brief Check if field was provided in the command line

	@param name argument name to check
	*/
	bool has(const String& name) const;

	/** @brief Check for parsing errors

	Returns false if error occurred while accessing the parameters (bad conversion, missing arguments,
	etc.). Call @ref printErrors to print error messages list.
	 */
	bool check() const;

	/** @brief Set the about message

	The about message will be shown when @ref printMessage is called, right before arguments table.
	 */
	void about(const String& message);

	/** @brief Print help message

	This method will print standard help message containing the about message and arguments description.

	@sa about
	*/
	void printMessage() const;

	/** @brief Print list of errors occurred

	@sa check
	*/
	void printErrors() const;

protected:
	void getByName(const String& name, bool space_delete, Param type, void* dst);
	void getByIndex(int index, bool space_delete, Param type, void* dst);

	Impl impl;
};

