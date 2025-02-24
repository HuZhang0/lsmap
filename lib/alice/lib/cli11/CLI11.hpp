#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

// This file was generated using MakeSingleHeader.py in CLI11/scripts
// from: v1.3.0

// This has the complete CLI library in one file.

#include <sys/stat.h>
#include <deque>
#include <set>
#include <iostream>
#include <string>
#include <iterator>
#include <tuple>
#include <locale>
#include <functional>
#include <numeric>
#include <iomanip>
#include <sys/types.h>
#include <utility>
#include <exception>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <memory>

// From CLI/Version.hpp

namespace CLI {

// Note that all code in CLI11 must be in a namespace, even if it just a define.

#define CLI11_VERSION_MAJOR 1
#define CLI11_VERSION_MINOR 3
#define CLI11_VERSION_PATCH 0
#define CLI11_VERSION "1.3.0"

} // namespace CLI

// From CLI/StringTools.hpp

namespace CLI {
namespace detail {

// Based on http://stackoverflow.com/questions/236129/split-a-string-in-c
/// Split a string by a delim
inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    // Check to see if empty string, give consistent result
    if(s.empty())
        elems.emplace_back("");
    else {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
    return elems;
}

/// Simple function to join a string
template <typename T> std::string join(const T &v, std::string delim = ",") {
    std::ostringstream s;
    size_t start = 0;
    for(const auto &i : v) {
        if(start++ > 0)
            s << delim;
        s << i;
    }
    return s.str();
}

/// Join a string in reverse order
template <typename T> std::string rjoin(const T &v, std::string delim = ",") {
    std::ostringstream s;
    for(size_t start = 0; start < v.size(); start++) {
        if(start > 0)
            s << delim;
        s << v[v.size() - start - 1];
    }
    return s.str();
}

// Based roughly on http://stackoverflow.com/questions/25829143/c-trim-whitespace-from-a-string

/// Trim whitespace from left of string
inline std::string &ltrim(std::string &str) {
    auto it = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale()); });
    str.erase(str.begin(), it);
    return str;
}

/// Trim anything from left of string
inline std::string &ltrim(std::string &str, const std::string &filter) {
    auto it = std::find_if(str.begin(), str.end(), [&filter](char ch) { return filter.find(ch) == std::string::npos; });
    str.erase(str.begin(), it);
    return str;
}

/// Trim whitespace from right of string
inline std::string &rtrim(std::string &str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale()); });
    str.erase(it.base(), str.end());
    return str;
}

/// Trim anything from right of string
inline std::string &rtrim(std::string &str, const std::string &filter) {
    auto it =
        std::find_if(str.rbegin(), str.rend(), [&filter](char ch) { return filter.find(ch) == std::string::npos; });
    str.erase(it.base(), str.end());
    return str;
}

/// Trim whitespace from string
inline std::string &trim(std::string &str) { return ltrim(rtrim(str)); }

/// Trim anything from string
inline std::string &trim(std::string &str, const std::string filter) { return ltrim(rtrim(str, filter), filter); }

/// Make a copy of the string and then trim it
inline std::string trim_copy(const std::string &str) {
    std::string s = str;
    return trim(s);
}

/// Make a copy of the string and then trim it, any filter string can be used (any char in string is filtered)
inline std::string trim_copy(const std::string &str, const std::string &filter) {
    std::string s = str;
    return trim(s, filter);
}
/// Print a two part "help" string
inline void format_help(std::stringstream &out, std::string name, std::string description, size_t wid, bool rst) {
    if (!rst)
        name = "  " + name;
    out << std::setw(static_cast<int>(wid)) << std::left << name;
    if(!description.empty()) {
        if(name.length() >= wid)
            out << std::endl << std::setw(static_cast<int>(wid)) << "";
        out << description;
    }
    out << std::endl;
}

/// Verify the first character of an option
template <typename T> bool valid_first_char(T c) { return std::isalpha(c, std::locale()) || c == '_'; }

/// Verify following characters of an option
template <typename T> bool valid_later_char(T c) {
    return std::isalnum(c, std::locale()) || c == '_' || c == '.' || c == '-';
}

/// Verify an option name
inline bool valid_name_string(const std::string &str) {
    if(str.empty() || !valid_first_char(str[0]))
        return false;
    for(auto c : str.substr(1))
        if(!valid_later_char(c))
            return false;
    return true;
}

/// Return a lower case version of a string
inline std::string to_lower(std::string str) {
    std::transform(std::begin(str), std::end(str), std::begin(str), [](const std::string::value_type &x) {
        return std::tolower(x, std::locale());
    });
    return str;
}

/// Split a string '"one two" "three"' into 'one two', 'three'
inline std::vector<std::string> split_up(std::string str) {

    std::vector<char> delims = {'\'', '\"'};
    auto find_ws = [](char ch) { return std::isspace<char>(ch, std::locale()); };
    trim(str);

    std::vector<std::string> output;

    while(!str.empty()) {
        if(str[0] == '\'') {
            auto end = str.find('\'', 1);
            if(end != std::string::npos) {
                output.push_back(str.substr(1, end - 1));
                str = str.substr(end + 1);
            } else {
                output.push_back(str.substr(1));
                str = "";
            }
        } else if(str[0] == '\"') {
            auto end = str.find('\"', 1);
            if(end != std::string::npos) {
                output.push_back(str.substr(1, end - 1));
                str = str.substr(end + 1);
            } else {
                output.push_back(str.substr(1));
                str = "";
            }

        } else {
            auto it = std::find_if(std::begin(str), std::end(str), find_ws);
            if(it != std::end(str)) {
                std::string value = std::string(str.begin(), it);
                output.push_back(value);
                str = std::string(it, str.end());
            } else {
                output.push_back(str);
                str = "";
            }
        }
        trim(str);
    }

    return output;
}

} // namespace detail
} // namespace CLI

// From CLI/Error.hpp

