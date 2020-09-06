#pragma once
#include "filesystem.hpp"
#include <numeric>
#include <algorithm>
#include <map>
#include <set>

using vector_string = std::vector<std::string>;

const std::string help = "Possible commands: \n\t"
"--help                                    Shows this message\n\t"
"--dir                                     Shows directory\n\t"
"--cd [path]                               Moves to directory\n\t"
"--print [filename]                        Shows file content\n\t"
"--mkdir [directory name]                  Creates a directory in current directory\n\t"
"--createnew [new filename] [file size]    Creates a file with a specific size\n\t"
"--rd [directory name]                     Removes directory\n\t"
"--del [filename]                          Removes file\n\t"
"--rename [old filename] [new filename]    Renames file in current directory\n"
;
constexpr auto max_cmd_argc = 2;

enum class user_input_type
{
	help,
	dir,
	cd,
	print,
	mkdir,
	createnew,
	rd,
	del,
	rename
};

std::istream& operator>>(std::istream& is, user_input_type& type);

const std::map<std::string, user_input_type> commands{
#define X(cmd) "--"#cmd, user_input_type::##cmd
	{X(help)},
	{X(dir)},
	{X(cd)},
	{X(print)},
	{X(createnew)},
	{X(mkdir)},
	{X(rd)},
	{X(del)},
	{X(rename)}
#undef X
};

inline user_input_type to_input_type(const std::string& s)
{
	const auto it = commands.find(s);
	if (it != commands.end()) {
		return it->second;
	}
	return user_input_type::help;
}

class user_input
{
public:
	user_input() { args_.reserve(max_cmd_argc); }
	user_input_type get_type() const noexcept { return type_; }
	vector_string get_args() const { return args_; }
	friend std::istream& operator>>(std::istream& is, user_input& user_input)
	{
		std::string s;
		is >> user_input.type_;
		switch (user_input.type_)
		{
#define READ_ARG	if (!(is >> s)) { (user_input).type_ = user_input_type::help; }    \
					else { (user_input).args_.emplace_back(std::move(s)); }
		case user_input_type::help:
		case user_input_type::dir:
			break;
		case user_input_type::cd:
		case user_input_type::print:
		case user_input_type::mkdir:
		case user_input_type::rd:
		case user_input_type::del:
			READ_ARG
			break;
		case user_input_type::createnew:
		case user_input_type::rename:
			READ_ARG
			READ_ARG
			break;
		default:;
#undef READ_ARG
		}
		return is;
	}
private:
	vector_string args_;
	user_input_type type_{};
};

inline std::istream& operator>>(std::istream& is, user_input_type& type)
{
	std::string cmd;
	if (!(is >> cmd)) {
		type = user_input_type::help;
		return is;
	}
	std::for_each(cmd.begin(), cmd.end(), ::tolower);
	type = to_input_type(cmd);
	return is;
}

inline void handle_user_input(const int argc, const char*const*const argv, filesystem& fs)
{
	const vector_string args(argv + 1, argv + argc);
	std::istringstream iss(std::accumulate(args.begin(), args.end(), std::string{}, [](auto & s, const auto& piece) {return s += ' ' + piece; }));
	user_input user_input;
	iss >> user_input;
	try
	{
		switch (user_input.get_type())
		{
#define CASE(cmd) case user_input_type::##cmd: fs.##cmd(user_input.get_args()); break;
		case user_input_type::help: std::cout << help << std::flush; break;
		CASE(dir)
		CASE(cd)
		CASE(print)
		CASE(mkdir)
		CASE(createnew)
		CASE(rd)
		CASE(del)
		CASE(rename)
		default:;
#undef CASE
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