namespace CLI {

// Use one of these on all error classes
#define CLI11_ERROR_DEF(parent, name)                                                                                  \
  protected:                                                                                                           \
    name(std::string name, std::string msg, int exit_code) : parent(std::move(name), std::move(msg), exit_code) {}     \
    name(std::string name, std::string msg, ExitCodes exit_code)                                                       \
        : parent(std::move(name), std::move(msg), exit_code) {}                                                        \
                                                                                                                       \
  public:                                                                                                              \
    name(std::string msg, ExitCodes exit_code) : parent(#name, std::move(msg), exit_code) {}                           \
    name(std::string msg, int exit_code) : parent(#name, std::move(msg), exit_code) {}

// This is added after the one above if a class is used directly and builds its own message
#define CLI11_ERROR_SIMPLE(name)                                                                                       \
    name(std::string msg) : name(#name, msg, ExitCodes::name) {}

/// These codes are part of every error in CLI. They can be obtained from e using e.exit_code or as a quick shortcut,
/// int values from e.get_error_code().
enum class ExitCodes {
    Success = 0,
    IncorrectConstruction = 100,
    BadNameString,
    OptionAlreadyAdded,
    FileError,
    ConversionError,
    ValidationError,
    RequiredError,
    RequiresError,
    ExcludesError,
    ExtrasError,
    INIError,
    InvalidError,
    HorribleError,
    OptionNotFound,
    ArgumentMismatch,
    BaseClass = 127
};

// Error definitions

/// @defgroup error_group Errors
/// @brief Errors thrown by CLI11
///
/// These are the errors that can be thrown. Some of them, like CLI::Success, are not really errors.
/// @{

/// All errors derive from this one
class Error : public std::runtime_error {
    int exit_code;
    std::string name{"Error"};

  public:
    int get_exit_code() const { return exit_code; }

    std::string get_name() const { return name; }

    Error(std::string name, std::string msg, int exit_code = static_cast<int>(ExitCodes::BaseClass))
        : runtime_error(msg), exit_code(exit_code), name(std::move(name)) {}

    Error(std::string name, std::string msg, ExitCodes exit_code) : Error(name, msg, static_cast<int>(exit_code)) {}
};

// Note: Using Error::Error constructors does not work on GCC 4.7

/// Construction errors (not in parsing)
class ConstructionError : public Error {
    CLI11_ERROR_DEF(Error, ConstructionError)
};

/// Thrown when an option is set to conflicting values (non-vector and multi args, for example)
class IncorrectConstruction : public ConstructionError {
    CLI11_ERROR_DEF(ConstructionError, IncorrectConstruction)
    CLI11_ERROR_SIMPLE(IncorrectConstruction)
    static IncorrectConstruction PositionalFlag(std::string name) {
        return IncorrectConstruction(name + ": Flags cannot be positional");
    }
    static IncorrectConstruction Set0Opt(std::string name) {
        return IncorrectConstruction(name + ": Cannot set 0 expected, use a flag instead");
    }
    static IncorrectConstruction ChangeNotVector(std::string name) {
        return IncorrectConstruction(name + ": You can only change the expected arguments for vectors");
    }
    static IncorrectConstruction AfterMultiOpt(std::string name) {
        return IncorrectConstruction(
            name + ": You can't change expected arguments after you've changed the multi option policy!");
    }
    static IncorrectConstruction MissingOption(std::string name) {
        return IncorrectConstruction("Option " + name + " is not defined");
    }
    static IncorrectConstruction MultiOptionPolicy(std::string name) {
        return IncorrectConstruction(name + ": multi_option_policy only works for flags and single value options");
    }
};

/// Thrown on construction of a bad name
class BadNameString : public ConstructionError {
    CLI11_ERROR_DEF(ConstructionError, BadNameString)
    CLI11_ERROR_SIMPLE(BadNameString)
    static BadNameString OneCharName(std::string name) { return BadNameString("Invalid one char name: " + name); }
    static BadNameString BadLongName(std::string name) { return BadNameString("Bad long name: " + name); }
    static BadNameString DashesOnly(std::string name) {
        return BadNameString("Must have a name, not just dashes: " + name);
    }
    static BadNameString MultiPositionalNames(std::string name) {
        return BadNameString("Only one positional name allowed, remove: " + name);
    }
};

/// Thrown when an option already exists
class OptionAlreadyAdded : public ConstructionError {
    CLI11_ERROR_DEF(ConstructionError, OptionAlreadyAdded)
    OptionAlreadyAdded(std::string name)
        : OptionAlreadyAdded(name + " is already added", ExitCodes::OptionAlreadyAdded) {}
    static OptionAlreadyAdded Requires(std::string name, std::string other) {
        return OptionAlreadyAdded(name + " requires " + other, ExitCodes::OptionAlreadyAdded);
    }
    static OptionAlreadyAdded Excludes(std::string name, std::string other) {
        return OptionAlreadyAdded(name + " excludes " + other, ExitCodes::OptionAlreadyAdded);
    }
};

// Parsing errors

/// Anything that can error in Parse
class ParseError : public Error {
    CLI11_ERROR_DEF(Error, ParseError)
};

// Not really "errors"

/// This is a successful completion on parsing, supposed to exit
class Success : public ParseError {
    CLI11_ERROR_DEF(ParseError, Success)
    Success() : Success("Successfully completed, should be caught and quit", ExitCodes::Success) {}
};

/// -h or --help on command line
class CallForHelp : public ParseError {
    CLI11_ERROR_DEF(ParseError, CallForHelp)
    CallForHelp() : CallForHelp("This should be caught in your main function, see examples", ExitCodes::Success) {}
};

/// Does not output a diagnostic in CLI11_PARSE, but allows to return from main() with a specific error code.
class RuntimeError : public ParseError {
    CLI11_ERROR_DEF(ParseError, RuntimeError)
    RuntimeError(int exit_code = 1) : RuntimeError("Runtime error", exit_code) {}
};

/// Thrown when parsing an INI file and it is missing
class FileError : public ParseError {
    CLI11_ERROR_DEF(ParseError, FileError)
    CLI11_ERROR_SIMPLE(FileError)
    static FileError Missing(std::string name) { return FileError(name + " was not readable (missing?)"); }
};

/// Thrown when conversion call back fails, such as when an int fails to coerce to a string
class ConversionError : public ParseError {
    CLI11_ERROR_DEF(ParseError, ConversionError)
    CLI11_ERROR_SIMPLE(ConversionError)
    ConversionError(std::string member, std::string name)
        : ConversionError("The value " + member + "is not an allowed value for " + name) {}
    ConversionError(std::string name, std::vector<std::string> results)
        : ConversionError("Could not convert: " + name + " = " + detail::join(results)) {}
    static ConversionError TooManyInputsFlag(std::string name) {
        return ConversionError(name + ": too many inputs for a flag");
    }
    static ConversionError TrueFalse(std::string name) {
        return ConversionError(name + ": Should be true/false or a number");
    }
};

/// Thrown when validation of results fails
class ValidationError : public ParseError {
    CLI11_ERROR_DEF(ParseError, ValidationError)
    CLI11_ERROR_SIMPLE(ValidationError)
    ValidationError(std::string name, std::string msg) : ValidationError(name + ": " + msg) {}
};

/// Thrown when a required option is missing
class RequiredError : public ParseError {
    CLI11_ERROR_DEF(ParseError, RequiredError)
    RequiredError(std::string name) : RequiredError(name + " is required", ExitCodes::RequiredError) {}
    static RequiredError Subcommand(size_t min_subcom) {
        if(min_subcom == 1)
            return RequiredError("A subcommand");
        else
            return RequiredError("Requires at least " + std::to_string(min_subcom) + " subcommands",
                                 ExitCodes::RequiredError);
    }
};

/// Thrown when the wrong number of arguments has been received
class ArgumentMismatch : public ParseError {
    CLI11_ERROR_DEF(ParseError, ArgumentMismatch)
    CLI11_ERROR_SIMPLE(ArgumentMismatch)
    ArgumentMismatch(std::string name, int expected, size_t recieved)
        : ArgumentMismatch(expected > 0 ? ("Expected exactly " + std::to_string(expected) + " arguments to " + name +
                                           ", got " + std::to_string(recieved))
                                        : ("Expected at least " + std::to_string(-expected) + " arguments to " + name +
                                           ", got " + std::to_string(recieved)),
                           ExitCodes::ArgumentMismatch) {}

    static ArgumentMismatch AtLeast(std::string name, int num) {
        return ArgumentMismatch(name + ": At least " + std::to_string(num) + " required");
    }
    static ArgumentMismatch TypedAtLeast(std::string name, int num, std::string type) {
        return ArgumentMismatch(name + ": " + std::to_string(num) + " required " + type + " missing");
    }
};

/// Thrown when a requires option is missing
class RequiresError : public ParseError {
    CLI11_ERROR_DEF(ParseError, RequiresError)
    RequiresError(std::string curname, std::string subname)
        : RequiresError(curname + " requires " + subname, ExitCodes::RequiresError) {}
};

/// Thrown when an excludes option is present
class ExcludesError : public ParseError {
    CLI11_ERROR_DEF(ParseError, ExcludesError)
    ExcludesError(std::string curname, std::string subname)
        : ExcludesError(curname + " excludes " + subname, ExitCodes::ExcludesError) {}
};

/// Thrown when too many positionals or options are found
class ExtrasError : public ParseError {
    CLI11_ERROR_DEF(ParseError, ExtrasError)
    ExtrasError(std::vector<std::string> args)
        : ExtrasError((args.size() > 1 ? "The following arguments were not expected: "
                                       : "The following argument was not expected: ") +
                          detail::rjoin(args, " "),
                      ExitCodes::ExtrasError) {}
};

/// Thrown when extra values are found in an INI file
class INIError : public ParseError {
    CLI11_ERROR_DEF(ParseError, INIError)
    CLI11_ERROR_SIMPLE(INIError)
    static INIError Extras(std::string item) { return INIError("INI was not able to parse " + item); }
    static INIError NotConfigurable(std::string item) {
        return INIError(item + ": This option is not allowed in a configuration file");
    }
};

/// Thrown when validation fails before parsing
class InvalidError : public ParseError {
    CLI11_ERROR_DEF(ParseError, InvalidError)
    InvalidError(std::string name)
        : InvalidError(name + ": Too many positional arguments with unlimited expected args", ExitCodes::InvalidError) {
    }
};

/// This is just a safety check to verify selection and parsing match - you should not ever see it
/// Strings are directly added to this error, but again, it should never be seen.
class HorribleError : public ParseError {
    CLI11_ERROR_DEF(ParseError, HorribleError)
    CLI11_ERROR_SIMPLE(HorribleError)
};

// After parsing

/// Thrown when counting a non-existent option
class OptionNotFound : public Error {
    CLI11_ERROR_DEF(Error, OptionNotFound)
    OptionNotFound(std::string name) : OptionNotFound(name + " not found", ExitCodes::OptionNotFound) {}
};

/// @}

} // namespace CLI

// From CLI/TypeTools.hpp

namespace CLI {

// Type tools

// We could check to see if C++14 is being used, but it does not hurt to redefine this
// (even Google does this: https://github.com/google/skia/blob/master/include/private/SkTLogic.h)
// It is not in the std namespace anyway, so no harm done.

template <bool B, class T = void> using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T> struct is_vector { static const bool value = false; };

template <class T, class A> struct is_vector<std::vector<T, A>> { static bool const value = true; };

template <typename T> struct is_bool { static const bool value = false; };

template <> struct is_bool<bool> { static bool const value = true; };

namespace detail {
// Based generally on https://rmf.io/cxx11/almost-static-if
/// Simple empty scoped class
enum class enabler {};

/// An instance to use in EnableIf
constexpr enabler dummy = {};

// Type name print

/// Was going to be based on
///  http://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template
/// But this is cleaner and works better in this case

template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "INT";
}

template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "UINT";
}

template <typename T, enable_if_t<std::is_floating_point<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "FLOAT";
}

/// This one should not be used, since vector types print the internal type
template <typename T, enable_if_t<is_vector<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "VECTOR";
}

template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value && !is_vector<T>::value,
                      detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "TEXT";
}

// Lexical cast

/// Integers / enums
template <typename T,
          enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value, detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    try {
        output = static_cast<T>(std::stoll(input));
        return true;
    } catch(const std::invalid_argument &) {
        return false;
    } catch(const std::out_of_range &) {
        return false;
    }
}

/// Floats
template <typename T, enable_if_t<std::is_floating_point<T>::value, detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    try {
        output = static_cast<T>(std::stold(input));
        return true;
    } catch(const std::invalid_argument &) {
        return false;
    } catch(const std::out_of_range &) {
        return false;
    }
}

/// String and similar
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value && !std::is_enum<T>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    output = input;
    return true;
}

} // namespace detail
} // namespace CLI

// From CLI/Split.hpp

namespace CLI {
namespace detail {

// Returns false if not a short option. Otherwise, sets opt name and rest and returns true
inline bool split_short(const std::string &current, std::string &name, std::string &rest) {
    if(current.size() > 1 && current[0] == '-' && valid_first_char(current[1])) {
        name = current.substr(1, 1);
        rest = current.substr(2);
        return true;
    } else
        return false;
}

// Returns false if not a long option. Otherwise, sets opt name and other side of = and returns true
inline bool split_long(const std::string &current, std::string &name, std::string &value) {
    if(current.size() > 2 && current.substr(0, 2) == "--" && valid_first_char(current[2])) {
        auto loc = current.find("=");
        if(loc != std::string::npos) {
            name = current.substr(2, loc - 2);
            value = current.substr(loc + 1);
        } else {
            name = current.substr(2);
            value = "";
        }
        return true;
    } else
        return false;
}

// Splits a string into multiple long and short names
inline std::vector<std::string> split_names(std::string current) {
    std::vector<std::string> output;
    size_t val;
    while((val = current.find(",")) != std::string::npos) {
        output.push_back(trim_copy(current.substr(0, val)));
        current = current.substr(val + 1);
    }
    output.push_back(trim_copy(current));
    return output;
}

/// Get a vector of short names, one of long names, and a single name
inline std::tuple<std::vector<std::string>, std::vector<std::string>, std::string>
get_names(const std::vector<std::string> &input) {

    std::vector<std::string> short_names;
    std::vector<std::string> long_names;
    std::string pos_name;

    for(std::string name : input) {
        if(name.length() == 0)
            continue;
        else if(name.length() > 1 && name[0] == '-' && name[1] != '-') {
            if(name.length() == 2 && valid_first_char(name[1]))
                short_names.emplace_back(1, name[1]);
            else
                throw BadNameString::OneCharName(name);
        } else if(name.length() > 2 && name.substr(0, 2) == "--") {
            name = name.substr(2);
            if(valid_name_string(name))
                long_names.push_back(name);
            else
                throw BadNameString::BadLongName(name);
        } else if(name == "-" || name == "--") {
            throw BadNameString::DashesOnly(name);
        } else {
            if(pos_name.length() > 0)
                throw BadNameString::MultiPositionalNames(name);
            pos_name = name;
        }
    }

    return std::tuple<std::vector<std::string>, std::vector<std::string>, std::string>(
        short_names, long_names, pos_name);
}

} // namespace detail
} // namespace CLI

// From CLI/Ini.hpp

namespace CLI {
namespace detail {

inline std::string inijoin(std::vector<std::string> args) {
    std::ostringstream s;
    size_t start = 0;
    for(const auto &arg : args) {
        if(start++ > 0)
            s << " ";

        auto it = std::find_if(arg.begin(), arg.end(), [](char ch) { return std::isspace<char>(ch, std::locale()); });
        if(it == arg.end())
            s << arg;
        else if(arg.find(R"(")") == std::string::npos)
            s << R"(")" << arg << R"(")";
        else
            s << R"(')" << arg << R"(')";
    }

    return s.str();
}

struct ini_ret_t {
    /// This is the full name with dots
    std::string fullname;

    /// Listing of inputs
    std::vector<std::string> inputs;

    /// Current parent level
    size_t level = 0;

    /// Return parent or empty string, based on level
    ///
    /// Level 0, a.b.c would return a
    /// Level 1, a.b.c could return b
    std::string parent() const {
        std::vector<std::string> plist = detail::split(fullname, '.');
        if(plist.size() > (level + 1))
            return plist[level];
        else
            return "";
    }

    /// Return name
    std::string name() const {
        std::vector<std::string> plist = detail::split(fullname, '.');
        return plist.at(plist.size() - 1);
    }
};

/// Internal parsing function
inline std::vector<ini_ret_t> parse_ini(std::istream &input) {
    std::string name, line;
    std::string section = "default";

    std::vector<ini_ret_t> output;

    while(getline(input, line)) {
        std::vector<std::string> items;

        detail::trim(line);
        size_t len = line.length();
        if(len > 1 && line[0] == '[' && line[len - 1] == ']') {
            section = line.substr(1, len - 2);
        } else if(len > 0 && line[0] != ';') {
            output.emplace_back();
            ini_ret_t &out = output.back();

            // Find = in string, split and recombine
            auto pos = line.find("=");
            if(pos != std::string::npos) {
                name = detail::trim_copy(line.substr(0, pos));
                std::string item = detail::trim_copy(line.substr(pos + 1));
                items = detail::split_up(item);
            } else {
                name = detail::trim_copy(line);
                items = {"ON"};
            }

            if(detail::to_lower(section) == "default")
                out.fullname = name;
            else
                out.fullname = section + "." + name;

            out.inputs.insert(std::end(out.inputs), std::begin(items), std::end(items));
        }
    }
    return output;
}

/// Parse an INI file, throw an error (ParseError:INIParseError or FileError) on failure
inline std::vector<ini_ret_t> parse_ini(const std::string &name) {

    std::ifstream input{name};
    if(!input.good())
        throw FileError::Missing(name);

    return parse_ini(input);
}

} // namespace detail
} // namespace CLI

// From CLI/Validators.hpp

namespace CLI {

/// @defgroup validator_group Validators
/// @brief Some validators that are provided
///
/// These are simple `void(std::string&)` validators that are useful. They throw
/// a ValidationError if they fail (or the normally expected error if the cast fails)
/// @{

/// Check for an existing file
inline std::string ExistingFile(const std::string &filename) {
    struct stat buffer;
    bool exist = stat(filename.c_str(), &buffer) == 0;
    bool is_dir = (buffer.st_mode & S_IFDIR) != 0;
    if(!exist) {
        return "File does not exist: " + filename;
    } else if(is_dir) {
        return "File is actually a directory: " + filename;
    }
    return std::string();
}

/// Check for an existing directory
inline std::string ExistingDirectory(const std::string &filename) {
    struct stat buffer;
    bool exist = stat(filename.c_str(), &buffer) == 0;
    bool is_dir = (buffer.st_mode & S_IFDIR) != 0;
    if(!exist) {
        return "Directory does not exist: " + filename;
    } else if(!is_dir) {
        return "Directory is actually a file: " + filename;
    }
    return std::string();
}

/// Check for a non-existing path
inline std::string NonexistentPath(const std::string &filename) {
    struct stat buffer;
    bool exist = stat(filename.c_str(), &buffer) == 0;
    if(exist) {
        return "Path already exists: " + filename;
    }
    return std::string();
}

/// Produce a range validator function
template <typename T> std::function<std::string(const std::string &)> Range(T min, T max) {
    return [min, max](std::string input) {
        T val;
        detail::lexical_cast(input, val);
        if(val < min || val > max)
            return "Value " + input + " not in range " + std::to_string(min) + " to " + std::to_string(max);

        return std::string();
    };
}

/// Range of one value is 0 to value
template <typename T> std::function<std::string(const std::string &)> Range(T max) {
    return Range(static_cast<T>(0), max);
}

/// @}

} // namespace CLI

// From CLI/Option.hpp

namespace CLI {

using results_t = std::vector<std::string>;
using callback_t = std::function<bool(results_t)>;

class Option;
class App;

using Option_p = std::unique_ptr<Option>;

enum class MultiOptionPolicy { Throw, TakeLast, TakeFirst, Join };

template <typename CRTP> class OptionBase {
    friend App;

  protected:
    /// The group membership
    std::string group_{"Options"};

    /// True if this is a required option
    bool required_{false};

    /// Ignore the case when matching (option, not value)
    bool ignore_case_{false};

    /// Allow this option to be given in a configuration file
    bool configurable_{true};

    /// Policy for multiple arguments when `expected_ == 1`  (can be set on bool flags, too)
    MultiOptionPolicy multi_option_policy_{MultiOptionPolicy::Throw};

    template <typename T> void copy_to(T *other) const {
        other->group(group_);
        other->required(required_);
        other->ignore_case(ignore_case_);
        other->configurable(configurable_);
        other->multi_option_policy(multi_option_policy_);
    }

  public:
    // setters

    /// Changes the group membership
    CRTP *group(std::string name) {
        group_ = name;
        return static_cast<CRTP *>(this);
        ;
    }

    /// Set the option as required
    CRTP *required(bool value = true) {
        required_ = value;
        return static_cast<CRTP *>(this);
    }

    /// Support Plumbum term
    CRTP *mandatory(bool value = true) { return required(value); }

    // Getters

    /// Get the group of this option
    const std::string &get_group() const { return group_; }

    /// True if this is a required option
    bool get_required() const { return required_; }

    /// The status of ignore case
    bool get_ignore_case() const { return ignore_case_; }

    /// The status of configurable
    bool get_configurable() const { return configurable_; }

    /// The status of the multi option policy
    MultiOptionPolicy get_multi_option_policy() const { return multi_option_policy_; }

    // Shortcuts for multi option policy

    /// Set the multi option policy to take last
    CRTP *take_last() {
        CRTP *self = static_cast<CRTP *>(this);
        self->multi_option_policy(MultiOptionPolicy::TakeLast);
        return self;
    }

    /// Set the multi option policy to take last
    CRTP *take_first() {
        CRTP *self = static_cast<CRTP *>(this);
        self->multi_option_policy(MultiOptionPolicy::TakeFirst);
        return self;
    }

    /// Set the multi option policy to take last
    CRTP *join() {
        CRTP *self = static_cast<CRTP *>(this);
        self->multi_option_policy(MultiOptionPolicy::Join);
        return self;
    }

    /// Allow in a configuration file
    CRTP *configurable(bool value = true) {
        configurable_ = value;
        return static_cast<CRTP *>(this);
    }
};

class OptionDefaults : public OptionBase<OptionDefaults> {
  public:
    OptionDefaults() = default;

    // Methods here need a different implementation if they are Option vs. OptionDefault

    /// Take the last argument if given multiple times
    OptionDefaults *multi_option_policy(MultiOptionPolicy value = MultiOptionPolicy::Throw) {
        multi_option_policy_ = value;
        return this;
    }

    /// Ignore the case of the option name
    OptionDefaults *ignore_case(bool value = true) {
        ignore_case_ = value;
        return this;
    }
};

class Option : public OptionBase<Option> {
    friend App;

  protected:
    /// @name Names
    ///@{

    /// A list of the short names (`-a`) without the leading dashes
    std::vector<std::string> snames_;

    /// A list of the long names (`--a`) without the leading dashes
    std::vector<std::string> lnames_;

    /// A positional name
    std::string pname_;

    /// If given, check the environment for this option
    std::string envname_;

    ///@}
    /// @name Help
    ///@{

    /// The description for help strings
    std::string description_;

    /// A human readable default value, usually only set if default is true in creation
    std::string defaultval_;

    /// A human readable type value, set when App creates this
    std::string typeval_;

    /// True if this option has a default
    bool default_{false};

    ///@}
    /// @name Configuration
    ///@{

    /// The number of expected values, 0 for flag, -1 for unlimited vector
    int expected_{1};

    /// A private setting to allow args to not be able to accept incorrect expected values
    bool changeable_{false};

    /// A list of validators to run on each value parsed
    std::vector<std::function<std::string(std::string &)>> validators_;

    /// A list of options that are required with this option
    std::set<Option *> requires_;

    /// A list of options that are excluded with this option
    std::set<Option *> excludes_;

    ///@}
    /// @name Other
    ///@{

    /// Remember the parent app
    App *parent_;

    /// Options store a callback to do all the work
    callback_t callback_;

    ///@}
    /// @name Parsing results
    ///@{

    /// Results of parsing
    results_t results_;

    /// Whether the callback has run (needed for INI parsing)
    bool callback_run_{false};

    ///@}

    /// Making an option by hand is not defined, it must be made by the App class
    Option(std::string name,
           std::string description = "",
           std::function<bool(results_t)> callback = [](results_t) { return true; },
           bool default_ = true,
           App *parent = nullptr)
        : description_(std::move(description)), default_(default_), parent_(parent), callback_(std::move(callback)) {
        std::tie(snames_, lnames_, pname_) = detail::get_names(detail::split_names(name));
    }

  public:
    /// @name Basic
    ///@{

    /// Count the total number of times an option was passed
    size_t count() const { return results_.size(); }

    /// This class is true if option is passed.
    operator bool() const { return count() > 0; }

    /// Clear the parsed results (mostly for testing)
    void clear() { results_.clear(); }

    ///@}
    /// @name Setting options
    ///@{

    /// Set the number of expected arguments (Flags bypass this)
    Option *expected(int value) {
        if(expected_ == value)
            return this;
        else if(value == 0)
            throw IncorrectConstruction::Set0Opt(single_name());
        else if(!changeable_)
            throw IncorrectConstruction::ChangeNotVector(single_name());
        else if(value != 1 && multi_option_policy_ != MultiOptionPolicy::Throw)
            throw IncorrectConstruction::AfterMultiOpt(single_name());

        expected_ = value;
        return this;
    }

    /// Adds a validator
    Option *check(std::function<std::string(const std::string &)> validator) {
        validators_.emplace_back(validator);
        return this;
    }

    /// Adds a validator-like function that can change result
    Option *transform(std::function<std::string(std::string)> func) {
        validators_.emplace_back([func](std::string &inout) {
            try {
                inout = func(inout);
            } catch(const ValidationError &e) {
                return std::string(e.what());
            }
            return std::string();
        });
        return this;
    }

    /// Sets required options
    Option *requires_option(Option *opt) {
        auto tup = requires_.insert(opt);
        if(!tup.second)
            throw OptionAlreadyAdded::Requires(get_name(), opt->get_name());
        return this;
    }

    /// Can find a string if needed
    template <typename T = App> Option *requires_option(std::string opt_name) {
        for(const Option_p &opt : dynamic_cast<T *>(parent_)->options_)
            if(opt.get() != this && opt->check_name(opt_name))
                return requires_option(opt.get());
        throw IncorrectConstruction::MissingOption(opt_name);
    }

    /// Any number supported, any mix of string and Opt
    template <typename A, typename B, typename... ARG> Option *requires_option(A opt, B opt1, ARG... args) {
        requires_option(opt);
        return requires_option(opt1, args...);
    }

    /// Sets excluded options
    Option *excludes(Option *opt) {
        auto tup = excludes_.insert(opt);
        if(!tup.second)
            throw OptionAlreadyAdded::Excludes(get_name(), opt->get_name());
        return this;
    }

    /// Can find a string if needed
    template <typename T = App> Option *excludes(std::string opt_name) {
        for(const Option_p &opt : dynamic_cast<T *>(parent_)->options_)
            if(opt.get() != this && opt->check_name(opt_name))
                return excludes(opt.get());
        throw IncorrectConstruction::MissingOption(opt_name);
    }
    /// Any number supported, any mix of string and Opt
    template <typename A, typename B, typename... ARG> Option *excludes(A opt, B opt1, ARG... args) {
        excludes(opt);
        return excludes(opt1, args...);
    }

    /// Sets environment variable to read if no option given
    Option *envname(std::string name) {
        envname_ = name;
        return this;
    }

    /// Ignore case
    ///
    /// The template hides the fact that we don't have the definition of App yet.
    /// You are never expected to add an argument to the template here.
    template <typename T = App> Option *ignore_case(bool value = true) {
        ignore_case_ = value;
        auto *parent = dynamic_cast<T *>(parent_);

        for(const Option_p &opt : parent->options_)
            if(opt.get() != this && *opt == *this)
                throw OptionAlreadyAdded(opt->get_name());

        return this;
    }

    /// Take the last argument if given multiple times
    Option *multi_option_policy(MultiOptionPolicy value = MultiOptionPolicy::Throw) {
        if(get_expected() != 0 && get_expected() != 1)
            throw IncorrectConstruction::MultiOptionPolicy(single_name());
        multi_option_policy_ = value;
        return this;
    }

    ///@}
    /// @name Accessors
    ///@{

    /// The number of arguments the option expects
    int get_expected() const { return expected_; }

    /// True if this has a default value
    int get_default() const { return default_; }

    /// True if the argument can be given directly
    bool get_positional() const { return pname_.length() > 0; }

    /// True if option has at least one non-positional name
    bool nonpositional() const { return (snames_.size() + lnames_.size()) > 0; }

    /// True if option has description
    bool has_description() const { return description_.length() > 0; }

    /// Get the description
    const std::string &get_description() const { return description_; }

    // Just the pname
    std::string get_pname() const { return pname_; }

    ///@}
    /// @name Help tools
    ///@{

    /// Gets a , sep list of names. Does not include the positional name if opt_only=true.
    std::string get_name(bool opt_only = false) const {
        std::vector<std::string> name_list;
        if(!opt_only && pname_.length() > 0)
            name_list.push_back(pname_);
        for(const std::string &sname : snames_)
            name_list.push_back("-" + sname);
        for(const std::string &lname : lnames_)
            name_list.push_back("--" + lname);
        return detail::join(name_list, ", ");
    }

    /// The name and any extras needed for positionals
    std::string help_positional() const {
        std::string out = pname_;
        if(get_expected() > 1)
            out = out + "(" + std::to_string(get_expected()) + "x)";
        else if(get_expected() == -1)
            out = out + "...";
        out = get_required() ? out : "[" + out + "]";
        return out;
    }

    /// The most discriptive name available
    std::string single_name() const {
        if(!lnames_.empty())
            return std::string("--") + lnames_[0];
        else if(!snames_.empty())
            return std::string("-") + snames_[0];
        else
            return pname_;
    }

    /// The first half of the help print, name plus default, etc. Setting opt_only to true avoids the positional name.
    std::string help_name(bool opt_only = false) const {
        std::stringstream out;
        out << get_name(opt_only) << help_aftername();
        return out.str();
    }

    /// pname with type info
    std::string help_pname() const {
        std::stringstream out;
        out << get_pname() << help_aftername();
        return out.str();
    }

    /// This is the part after the name is printed but before the description
    std::string help_aftername() const {
        std::stringstream out;

        if(get_expected() != 0) {
            if(!typeval_.empty())
                out << " " << typeval_;
            if(!defaultval_.empty())
                out << "=" << defaultval_;
            if(get_expected() > 1)
                out << " x " << get_expected();
            if(get_expected() == -1)
                out << " ...";
        }
        if(!envname_.empty())
            out << " (env:" << envname_ << ")";
        if(!requires_.empty()) {
            out << " Requires:";
            for(const Option *opt : requires_)
                out << " " << opt->get_name();
        }
        if(!excludes_.empty()) {
            out << " Excludes:";
            for(const Option *opt : excludes_)
                out << " " << opt->get_name();
        }
        return out.str();
    }

    ///@}
    /// @name Parser tools
    ///@{

    /// Process the callback
    void run_callback() {

        // Run the validators (can change the string)
        if(!validators_.empty()) {
            for(std::string &result : results_)
                for(const std::function<std::string(std::string &)> &vali : validators_) {
                    std::string err_msg = vali(result);
                    if(!err_msg.empty())
                        throw ValidationError(single_name(), err_msg);
                }
        }

        bool local_result;

        // Operation depends on the policy setting
        if(multi_option_policy_ == MultiOptionPolicy::TakeLast) {
            results_t partial_result = {results_.back()};
            local_result = !callback_(partial_result);
        } else if(multi_option_policy_ == MultiOptionPolicy::TakeFirst) {
            results_t partial_result = {results_.at(0)};
            local_result = !callback_(partial_result);
        } else if(multi_option_policy_ == MultiOptionPolicy::Join) {
            results_t partial_result = {detail::join(results_, "\n")};
            local_result = !callback_(partial_result);
        } else {
            if((expected_ > 0 && results_.size() != static_cast<size_t>(expected_)) ||
               (expected_ < 0 && results_.size() < static_cast<size_t>(-expected_)))
                throw ArgumentMismatch(single_name(), expected_, results_.size());
            else
                local_result = !callback_(results_);
        }

        if(local_result)
            throw ConversionError(get_name(), results_);
    }

    /// If options share any of the same names, they are equal (not counting positional)
    bool operator==(const Option &other) const {
        for(const std::string &sname : snames_)
            if(other.check_sname(sname))
                return true;
        for(const std::string &lname : lnames_)
            if(other.check_lname(lname))
                return true;
        // We need to do the inverse, just in case we are ignore_case
        for(const std::string &sname : other.snames_)
            if(check_sname(sname))
                return true;
        for(const std::string &lname : other.lnames_)
            if(check_lname(lname))
                return true;
        return false;
    }

    /// Check a name. Requires "-" or "--" for short / long, supports positional name
    bool check_name(std::string name) const {

        if(name.length() > 2 && name.substr(0, 2) == "--")
            return check_lname(name.substr(2));
        else if(name.length() > 1 && name.substr(0, 1) == "-")
            return check_sname(name.substr(1));
        else {
            std::string local_pname = pname_;
            if(ignore_case_) {
                local_pname = detail::to_lower(local_pname);
                name = detail::to_lower(name);
            }
            return name == local_pname;
        }
    }

    /// Requires "-" to be removed from string
    bool check_sname(std::string name) const {
        if(ignore_case_) {
            name = detail::to_lower(name);
            return std::find_if(std::begin(snames_), std::end(snames_), [&name](std::string local_sname) {
                       return detail::to_lower(local_sname) == name;
                   }) != std::end(snames_);
        } else
            return std::find(std::begin(snames_), std::end(snames_), name) != std::end(snames_);
    }

    /// Requires "--" to be removed from string
    bool check_lname(std::string name) const {
        if(ignore_case_) {
            name = detail::to_lower(name);
            return std::find_if(std::begin(lnames_), std::end(lnames_), [&name](std::string local_sname) {
                       return detail::to_lower(local_sname) == name;
                   }) != std::end(lnames_);
        } else
            return std::find(std::begin(lnames_), std::end(lnames_), name) != std::end(lnames_);
    }

    /// Puts a result at the end, unless last_ is set, in which case it just keeps the last one
    void add_result(std::string s) {
        results_.push_back(s);
        callback_run_ = false;
    }

    /// Get a copy of the results
    std::vector<std::string> results() const { return results_; }

    /// See if the callback has been run already
    bool get_callback_run() const { return callback_run_; }

    ///@}
    /// @name Custom options
    ///@{

    /// Set a custom option, typestring, expected; locks changeable unless expected is -1
    void set_custom_option(std::string typeval, int expected = 1) {
        typeval_ = typeval;
        expected_ = expected;
        if(expected == 0)
            required_ = false;
        changeable_ = expected < 0;
    }

    /// Set the default value string representation
    void set_default_str(std::string val) { defaultval_ = val; }

    /// Set the default value string representation and evaluate
    void set_default_val(std::string val) {
        set_default_str(val);
        auto old_results = results_;
        results_ = {val};
        run_callback();
        results_ = std::move(old_results);
    }

    /// Set the type name displayed on this option
    void set_type_name(std::string val) { typeval_ = val; }

    /// Get the typename for this option
    std::string get_type_name() const { return typeval_; }

    ///@}

  protected:
    /// @name App Helpers
    ///@{
    /// Can print positional name detailed option if true
    bool _has_help_positional() const {
        return get_positional() && (has_description() || !requires_.empty() || !excludes_.empty());
    }
    ///@}
};

} // namespace CLI

// From CLI/App.hpp

namespace CLI {

#ifndef CLI11_PARSE
#define CLI11_PARSE(app, argc, argv)                                                                                   \
    try {                                                                                                              \
        (app).parse((argc), (argv));                                                                                   \
    } catch(const CLI::ParseError &e) {                                                                                \
        return (app).exit(e);                                                                                          \
    }
#endif

namespace detail {
enum class Classifer { NONE, POSITIONAL_MARK, SHORT, LONG, SUBCOMMAND };
struct AppFriend;
} // namespace detail

namespace FailureMessage {
std::string simple(const App *app, const Error &e);
std::string help(const App *app, const Error &e);
} // namespace FailureMessage

class App;

using App_p = std::unique_ptr<App>;

/// Creates a command line program, with very few defaults.
/** To use, create a new `Program()` instance with `argc`, `argv`, and a help description. The templated
 *  add_option methods make it easy to prepare options. Remember to call `.start` before starting your
 * program, so that the options can be evaluated and the help option doesn't accidentally run your program. */
class App {
    friend Option;
    friend detail::AppFriend;

  protected:
    // This library follows the Google style guide for member names ending in underscores

    /// @name Basics
    ///@{

    /// Subcommand name or program name (from parser)
    std::string name_{"program"};

    /// Description of the current program/subcommand
    std::string description_;

    /// If true, allow extra arguments (ie, don't throw an error). INHERITABLE
    bool allow_extras_{false};

    ///  If true, return immediately on an unrecognised option (implies allow_extras) INHERITABLE
    bool prefix_command_{false};

    /// This is a function that runs when complete. Great for subcommands. Can throw.
    std::function<void()> callback_;

    ///@}
    /// @name Options
    ///@{

    /// The default values for options, customizable and changeable INHERITABLE
    OptionDefaults option_defaults_;

    /// The list of options, stored locally
    std::vector<Option_p> options_;

    ///@}
    /// @name Help
    ///@{

    /// Footer to put after all options in the help output INHERITABLE
    std::string footer_;

    /// A pointer to the help flag if there is one INHERITABLE
    Option *help_ptr_{nullptr};

    /// The error message printing function INHERITABLE
    std::function<std::string(const App *, const Error &e)> failure_message_ = FailureMessage::simple;

    ///@}
    /// @name Parsing
    ///@{

    using missing_t = std::vector<std::pair<detail::Classifer, std::string>>;

    /// Pair of classifier, string for missing options. (extra detail is removed on returning from parse)
    ///
    /// This is faster and cleaner than storing just a list of strings and reparsing. This may contain the -- separator.
    missing_t missing_;

    /// This is a list of pointers to options with the original parse order
    std::vector<Option *> parse_order_;

    /// This is a list of the subcommands collected, in order
    std::vector<App *> parsed_subcommands_;

    ///@}
    /// @name Subcommands
    ///@{

    /// Storage for subcommand list
    std::vector<App_p> subcommands_;

    /// If true, the program name is not case sensitive INHERITABLE
    bool ignore_case_{false};

    /// Allow subcommand fallthrough, so that parent commands can collect commands after subcommand.  INHERITABLE
    bool fallthrough_{false};

    /// A pointer to the parent if this is a subcommand
    App *parent_{nullptr};

    /// True if this command/subcommand was parsed
    bool parsed_{false};

    /// Minimum required subcommands
    size_t require_subcommand_min_ = 0;

    /// Max number of subcommands allowed (parsing stops after this number). 0 is unlimited INHERITABLE
    size_t require_subcommand_max_ = 0;

    /// The group membership INHERITABLE
    std::string group_{"Subcommands"};

    ///@}
    /// @name Config
    ///@{

    /// The name of the connected config file
    std::string config_name_;

    /// True if ini is required (throws if not present), if false simply keep going.
    bool config_required_{false};

    /// Pointer to the config option
    Option *config_ptr_{nullptr};

    ///@}

    /// Special private constructor for subcommand
    App(std::string description_, App *parent) : description_(std::move(description_)), parent_(parent) {
        // Inherit if not from a nullptr
        if(parent_ != nullptr) {
            if(parent_->help_ptr_ != nullptr)
                set_help_flag(parent_->help_ptr_->get_name(), parent_->help_ptr_->get_description());

            /// OptionDefaults
            option_defaults_ = parent_->option_defaults_;

            // INHERITABLE
            failure_message_ = parent_->failure_message_;
            allow_extras_ = parent_->allow_extras_;
            prefix_command_ = parent_->prefix_command_;
            ignore_case_ = parent_->ignore_case_;
            fallthrough_ = parent_->fallthrough_;
            group_ = parent_->group_;
            footer_ = parent_->footer_;
            require_subcommand_max_ = parent_->require_subcommand_max_;
        }
    }

  public:
    /// @name Basic
    ///@{

    /// Create a new program. Pass in the same arguments as main(), along with a help string.
    App(std::string description_ = "") : App(description_, nullptr) {
        set_help_flag("-h,--help", "Print this help message and exit");
    }

    virtual ~App() = default;

    /// Set a callback for the end of parsing.
    ///
    /// Due to a bug in c++11,
    /// it is not possible to overload on std::function (fixed in c++14
    /// and backported to c++11 on newer compilers). Use capture by reference
    /// to get a pointer to App if needed.
    App *set_callback(std::function<void()> callback) {
        callback_ = callback;
        return this;
    }

    /// Remove the error when extras are left over on the command line.
    App *allow_extras(bool allow = true) {
        allow_extras_ = allow;
        return this;
    }

    /// Do not parse anything after the first unrecognised option and return
    App *prefix_command(bool allow = true) {
        prefix_command_ = allow;
        return this;
    }

    /// Ignore case. Subcommand inherit value.
    App *ignore_case(bool value = true) {
        ignore_case_ = value;
        if(parent_ != nullptr) {
            for(const auto &subc : parent_->subcommands_) {
                if(subc.get() != this && (this->check_name(subc->name_) || subc->check_name(this->name_)))
                    throw OptionAlreadyAdded(subc->name_);
            }
        }
        return this;
    }

    /// Check to see if this subcommand was parsed, true only if received on command line.
    bool parsed() const { return parsed_; }

    /// Get the OptionDefault object, to set option defaults
    OptionDefaults *option_defaults() { return &option_defaults_; }

    ///@}
    /// @name Adding options
    ///@{

    /// Add an option, will automatically understand the type for common types.
    ///
    /// To use, create a variable with the expected type, and pass it in after the name.
    /// After start is called, you can use count to see if the value was passed, and
    /// the value will be initialized properly. Numbers, vectors, and strings are supported.
    ///
    /// ->required(), ->default, and the validators are options,
    /// The positional options take an optional number of arguments.
    ///
    /// For example,
    ///
    ///     std::string filename;
    ///     program.add_option("filename", filename, "description of filename");
    ///
    Option *add_option(std::string name, callback_t callback, std::string description = "", bool defaulted = false) {
        Option myopt{name, description, callback, defaulted, this};

        if(std::find_if(std::begin(options_), std::end(options_), [&myopt](const Option_p &v) {
               return *v == myopt;
           }) == std::end(options_)) {
            options_.emplace_back();
            Option_p &option = options_.back();
            option.reset(new Option(name, description, callback, defaulted, this));
            option_defaults_.copy_to(option.get());
            return option.get();
        } else
            throw OptionAlreadyAdded(myopt.get_name());
    }

    /// Add option for non-vectors (duplicate copy needed without defaulted to avoid `iostream << value`)
    template <typename T, enable_if_t<!is_vector<T>::value, detail::enabler> = detail::dummy>
    Option *add_option(std::string name,
                       T &variable, ///< The variable to set
                       std::string description = "") {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&variable, simple_name](CLI::results_t res) {
            return detail::lexical_cast(res[0], variable);
        };

        Option *opt = add_option(name, fun, description, false);
        opt->set_custom_option(detail::type_name<T>());
        return opt;
    }

    /// Add option for non-vectors with a default print
    template <typename T, enable_if_t<!is_vector<T>::value, detail::enabler> = detail::dummy>
    Option *add_option(std::string name,
                       T &variable, ///< The variable to set
                       std::string description,
                       bool defaulted) {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&variable, simple_name](CLI::results_t res) {
            return detail::lexical_cast(res[0], variable);
        };

        Option *opt = add_option(name, fun, description, defaulted);
        opt->set_custom_option(detail::type_name<T>());
        if(defaulted) {
            std::stringstream out;
            out << variable;
            opt->set_default_str(out.str());
        }
        return opt;
    }

    /// Add option for vectors (no default)
    template <typename T>
    Option *add_option(std::string name,
                       std::vector<T> &variable, ///< The variable vector to set
                       std::string description = "") {

        CLI::callback_t fun = [&variable](CLI::results_t res) {
            bool retval = true;
            variable.clear();
            for(const auto &a : res) {
                variable.emplace_back();
                retval &= detail::lexical_cast(a, variable.back());
            }
            return (!variable.empty()) && retval;
        };

        Option *opt = add_option(name, fun, description, false);
        opt->set_custom_option(detail::type_name<T>(), -1);
        return opt;
    }

    /// Add option for vectors
    template <typename T>
    Option *add_option(std::string name,
                       std::vector<T> &variable, ///< The variable vector to set
                       std::string description,
                       bool defaulted) {

        CLI::callback_t fun = [&variable](CLI::results_t res) {
            bool retval = true;
            variable.clear();
            for(const auto &a : res) {
                variable.emplace_back();
                retval &= detail::lexical_cast(a, variable.back());
            }
            return (!variable.empty()) && retval;
        };

        Option *opt = add_option(name, fun, description, defaulted);
        opt->set_custom_option(detail::type_name<T>(), -1);
        if(defaulted)
            opt->set_default_str("[" + detail::join(variable) + "]");
        return opt;
    }

    /// Set a help flag, replaced the existing one if present
    Option *set_help_flag(std::string name = "", std::string description = "") {
        if(help_ptr_ != nullptr) {
            remove_option(help_ptr_);
            help_ptr_ = nullptr;
        }

        // Empty name will simply remove the help flag
        if(!name.empty()) {
            help_ptr_ = add_flag(name, description);
            help_ptr_->configurable(false);
        }

        return help_ptr_;
    }

    /// Add option for flag
    Option *add_flag(std::string name, std::string description = "") {
        CLI::callback_t fun = [](CLI::results_t) { return true; };

        Option *opt = add_option(name, fun, description, false);
        if(opt->get_positional())
            throw IncorrectConstruction::PositionalFlag(name);
        opt->set_custom_option("", 0);
        return opt;
    }

    /// Add option for flag integer
    template <typename T,
              enable_if_t<std::is_integral<T>::value && !is_bool<T>::value, detail::enabler> = detail::dummy>
    Option *add_flag(std::string name,
                     T &count, ///< A variable holding the count
                     std::string description = "") {

        count = 0;
        CLI::callback_t fun = [&count](CLI::results_t res) {
            count = static_cast<T>(res.size());
            return true;
        };

        Option *opt = add_option(name, fun, description, false);
        if(opt->get_positional())
            throw IncorrectConstruction::PositionalFlag(name);
        opt->set_custom_option("", 0);
        return opt;
    }

    /// Bool version - defaults to allowing multiple passings, but can be forced to one if
    /// `multi_option_policy(CLI::MultiOptionPolicy::Throw)` is used.
    template <typename T, enable_if_t<is_bool<T>::value, detail::enabler> = detail::dummy>
    Option *add_flag(std::string name,
                     T &count, ///< A variable holding true if passed
                     std::string description = "") {

        count = false;
        CLI::callback_t fun = [&count](CLI::results_t res) {
            count = true;
            return res.size() == 1;
        };

        Option *opt = add_option(name, fun, description, false);
        if(opt->get_positional())
            throw IncorrectConstruction::PositionalFlag(name);
        opt->set_custom_option("", 0);
        opt->multi_option_policy(CLI::MultiOptionPolicy::TakeLast);
        return opt;
    }

    /// Add option for callback
    Option *add_flag_function(std::string name,
                              std::function<void(size_t)> function, ///< A function to call, void(size_t)
                              std::string description = "") {

        CLI::callback_t fun = [function](CLI::results_t res) {
            auto count = static_cast<size_t>(res.size());
            function(count);
            return true;
        };

        Option *opt = add_option(name, fun, description, false);
        if(opt->get_positional())
            throw IncorrectConstruction::PositionalFlag(name);
        opt->set_custom_option("", 0);
        return opt;
    }

#if __cplusplus >= 201402L
    /// Add option for callback (C++14 or better only)
    Option *add_flag(std::string name,
                     std::function<void(size_t)> function, ///< A function to call, void(size_t)
                     std::string description = "") {
        return add_flag_function(name, function, description);
    }
#endif

    /// Add set of options (No default)
    template <typename T>
    Option *add_set(std::string name,
                    T &member,           ///< The selected member of the set
                    std::set<T> options, ///< The set of possibilities
                    std::string description = "") {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&member, options, simple_name](CLI::results_t res) {
            bool retval = detail::lexical_cast(res[0], member);
            if(!retval)
                throw ConversionError(res[0], simple_name);
            return std::find(std::begin(options), std::end(options), member) != std::end(options);
        };

        Option *opt = add_option(name, fun, description, false);
        std::string typeval = detail::type_name<T>();
        typeval += " in {" + detail::join(options) + "}";
        opt->set_custom_option(typeval);
        return opt;
    }

    /// Add set of options
    template <typename T>
    Option *add_set(std::string name,
                    T &member,           ///< The selected member of the set
                    std::set<T> options, ///< The set of posibilities
                    std::string description,
                    bool defaulted) {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&member, options, simple_name](CLI::results_t res) {
            bool retval = detail::lexical_cast(res[0], member);
            if(!retval)
                throw ConversionError(res[0], simple_name);
            return std::find(std::begin(options), std::end(options), member) != std::end(options);
        };

        Option *opt = add_option(name, fun, description, defaulted);
        std::string typeval = detail::type_name<T>();
        typeval += " in {" + detail::join(options) + "}";
        opt->set_custom_option(typeval);
        if(defaulted) {
            std::stringstream out;
            out << member;
            opt->set_default_str(out.str());
        }
        return opt;
    }

    /// Add set of options, string only, ignore case (no default)
    Option *add_set_ignore_case(std::string name,
                                std::string &member,           ///< The selected member of the set
                                std::set<std::string> options, ///< The set of possibilities
                                std::string description = "") {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&member, options, simple_name](CLI::results_t res) {
            member = detail::to_lower(res[0]);
            auto iter = std::find_if(std::begin(options), std::end(options), [&member](std::string val) {
                return detail::to_lower(val) == member;
            });
            if(iter == std::end(options))
                throw ConversionError(member, simple_name);
            else {
                member = *iter;
                return true;
            }
        };

        Option *opt = add_option(name, fun, description, false);
        std::string typeval = detail::type_name<std::string>();
        typeval += " in {" + detail::join(options) + "}";
        opt->set_custom_option(typeval);

        return opt;
    }

    /// Add set of options, string only, ignore case
    Option *add_set_ignore_case(std::string name,
                                std::string &member,           ///< The selected member of the set
                                std::set<std::string> options, ///< The set of posibilities
                                std::string description,
                                bool defaulted) {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&member, options, simple_name](CLI::results_t res) {
            member = detail::to_lower(res[0]);
            auto iter = std::find_if(std::begin(options), std::end(options), [&member](std::string val) {
                return detail::to_lower(val) == member;
            });
            if(iter == std::end(options))
                throw ConversionError(member, simple_name);
            else {
                member = *iter;
                return true;
            }
        };

        Option *opt = add_option(name, fun, description, defaulted);
        std::string typeval = detail::type_name<std::string>();
        typeval += " in {" + detail::join(options) + "}";
        opt->set_custom_option(typeval);
        if(defaulted) {
            opt->set_default_str(member);
        }
        return opt;
    }

    /// Add a complex number
    template <typename T>
    Option *add_complex(std::string name,
                        T &variable,
                        std::string description = "",
                        bool defaulted = false,
                        std::string label = "COMPLEX") {

        std::string simple_name = CLI::detail::split(name, ',').at(0);
        CLI::callback_t fun = [&variable, simple_name, label](results_t res) {
            double x, y;
            bool worked = detail::lexical_cast(res[0], x) && detail::lexical_cast(res[1], y);
            if(worked)
                variable = T(x, y);
            return worked;
        };

        CLI::Option *opt = add_option(name, fun, description, defaulted);
        opt->set_custom_option(label, 2);
        if(defaulted) {
            std::stringstream out;
            out << variable;
            opt->set_default_str(out.str());
        }
        return opt;
    }

    /// Set a configuration ini file option, or clear it if no name passed
    Option *set_config(std::string name = "",
                       std::string default_filename = "",
                       std::string help = "Read an ini file",
                       bool required = false) {

        // Remove existing config if present
        if(config_ptr_ != nullptr)
            remove_option(config_ptr_);

        // Only add config if option passed
        if(!name.empty()) {
            config_name_ = default_filename;
            config_required_ = required;
            config_ptr_ = add_option(name, config_name_, help, !default_filename.empty());
            config_ptr_->configurable(false);
        }

        return config_ptr_;
    }

    /// Removes an option from the App. Takes an option pointer. Returns true if found and removed.
    bool remove_option(Option *opt) {
        auto iterator =
            std::find_if(std::begin(options_), std::end(options_), [opt](const Option_p &v) { return v.get() == opt; });
        if(iterator != std::end(options_)) {
            options_.erase(iterator);
            return true;
        }
        return false;
    }

    ///@}
    /// @name Subcommmands
    ///@{

    /// Add a subcommand. Inherits INHERITABLE and OptionDefaults, and help flag
    App *add_subcommand(std::string name, std::string description = "") {
        subcommands_.emplace_back(new App(description, this));
        subcommands_.back()->name_ = name;
        for(const auto &subc : subcommands_)
            if(subc.get() != subcommands_.back().get())
                if(subc->check_name(subcommands_.back()->name_) || subcommands_.back()->check_name(subc->name_))
                    throw OptionAlreadyAdded(subc->name_);
        return subcommands_.back().get();
    }

    /// Check to see if a subcommand is part of this command (doesn't have to be in command line)
    App *get_subcommand(App *subcom) const {
        for(const App_p &subcomptr : subcommands_)
            if(subcomptr.get() == subcom)
                return subcom;
        throw OptionNotFound(subcom->get_name());
    }

    /// Check to see if a subcommand is part of this command (text version)
    App *get_subcommand(std::string subcom) const {
        for(const App_p &subcomptr : subcommands_)
            if(subcomptr->check_name(subcom))
                return subcomptr.get();
        throw OptionNotFound(subcom);
    }

    /// Changes the group membership
    App *group(std::string name) {
        group_ = name;
        return this;
    }

    /// The argumentless form of require subcommand requires 1 or more subcommands
    App *require_subcommand() {
        require_subcommand_min_ = 1;
        require_subcommand_max_ = 0;
        return this;
    }

    /// Require a subcommand to be given (does not affect help call)
    /// The number required can be given. Negative values indicate maximum
    /// number allowed (0 for any number). Max number inheritable.
    App *require_subcommand(int value) {
        if(value < 0) {
            require_subcommand_min_ = 0;
            require_subcommand_max_ = static_cast<size_t>(-value);
        } else {
            require_subcommand_min_ = static_cast<size_t>(value);
            require_subcommand_max_ = static_cast<size_t>(value);
        }
        return this;
    }

    /// Explicitly control the number of subcommands required. Setting 0
    /// for the max means unlimited number allowed. Max number inheritable.
    App *require_subcommand(size_t min, size_t max) {
        require_subcommand_min_ = min;
        require_subcommand_max_ = max;
        return this;
    }

    /// Stop subcommand fallthrough, so that parent commands cannot collect commands after subcommand.
    /// Default from parent, usually set on parent.
    App *fallthrough(bool value = true) {
        fallthrough_ = value;
        return this;
    }

    /// Check to see if this subcommand was parsed, true only if received on command line.
    /// This allows the subcommand to be directly checked.
    operator bool() const { return parsed_; }

    ///@}
    /// @name Extras for subclassing
    ///@{

    /// This allows subclasses to inject code before callbacks but after parse.
    ///
    /// This does not run if any errors or help is thrown.
    virtual void pre_callback() {}

    ///@}
    /// @name Parsing
    ///@{

    /// Parses the command line - throws errors
    /// This must be called after the options are in but before the rest of the program.
    void parse(int argc, char **argv) {
        name_ = argv[0];
        std::vector<std::string> args;
        for(int i = argc - 1; i > 0; i--)
            args.emplace_back(argv[i]);
        parse(args);
    }

    /// The real work is done here. Expects a reversed vector.
    /// Changes the vector to the remaining options.
    void parse(std::vector<std::string> &args) {
        _validate();
        _parse(args);
        run_callback();
    }

    /// Provide a function to print a help message. The function gets access to the App pointer and error.
    void set_failure_message(std::function<std::string(const App *, const Error &e)> function) {
        failure_message_ = function;
    }

    /// Print a nice error message and return the exit code
    int exit(const Error &e, std::ostream &out = std::cout, std::ostream &err = std::cerr) const {

        /// Avoid printing anything if this is a CLI::RuntimeError
        if(dynamic_cast<const CLI::RuntimeError *>(&e) != nullptr)
            return e.get_exit_code();

        if(dynamic_cast<const CLI::CallForHelp *>(&e) != nullptr) {
            out << help();
            return e.get_exit_code();
        }

        if(e.get_exit_code() != static_cast<int>(ExitCodes::Success)) {
            if(failure_message_)
                err << failure_message_(this, e) << std::flush;
        }

        return e.get_exit_code();
    }

    /// Reset the parsed data
    void reset() {

        parsed_ = false;
        missing_.clear();
        parsed_subcommands_.clear();

        for(const Option_p &opt : options_) {
            opt->clear();
        }
        for(const App_p &app : subcommands_) {
            app->reset();
        }
    }

    ///@}
    /// @name Post parsing
    ///@{

    /// Counts the number of times the given option was passed.
    size_t count(std::string name) const {
        for(const Option_p &opt : options_) {
            if(opt->check_name(name)) {
                return opt->count();
            }
        }
        throw OptionNotFound(name);
    }

    /// Get a subcommand pointer list to the currently selected subcommands (after parsing by default, in command line
    /// order)
    std::vector<App *> get_subcommands(bool parsed = true) const {
        if(parsed) {
            return parsed_subcommands_;
        } else {
            std::vector<App *> subcomms(subcommands_.size());
            std::transform(std::begin(subcommands_), std::end(subcommands_), std::begin(subcomms), [](const App_p &v) {
                return v.get();
            });
            return subcomms;
        }
    }

    /// Check to see if given subcommand was selected
    bool got_subcommand(App *subcom) const {
        // get subcom needed to verify that this was a real subcommand
        return get_subcommand(subcom)->parsed_;
    }

    /// Check with name instead of pointer to see if subcommand was selected
    bool got_subcommand(std::string name) const { return get_subcommand(name)->parsed_; }

    ///@}
    /// @name Help
    ///@{

    /// Set footer.
    App *set_footer(std::string footer) {
        footer_ = footer;
        return this;
    }

    /// Produce a string that could be read in as a config of the current values of the App. Set default_also to include
    /// default arguments. Prefix will add a string to the beginning of each option.
    std::string config_to_str(bool default_also = false, std::string prefix = "") const {
        std::stringstream out;
        for(const Option_p &opt : options_) {

            // Only process option with a long-name and configurable
            if(!opt->lnames_.empty() && opt->get_configurable()) {
                std::string name = prefix + opt->lnames_[0];

                // Non-flags
                if(opt->get_expected() != 0) {

                    // If the option was found on command line
                    if(opt->count() > 0)
                        out << name << "=" << detail::inijoin(opt->results()) << std::endl;

                    // If the option has a default and is requested by optional argument
                    else if(default_also && !opt->defaultval_.empty())
                        out << name << "=" << opt->defaultval_ << std::endl;
                    // Flag, one passed
                } else if(opt->count() == 1) {
                    out << name << "=true" << std::endl;

                    // Flag, multiple passed
                } else if(opt->count() > 1) {
                    out << name << "=" << opt->count() << std::endl;

                    // Flag, not present
                } else if(opt->count() == 0 && default_also) {
                    out << name << "=false" << std::endl;
                }
            }
        }
        for(const App_p &subcom : subcommands_)
            out << subcom->config_to_str(default_also, prefix + subcom->name_ + ".");
        return out.str();
    }

    /// Makes a help message, with a column wid for column 1
    std::string help(size_t wid = 30, std::string prev = "", bool rst = false) const {
        // Delegate to subcommand if needed
        if(prev.empty())
            prev = name_;
        else
            prev += " " + name_;

        auto selected_subcommands = get_subcommands();
        if(!selected_subcommands.empty())
            return selected_subcommands.at(0)->help(wid, prev);

        std::stringstream out;
        out << description_ << std::endl;
        if (rst)
            out << std::endl;
        out << (rst ? "**Usage:** " : "Usage: ") << prev;

        // Check for options_
        bool npos = false;
        std::set<std::string> groups;
        for(const Option_p &opt : options_) {
            if(opt->nonpositional()) {
                npos = true;
                groups.insert(opt->get_group());
            }
        }

        if(npos)
            out << " [OPTIONS]";

        // Positionals
        bool pos = false;
        for(const Option_p &opt : options_)
            if(opt->get_positional()) {
                // A hidden positional should still show up in the usage statement
                // if(detail::to_lower(opt->get_group()).empty())
                //    continue;
                out << " " << opt->help_positional();
                if(opt->_has_help_positional())
                    pos = true;
            }

        if(!subcommands_.empty()) {
            if(require_subcommand_min_ > 0)
                out << " SUBCOMMAND";
            else
                out << " [SUBCOMMAND]";
        }

        out << std::endl;

        // Positional descriptions
        if(pos) {
            if (rst)
                out << std::endl << "**Positionals:**" << std::endl << std::endl;
            else
                out << std::endl << "Positionals:" << std::endl;
            int dwid = 0;
            if (rst) {
                dwid = (*std::max_element(options_.begin(), options_.end(), []( const Option_p& o1, const Option_p& o2 ) { return o1->get_description().size() < o2->get_description().size(); } ))->get_description().size();
                out << std::string(wid - 1, '=') << " " << std::string(dwid, '=') << std::endl;
            }
            for(const Option_p &opt : options_) {
                if(detail::to_lower(opt->get_group()).empty())
                    continue; // Hidden
                if(opt->_has_help_positional())
                    detail::format_help(out, opt->help_pname(), opt->get_description(), wid, rst);
            }
            if (rst)
                out << std::string(wid - 1, '=') << " " << std::string(dwid, '=') << std::endl;
        }

        // Options
        if(npos) {
            for(const std::string &group : groups) {
                if(detail::to_lower(group).empty())
                    continue; // Hidden
                if (rst)
                    out << std::endl << "**" << group << ":**" << std::endl << std::endl;
                else
                    out << std::endl << group << ":" << std::endl;
                
                int dwid = 0;
                if (rst) {
                    dwid = (*std::max_element(options_.begin(), options_.end(), []( const Option_p& o1, const Option_p& o2 ) { return o1->get_description().size() < o2->get_description().size(); } ))->get_description().size();
                    out << std::string(wid - 1, '=') << " " << std::string(dwid, '=') << std::endl;
                }
                for(const Option_p &opt : options_) {
                    if(opt->nonpositional() && opt->get_group() == group)
                        detail::format_help(out, opt->help_name(true), opt->get_description(), wid, rst);
                }
                if (rst)
                    out << std::string(wid - 1, '=') << " " << std::string(dwid, '=') << std::endl;
            }
        }

        // Subcommands
        if(!subcommands_.empty()) {
            std::set<std::string> subcmd_groups_seen;
            for(const App_p &com : subcommands_) {
                const std::string &group_key = detail::to_lower(com->get_group());
                if(group_key.empty() || subcmd_groups_seen.count(group_key) != 0)
                    continue; // Hidden or not in a group

                subcmd_groups_seen.insert(group_key);
                out << std::endl << com->get_group() << ":" << std::endl;
                for(const App_p &new_com : subcommands_)
                    if(detail::to_lower(new_com->get_group()) == group_key)
                        detail::format_help(out, new_com->get_name(), new_com->description_, wid, rst);
            }
        }

        if(!footer_.empty()) {
            out << std::endl << footer_ << std::endl;
        }

        return out.str();
    }

    ///@}
    /// @name Getters
    ///@{

    /// Check the status of ignore_case
    bool get_ignore_case() const { return ignore_case_; }

    /// Check the status of fallthrough
    bool get_fallthrough() const { return fallthrough_; }

    /// Get the group of this subcommand
    const std::string &get_group() const { return group_; }

    /// Get footer.
    std::string get_footer() const { return footer_; }

    /// Get the required min subcommand value
    size_t get_require_subcommand_min() const { return require_subcommand_min_; }

    /// Get the required max subcommand value
    size_t get_require_subcommand_max() const { return require_subcommand_max_; }

    /// Get the prefix command status
    bool get_prefix_command() const { return prefix_command_; }

    /// Get the status of allow extras
    bool get_allow_extras() const { return allow_extras_; }

    /// Get a pointer to the help flag.
    Option *get_help_ptr() { return help_ptr_; }

    /// Get a pointer to the help flag. (const)
    const Option *get_help_ptr() const { return help_ptr_; }

    /// Get a pointer to the config option.
    Option *get_config_ptr() { return config_ptr_; }

    /// Get a pointer to the config option. (const)
    const Option *get_config_ptr() const { return config_ptr_; }
    /// Get the name of the current app
    std::string get_name() const { return name_; }

    /// Check the name, case insensitive if set
    bool check_name(std::string name_to_check) const {
        std::string local_name = name_;
        if(ignore_case_) {
            local_name = detail::to_lower(name_);
            name_to_check = detail::to_lower(name_to_check);
        }

        return local_name == name_to_check;
    }

    /// This gets a vector of pointers with the original parse order
    const std::vector<Option *> &parse_order() const { return parse_order_; }

    /// This retuns the missing options from the current subcommand
    std::vector<std::string> remaining(bool recurse = false) const {
        std::vector<std::string> miss_list;
        for(const std::pair<detail::Classifer, std::string> &miss : missing_) {
            miss_list.push_back(std::get<1>(miss));
        }
        if(recurse) {
            for(const App *sub : parsed_subcommands_) {
                std::vector<std::string> output = sub->remaining(recurse);
                std::copy(std::begin(output), std::end(output), std::back_inserter(miss_list));
            }
        }
        return miss_list;
    }

    /// This returns the number of remaining options, minus the -- seperator
    size_t remaining_size(bool recurse = false) const {
        size_t count = std::count_if(
            std::begin(missing_), std::end(missing_), [](const std::pair<detail::Classifer, std::string> &val) {
                return val.first != detail::Classifer::POSITIONAL_MARK;
            });
        if(recurse) {
            for(const App_p &sub : subcommands_) {
                count += sub->remaining_size(recurse);
            }
        }
        return count;
    }

    ///@}

  protected:
    /// Check the options to make sure there are no conflicts.
    ///
    /// Currently checks to see if multiple positionals exist with -1 args
    void _validate() const {
        auto count = std::count_if(std::begin(options_), std::end(options_), [](const Option_p &opt) {
            return opt->get_expected() == -1 && opt->get_positional();
        });
        if(count > 1)
            throw InvalidError(name_);
        for(const App_p &app : subcommands_)
            app->_validate();
    }

    /// Internal function to run (App) callback, top down
    void run_callback() {
        pre_callback();
        if(callback_)
            callback_();
        for(App *subc : get_subcommands()) {
            subc->run_callback();
        }
    }

    /// Check to see if a subcommand is valid. Give up immediately if subcommand max has been reached.
    bool _valid_subcommand(const std::string &current) const {
        // Don't match if max has been reached - but still check parents
        if(require_subcommand_max_ != 0 && parsed_subcommands_.size() >= require_subcommand_max_) {
            return parent_ != nullptr && parent_->_valid_subcommand(current);
        }

        for(const App_p &com : subcommands_)
            if(com->check_name(current) && !*com)
                return true;

        // Check parent if exists, else return false
        return parent_ != nullptr && parent_->_valid_subcommand(current);
    }

    /// Selects a Classifier enum based on the type of the current argument
    detail::Classifer _recognize(const std::string &current) const {
        std::string dummy1, dummy2;

        if(current == "--")
            return detail::Classifer::POSITIONAL_MARK;
        if(_valid_subcommand(current))
            return detail::Classifer::SUBCOMMAND;
        if(detail::split_long(current, dummy1, dummy2))
            return detail::Classifer::LONG;
        if(detail::split_short(current, dummy1, dummy2))
            return detail::Classifer::SHORT;
        return detail::Classifer::NONE;
    }

    /// Internal parse function
    void _parse(std::vector<std::string> &args) {
        parsed_ = true;
        bool positional_only = false;

        while(!args.empty()) {
            _parse_single(args, positional_only);
        }

        if(help_ptr_ != nullptr && help_ptr_->count() > 0) {
            throw CallForHelp();
        }

        // Process an INI file
        if(config_ptr_ != nullptr) {
            if(*config_ptr_) {
                config_ptr_->run_callback();
                config_required_ = true;
            }
            if(!config_name_.empty()) {
                try {
                    std::vector<detail::ini_ret_t> values = detail::parse_ini(config_name_);
                    while(!values.empty()) {
                        if(!_parse_ini(values)) {
                            throw INIError::Extras(values.back().fullname);
                        }
                    }
                } catch(const FileError &) {
                    if(config_required_)
                        throw;
                }
            }
        }

        // Get envname options if not yet passed
        for(const Option_p &opt : options_) {
            if(opt->count() == 0 && !opt->envname_.empty()) {
                char *buffer = nullptr;
                std::string ename_string;

#ifdef _MSC_VER
                // Windows version
                size_t sz = 0;
                if(_dupenv_s(&buffer, &sz, opt->envname_.c_str()) == 0 && buffer != nullptr) {
                    ename_string = std::string(buffer);
                    free(buffer);
                }
#else
                // This also works on Windows, but gives a warning
                buffer = std::getenv(opt->envname_.c_str());
                if(buffer != nullptr)
                    ename_string = std::string(buffer);
#endif

                if(!ename_string.empty()) {
                    opt->add_result(ename_string);
                }
            }
        }

        // Process callbacks
        for(const Option_p &opt : options_) {
            if(opt->count() > 0 && !opt->get_callback_run()) {
                opt->run_callback();
            }
        }

        // Verify required options
        for(const Option_p &opt : options_) {
            // Required or partially filled
            if(opt->get_required() || opt->count() != 0) {
                // Make sure enough -N arguments parsed (+N is already handled in parsing function)
                if(opt->get_expected() < 0 && opt->count() < static_cast<size_t>(-opt->get_expected()))
                    throw ArgumentMismatch::AtLeast(opt->single_name(), -opt->get_expected());

                // Required but empty
                if(opt->get_required() && opt->count() == 0)
                    throw RequiredError(opt->single_name());
            }
            // Requires
            for(const Option *opt_req : opt->requires_)
                if(opt->count() > 0 && opt_req->count() == 0)
                    throw RequiresError(opt->single_name(), opt_req->single_name());
            // Excludes
            for(const Option *opt_ex : opt->excludes_)
                if(opt->count() > 0 && opt_ex->count() != 0)
                    throw ExcludesError(opt->single_name(), opt_ex->single_name());
        }

        auto selected_subcommands = get_subcommands();
        if(require_subcommand_min_ > selected_subcommands.size())
            throw RequiredError::Subcommand(require_subcommand_min_);

        // Convert missing (pairs) to extras (string only)
        if(!(allow_extras_ || prefix_command_)) {
            size_t num_left_over = remaining_size();
            if(num_left_over > 0) {
                args = remaining(false);
                std::reverse(std::begin(args), std::end(args));
                throw ExtrasError(args);
            }
        }
    }

    /// Parse one ini param, return false if not found in any subcommand, remove if it is
    ///
    /// If this has more than one dot.separated.name, go into the subcommand matching it
    /// Returns true if it managed to find the option, if false you'll need to remove the arg manually.
    bool _parse_ini(std::vector<detail::ini_ret_t> &args) {
        detail::ini_ret_t &current = args.back();
        std::string parent = current.parent(); // respects current.level
        std::string name = current.name();

        // If a parent is listed, go to a subcommand
        if(!parent.empty()) {
            current.level++;
            for(const App_p &com : subcommands_)
                if(com->check_name(parent))
                    return com->_parse_ini(args);
            return false;
        }

        auto op_ptr = std::find_if(
            std::begin(options_), std::end(options_), [name](const Option_p &v) { return v->check_lname(name); });

        if(op_ptr == std::end(options_))
            return false;

        // Let's not go crazy with pointer syntax
        Option_p &op = *op_ptr;

        if(!op->get_configurable())
            throw INIError::NotConfigurable(current.fullname);

        if(op->results_.empty()) {
            // Flag parsing
            if(op->get_expected() == 0) {
                if(current.inputs.size() == 1) {
                    std::string val = current.inputs.at(0);
                    val = detail::to_lower(val);
                    if(val == "true" || val == "on" || val == "yes")
                        op->results_ = {""};
                    else if(val == "false" || val == "off" || val == "no")
                        ;
                    else
                        try {
                            size_t ui = std::stoul(val);
                            for(size_t i = 0; i < ui; i++)
                                op->results_.emplace_back("");
                        } catch(const std::invalid_argument &) {
                            throw ConversionError::TrueFalse(current.fullname);
                        }
                } else
                    throw ConversionError::TooManyInputsFlag(current.fullname);
            } else {
                op->results_ = current.inputs;
                op->run_callback();
            }
        }

        args.pop_back();
        return true;
    }

    /// Parse "one" argument (some may eat more than one), delegate to parent if fails, add to missing if missing from
    /// master
    void _parse_single(std::vector<std::string> &args, bool &positional_only) {

        detail::Classifer classifer = positional_only ? detail::Classifer::NONE : _recognize(args.back());
        switch(classifer) {
        case detail::Classifer::POSITIONAL_MARK:
            missing_.emplace_back(classifer, args.back());
            args.pop_back();
            positional_only = true;
            break;
        case detail::Classifer::SUBCOMMAND:
            _parse_subcommand(args);
            break;
        case detail::Classifer::LONG:
            // If already parsed a subcommand, don't accept options_
            _parse_arg(args, true);
            break;
        case detail::Classifer::SHORT:
            // If already parsed a subcommand, don't accept options_
            _parse_arg(args, false);
            break;
        case detail::Classifer::NONE:
            // Probably a positional or something for a parent (sub)command
            _parse_positional(args);
        }
    }

    /// Count the required remaining positional arguments
    size_t _count_remaining_positionals(bool required = false) const {
        size_t retval = 0;
        for(const Option_p &opt : options_)
            if(opt->get_positional() && (!required || opt->get_required()) && opt->get_expected() > 0 &&
               static_cast<int>(opt->count()) < opt->get_expected())
                retval = static_cast<size_t>(opt->get_expected()) - opt->count();

        return retval;
    }

    /// Parse a positional, go up the tree to check
    void _parse_positional(std::vector<std::string> &args) {

        std::string positional = args.back();
        for(const Option_p &opt : options_) {
            // Eat options, one by one, until done
            if(opt->get_positional() &&
               (static_cast<int>(opt->count()) < opt->get_expected() || opt->get_expected() < 0)) {

                opt->add_result(positional);
                parse_order_.push_back(opt.get());
                args.pop_back();
                return;
            }
        }

        if(parent_ != nullptr && fallthrough_)
            return parent_->_parse_positional(args);
        else {
            args.pop_back();
            missing_.emplace_back(detail::Classifer::NONE, positional);

            if(prefix_command_) {
                while(!args.empty()) {
                    missing_.emplace_back(detail::Classifer::NONE, args.back());
                    args.pop_back();
                }
            }
        }
    }

    /// Parse a subcommand, modify args and continue
    ///
    /// Unlike the others, this one will always allow fallthrough
    void _parse_subcommand(std::vector<std::string> &args) {
        if(_count_remaining_positionals(/* required */ true) > 0)
            return _parse_positional(args);
        for(const App_p &com : subcommands_) {
            if(com->check_name(args.back())) {
                args.pop_back();
                if(std::find(std::begin(parsed_subcommands_), std::end(parsed_subcommands_), com.get()) ==
                   std::end(parsed_subcommands_))
                    parsed_subcommands_.push_back(com.get());
                com->_parse(args);
                return;
            }
        }
        if(parent_ != nullptr)
            return parent_->_parse_subcommand(args);
        else
            throw HorribleError("Subcommand " + args.back() + " missing");
    }

    /// Parse a short (false) or long (true) argument, must be at the top of the list
    void _parse_arg(std::vector<std::string> &args, bool second_dash) {

        detail::Classifer current_type = second_dash ? detail::Classifer::LONG : detail::Classifer::SHORT;

        std::string current = args.back();

        std::string name;
        std::string value;
        std::string rest;

        if(second_dash) {
            if(!detail::split_long(current, name, value))
                throw HorribleError("Long parsed but missing (you should not see this):" + args.back());
        } else {
            if(!detail::split_short(current, name, rest))
                throw HorribleError("Short parsed but missing! You should not see this");
        }

        auto op_ptr = std::find_if(std::begin(options_), std::end(options_), [name, second_dash](const Option_p &opt) {
            return second_dash ? opt->check_lname(name) : opt->check_sname(name);
        });

        // Option not found
        if(op_ptr == std::end(options_)) {
            // If a subcommand, try the master command
            if(parent_ != nullptr && fallthrough_)
                return parent_->_parse_arg(args, second_dash);
            // Otherwise, add to missing
            else {
                args.pop_back();
                missing_.emplace_back(current_type, current);
                return;
            }
        }

        args.pop_back();

        // Get a reference to the pointer to make syntax bearable
        Option_p &op = *op_ptr;

        int num = op->get_expected();

        if(!value.empty()) {
            if(num != -1)
                num--;
            op->add_result(value);
            parse_order_.push_back(op.get());
        } else if(num == 0) {
            op->add_result("");
            parse_order_.push_back(op.get());
        } else if(!rest.empty()) {
            if(num > 0)
                num--;
            op->add_result(rest);
            parse_order_.push_back(op.get());
            rest = "";
        }

        // Unlimited vector parser
        if(num < 0) {
            int collected = 0; // Make sure we always eat the minimum
            while(!args.empty() && _recognize(args.back()) == detail::Classifer::NONE) {
                if(collected >= -num) {
                    // We could break here for allow extras, but we don't

                    // If any positionals remain, don't keep eating
                    if(_count_remaining_positionals() > 0)
                        break;

                    // If there are any unlimited positionals, those also take priority
                    if(std::any_of(std::begin(options_), std::end(options_), [](const Option_p &opt) {
                           return opt->get_positional() && opt->get_expected() < 0;
                       }))
                        break;
                }
                op->add_result(args.back());
                parse_order_.push_back(op.get());
                args.pop_back();
                collected++;
            }

        } else {
            while(num > 0 && !args.empty()) {
                num--;
                std::string current_ = args.back();
                args.pop_back();
                op->add_result(current_);
                parse_order_.push_back(op.get());
            }

            if(num > 0) {
                throw ArgumentMismatch::TypedAtLeast(op->single_name(), num, op->get_type_name());
            }
        }

        if(!rest.empty()) {
            rest = "-" + rest;
            args.push_back(rest);
        }
    }
};

namespace FailureMessage {

inline std::string simple(const App *app, const Error &e) {
    std::string header = std::string(e.what()) + "\n";
    if(app->get_help_ptr() != nullptr)
        header += "Run with " + app->get_help_ptr()->single_name() + " for more information.\n";
    return header;
}

inline std::string help(const App *app, const Error &e) {
    std::string header = std::string("ERROR: ") + e.get_name() + ": " + e.what() + "\n";
    header += app->help();
    return header;
}

} // namespace FailureMessage

namespace detail {
/// This class is simply to allow tests access to App's protected functions
struct AppFriend {

    /// Wrap _parse_short, perfectly forward arguments and return
    template <typename... Args>
    static auto parse_arg(App *app, Args &&... args) ->
        typename std::result_of<decltype (&App::_parse_arg)(App, Args...)>::type {
        return app->_parse_arg(std::forward<Args>(args)...);
    }

    /// Wrap _parse_subcommand, perfectly forward arguments and return
    template <typename... Args>
    static auto parse_subcommand(App *app, Args &&... args) ->
        typename std::result_of<decltype (&App::_parse_subcommand)(App, Args...)>::type {
        return app->_parse_subcommand(std::forward<Args>(args)...);
    }
};
} // namespace detail

} // namespace CLI
