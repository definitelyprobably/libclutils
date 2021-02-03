// 
// This file is part of the libclutils library.
// Copyright (c) 2021, Karta Kooner, GNU GPL v3+
//

#ifndef DEFPROB_CLUTILS_CLUTILS_HPP
#define DEFPROB_CLUTILS_CLUTILS_HPP


#include <limits>
#include <concepts>
#include <stdexcept>
#include <initializer_list>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <tuple>
#include <variant>
#include <unordered_map>
#include <unordered_set>


namespace CLUtils {

using CLType = unsigned long long;


template <typename TChar>
std::basic_string<TChar> to_string(const CLType& val);

template <>
std::string to_string<char>(const CLType& val);

template <>
std::wstring to_string<wchar_t>(const CLType& val);


// Concepts
template <typename T>
concept PosType = std::convertible_to<std::remove_cvref_t<T>, CLType>;


// Exceptions
inline namespace Exception {
struct CLUtilsException;
struct FlagNameError;
struct NameConflictError;
struct InternalError;
}


// forward declare BasicName for the Literals:: user-defined literals
template <typename T>
class BasicName;


// user-defined literals for BasicName class
inline namespace Literals {

BasicName<char> operator"" _short(const char* str, std::size_t);

BasicName<char> operator"" _long(const char* str, std::size_t);

BasicName<wchar_t> operator"" _short(const wchar_t* str, std::size_t);

BasicName<wchar_t> operator"" _long(const wchar_t* str, std::size_t);

BasicName<char8_t> operator"" _short(const char8_t* str, std::size_t);

BasicName<char8_t> operator"" _long(const char8_t* str, std::size_t);

BasicName<char16_t> operator"" _short(const char16_t* str, std::size_t);

BasicName<char16_t> operator"" _long(const char16_t* str, std::size_t);

BasicName<char32_t> operator"" _short(const char32_t* str, std::size_t);

BasicName<char32_t> operator"" _long(const char32_t* str, std::size_t);

} // namespace Literals


enum class FlagType { unset, short_type, long_type };

enum class FlagClass { bare, optional, mandatory, stop };

enum class InputType { unset, internal, external };

enum class Aggressive { no, yes };

enum class Greedy { no, lax, yes };
  // greedy controls the bahaviour of mandatory flags:
  // (mandatory flag: -m, unrecognized flag: -A):
  //       |     no        .    lax        .   yes
  // ------|---------------.---------------.-----------
  // -m -m | missing input . missing input . input: -m
  // -m -A | missing input . input: -A     . input: -A
  // -m A  | input: A      . input: A      . input: A
  //
  // for optional flags, this behaviour is modified:
  // (optional flag: -o, unrecognized flag: -A),
  // (Note: generally, errors for mandatory flags are
  //  now acceptable as -o having no input; also note:
  //  another recognized flag is _never_ taken as an
  //  input):
  //       |     no        .    lax        .   yes
  // ------|---------------.---------------.-----------
  // -o -o | no input      . no input      . no input
  // -o -A | no input      . input: -A     . input: -A
  // -o A  | input: A      . input: A      . input: A
  //
  // if allow_unrecognized_opts() is called, then the
  // behaviour of optional flags is modified in that
  // the flag will no longer have an input:
  //       |     no        .    lax        .   yes
  // ------|---------------.---------------.-----------
  // -o -o | no input      . no input      . no input
  // -o -A | no input      . no input      . no input
  // -o A  | input: A      . input: A      . input: A
  //
  // likewise if allow_arguments() is called:
  //       |     no        .    lax        .   yes
  // ------|---------------.---------------.-----------
  // -o -o | no input      . no input      . no input
  // -o -A | no input      . input: -A     . input: -A
  // -o A  | no input      . no input      . noinput
 

enum class ErrorKey {
  ArgEmpty = 1000,
  Unrecognized = 2000,
  Surplus = 3000,
  Surplus_ = 3010,
  Proscribed = 3100,
  Proscribed_ = 3110,
  StopProscribed = 3101,
  StopProscribed_ = 3111,
  EmptyInput = 4000,
  EmptyInput_ = 4010,
  BareEmptyInput = 4100,
  StopEmptyInput = 4101,
  BareInput = 4200,
  BareInput_ = 4210,
  StopInput = 4201,
  StopInput_ = 4211,
  MissingInput = 4300,
  MissingInput_ = 4310
};

// helper object for returning the ErroKeys
#ifdef _MSC_VER
const
#else
constinit const
#endif
std::initializer_list<ErrorKey> error_keys{
  ErrorKey::ArgEmpty,
  ErrorKey::Surplus,
  ErrorKey::Surplus_,
  ErrorKey::Proscribed,
  ErrorKey::Proscribed_,
  ErrorKey::Unrecognized,
  ErrorKey::EmptyInput,
  ErrorKey::EmptyInput_,
  ErrorKey::BareEmptyInput,
  ErrorKey::BareInput,
  ErrorKey::BareInput_,
  ErrorKey::MissingInput,
  ErrorKey::MissingInput_,
  ErrorKey::StopEmptyInput,
  ErrorKey::StopInput,
  ErrorKey::StopInput_,
  ErrorKey::StopProscribed,
  ErrorKey::StopProscribed_
};


// BasicName
//
// Type defining a flag name; associated to this name is the flag type.
// E.g., "--flag", type: long; "-f", type: short; etc.
//
// Flag declarations ultimately define BasicNames (since the flag declaration
// is effectively a name declaration).
//
template <typename TChar>
class BasicName {
public:
  using Char        = TChar;
  using value_type  = Char;
  using String      = std::basic_string<Char>;
  using StringView  = std::basic_string_view<Char>;
  using Data        = std::variant<String, StringView>;
  using FlagMarkers = std::unordered_set<Char>;

public:
  BasicName(const Char* name, const FlagType type = FlagType::unset);
  BasicName(const StringView name, const FlagType type = FlagType::unset);
  template <typename T>
  BasicName(T&& name, const FlagType type = FlagType::unset);
  template <typename T>
  BasicName(const T& name, const FlagType type = FlagType::unset);

  void check_name() const;
  void guess_type(const FlagMarkers& flag_markers, const bool tainted);

  [[nodiscard]] FlagType    type() const noexcept;
  [[nodiscard]] StringView  name() const noexcept;
  [[nodiscard]] typename StringView::size_type length() const noexcept;

private:
  Data      name_;
  FlagType  type_{FlagType::unset};
  bool      type_explicit_{false};
};



// BasicArg
//
// BasicArg is an argument instance on the command line. It will just contain
// the name of the argument itself, and the position on the command line.
//
template <typename TChar>
class BasicArg {
public:
  using Char       = TChar;
  using value_type = Char;
  using StringView = std::basic_string_view<Char>;

public:
  BasicArg(const StringView name, const CLType pos, const CLType subpos = 0);

  [[nodiscard]] StringView name() const noexcept;
  [[nodiscard]] CLType pos() const noexcept;
  [[nodiscard]] CLType subpos() const noexcept;

private:
  StringView name_;
  CLType     pos_;
  CLType     subpos_;
};



// BasicInput
//
// BasicInput is a class that holds the input to a flag. For example, for
// "--flag=input", this class would record info about the input to "--flag",
// namely "input" (and other related properties).
//
template <typename TChar>
class BasicInput {
public:
  using Char       = TChar;
  using value_type = Char;
  using StringView = std::basic_string_view<Char>;

public:
  BasicInput() = default;
  BasicInput(const StringView value, const InputType type);

  [[nodiscard]] bool operator()() const noexcept;

  [[nodiscard]] StringView  value() const noexcept;
  [[nodiscard]] InputType   type() const noexcept;

private:
  bool       is_set_{false};
  StringView value_{};
  InputType  type_{InputType::unset};
};



// BasicInstance
//
// This class holds all the data pertaining to an instance of a defined
// flag on the command-line. Specifically, the flag name, its command-line
// position (and sub-position) and any input to the flag.
//
template <typename TChar>
class BasicInstance {
public:
  using Char       = TChar;
  using value_type = Char;
  using StringView = std::basic_string_view<Char>;
  using Input      = BasicInput<Char>;

public:
  BasicInstance(const StringView name, const CLType pos);
  BasicInstance(const StringView name, const CLType pos,
                const CLType subpos);

  template <class... IArgs>
  BasicInstance(const StringView name, const CLType pos, IArgs... input);

  template <class... IArgs>
  BasicInstance(const StringView name, const CLType pos, const CLType subpos,
    IArgs... input);

  [[nodiscard]] StringView    name() const noexcept;
  [[nodiscard]] CLType        pos() const noexcept;
  [[nodiscard]] CLType        subpos() const noexcept;
  [[nodiscard]] const Input&  input() const noexcept;

private:
  StringView name_  {};
  CLType     pos_   {};
  CLType     subpos_{};
  Input      input_ {};
};



// BasicFlag
//
// This class holds all the data defining a flag with respect to parsing the
// command-line. For example, the synonyms of the flag, its class, how many
// times it may appear on the comamnd-line and also all instances of the
// flag on the command-line.
//
template <typename TChar>
class BasicFlag {
public:
  using Char = TChar;
  using value_type = Char;
  using StringView = std::basic_string_view<Char>;
  using Name = BasicName<Char>;
  using Instance = BasicInstance<Char>;
  using Input = BasicInput<Char>;
  using Names = std::vector<Name>;
  using Instances = std::vector<Instance>;
  using FlagMarkers = typename Name::FlagMarkers;
  enum class LimitType { Proscribed, Within, Without };

public:
  template <class... N, typename = std::common_type_t<N...>>
  BasicFlag(const FlagClass flag_class, N&&... names);

  template <PosType C, class... N, typename = std::common_type_t<N...>>
  BasicFlag(const FlagClass flag_class, const C max, N&&... names);

  void guess_types(const FlagMarkers& flag_markers, const bool tainted);
  [[nodiscard]] FlagClass        flag_class() const noexcept;
  [[nodiscard]] const Names&     names() const noexcept;
  [[nodiscard]] Names&           names() noexcept;
  [[nodiscard]] StringView       canonical_name() const noexcept;
  [[nodiscard]] CLType           max() const noexcept;
  [[nodiscard]] LimitType        within_limit() const noexcept;
  [[nodiscard]] const Instances& instances() const noexcept;
  [[nodiscard]] Instances&       instances() noexcept;
                void             clear() noexcept;

  void add_instance(const StringView name, const CLType pos,
                    const CLType subpos, const StringView input,
                    const InputType type);

  void add_instance(const StringView name, const CLType pos,
                    const CLType subpos);

private:
  FlagClass class_;
  Names     names_;
  Instances instances_{};
  CLType    max_{std::numeric_limits<CLType>::max()};
};



template <typename TChar>
class BasicOpts {
private:
  struct Parsing;
  struct ErrorInfo;

public:
  using Char         = TChar;
  using value_type   = Char;
  using StringView   = std::basic_string_view<Char>;
  using String       = std::basic_string<Char>;
  using CLOpts       = std::vector<String>;
  using Name         = BasicName<Char>;
  using Flag         = BasicFlag<Char>;
  using Arg          = BasicArg<Char>;
  using Flags        = std::vector<Flag>;
  using Args         = std::vector<Arg>;
  using Unrecognized = std::vector<Arg>;
  using ArgPos       = typename Args::difference_type;
  using UnrecognizedPos = typename Unrecognized::difference_type;
  using FlagPtr      = Flag*;
  using NamePtr      = Name*;
  using SearchRes    = std::tuple<NamePtr, FlagPtr>;
  using SearchAll    = std::vector<SearchRes>;
  using Map          = std::unordered_map<StringView, SearchRes>;
  using Instance     = typename Flag::Instance;
  using Instances    = typename Flag::Instances;
  using InstancePos  = typename Instances::difference_type;
  using InputResult  = std::tuple<bool, StringView>;
  using ErrorInput   = std::variant<StringView, CLType>;
  using Error        = std::tuple<ErrorInfo, String>;
  using Errors       = std::vector<Error>;
  using ErrorPos     = typename Errors::size_type;
  using ErrorInfos   = std::vector<ErrorInfo>;
  using ErrorMap     = std::unordered_map<ErrorKey, String>;
  using Keys         = std::vector<ErrorKey>;
  using CanonNames   = std::vector<StringView>;
  using FlagMarkers  = typename Flag::FlagMarkers;
  using LimitType    = typename Flag::LimitType;

private:
  struct Parsing {
    void clear() noexcept;
    void reset_all() noexcept;
    [[nodiscard]] bool operator()() const noexcept;

    bool     stop  {false};  // set to true if a stop flag has been given
    FlagPtr  pflag {nullptr};
    NamePtr  pname {nullptr};
    CLType   subpos{0};
  };

  struct ErrorInfo {
    // error: pos P: ...
    ErrorInfo(const ErrorKey k, const CLType p);

    // error: pos P: flag 'O': ... 'I'...
    ErrorInfo(const ErrorKey k, const CLType p,
          const StringView o, const bool f, const CLType i);

    // error: pos P: flag 'O': ... 'I'...
    ErrorInfo(const ErrorKey k, const CLType p,
          const StringView o, const bool f, const StringView i);

    // error: pos P: flag 'O': ...
    ErrorInfo(const ErrorKey k, const CLType p,
          const StringView o, const bool f);

    // error: pos P, subpos SP: flag 'O': ... 'I'...
    ErrorInfo(const ErrorKey k, const CLType p, const CLType sp,
          const StringView o, const bool f, const CLType i);

    // error: pos P, subpos SP: flag 'O': ... 'I'...
    ErrorInfo(const ErrorKey k, const CLType p, const CLType sp,
          const StringView o, const bool f, const StringView i);

    // error: pos P, subpos SP: flag 'O': ...
    ErrorInfo(const ErrorKey k, const CLType p, const CLType sp,
          const StringView o, const bool f);

    ErrorKey    key       {}; // key value is isomorphic to the error string
    CLType      pos       {};
    CLType      subpos    {};  // if subpos is empty, then there is no subpos
    bool        have_opt  {false};
    StringView  opt       {};
    bool        is_flag   {false}; // is_flag to true if the opt is a flag
    bool        have_input{false};
    ErrorInput  input     {}; // StringView or CLType
  };

public:
  template <class... N>
  void add_bare(N&&... names);
  void add_bare() = delete;

  template <class... N>
  void add_optional(N&&... names);
  void add_optional() = delete;

  template <class... N>
  void add_mandatory(N&&... names);
  void add_mandatory() = delete;

  template <class... N>
  void add_stop(N&&... names);
  void add_stop() = delete;

  void allow_empty_arguments(const bool state = true) noexcept;

  void allow_empty_inputs(const bool state = true) noexcept;

  template <PosType T>
  void allow_arguments(const T max) noexcept;

  void allow_arguments() noexcept;

  template <PosType T>
  void allow_unrecognized_flags(const T max, const Aggressive state = Aggressive::no) noexcept;

  void allow_unrecognized_flags(const Aggressive state = Aggressive::no) noexcept;

  template <PosType T>
  void allow_unrecognized_opts(const T max) noexcept;

  void allow_unrecognized_opts() noexcept;

  void allow_chaining(const bool state = true) noexcept;

  void set_chaining(const bool state = true) noexcept;

  void set_no_chaining() noexcept;

  void set_greedy(const bool state = true) noexcept;

  void set_not_greedy() noexcept;

  void set_lax_greedy() noexcept;

  void set_greedy(const Greedy state) noexcept;

  void set_optional_greedy(const bool state = true) noexcept;

  void set_optional_not_greedy() noexcept;

  template <typename Str>
  void set_preamble(Str&& str);

  template <typename Str>
  void set_postscript(Str&& str);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void set_flag_markers(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void set_flag_marker(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void add_flag_markers(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void add_flag_marker(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void set_opt_markers(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void set_opt_marker(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void add_opt_markers(Chars... flag_markers);

  template <class... Chars, typename = std::common_type_t<Chars...>>
  void add_opt_marker(Chars... flag_markers);

  void set_flag_marker() = delete;
  void set_flag_markers() = delete;
  void add_flag_marker() = delete;
  void add_flag_markers() = delete;
  void set_opt_marker() = delete;
  void set_opt_markers() = delete;
  void add_opt_marker() = delete;
  void add_opt_markers() = delete;

  void set_input_marker(const Char input_marker) noexcept;

  void clear() noexcept;

  void clear_errors() noexcept;

  void clear_declarations() noexcept;

  void default_error_formats() noexcept;

  [[nodiscard]] const Errors& get_all_errors() const noexcept;

  [[nodiscard]] Errors& get_all_errors() noexcept;

  [[nodiscard]] const Map& get_map();

  [[nodiscard]] bool have_opt(const StringView name) const;
  [[nodiscard]] bool has_opt(const StringView name) const;
  [[nodiscard]] bool have_flag(const StringView name) const;
  [[nodiscard]] bool has_flag(const StringView name) const;

  [[nodiscard]] Instances& get_all_instances(const StringView name);
  [[nodiscard]] const Instances& get_all_instances(const StringView name) const;

  [[nodiscard]] bool has_input(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] bool has_input(const Instance& instance) const noexcept;

  [[nodiscard]] InputResult get_input(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] InputResult get_input(const Instance& instance) const noexcept;

  [[nodiscard]] bool is_input_internal(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] bool is_input_internal(const Instance& instance) const noexcept;

  [[nodiscard]] bool is_input_external(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] bool is_input_external(const Instance& instance) const noexcept;

  [[nodiscard]] StringView get_name(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] StringView get_name(const Instance& instance) const noexcept;

  [[nodiscard]] StringView get_opt_name(const StringView name,
                             const InstancePos pos = -1) const;
  [[nodiscard]] StringView get_opt_name(const Instance& instance) const noexcept;

  [[nodiscard]] StringView get_flag_name(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] StringView get_flag_name(const Instance& instance) const noexcept;

  [[nodiscard]] CLType get_pos(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] CLType get_pos(const Instance& instance) const noexcept;

  [[nodiscard]] CLType get_subpos(const StringView name, const InstancePos pos = -1) const;
  [[nodiscard]] CLType get_subpos(const Instance& instance) const noexcept;

  [[nodiscard]] Unrecognized& get_all_unrecognized_flags() noexcept;
  [[nodiscard]] const Unrecognized& get_all_unrecognized_flags() const noexcept;
  [[nodiscard]] Unrecognized& get_all_unrecognized_opts() noexcept;
  [[nodiscard]] const Unrecognized& get_all_unrecognized_opts() const noexcept;

  [[nodiscard]] Arg& get_unrecognized_flag(const UnrecognizedPos pos = -1);
  [[nodiscard]] const Arg& get_unrecognized_flag(const UnrecognizedPos pos = -1) const;
  [[nodiscard]] Arg& get_unrecognized_opt(const UnrecognizedPos pos = -1);
  [[nodiscard]] const Arg& get_unrecognized_opt(const UnrecognizedPos pos = -1) const;

  [[nodiscard]] Args& get_all_arguments() noexcept;
  [[nodiscard]] const Args& get_all_arguments() const noexcept;

  [[nodiscard]] Arg& get_argument(const ArgPos pos = -1);
  [[nodiscard]] const Arg& get_argument(const ArgPos pos = -1) const;

  [[nodiscard]] StringView get_preamble() const;
  [[nodiscard]] StringView get_postscript() const;

  template <PosType T>
  bool parse(T argc, Char** argv, T start_at = 1);

  template <typename T>
  bool parse(T&& args);

  template <typename T, PosType U>
  bool parse(T&& argv, U start_at);

  template <class... T, typename = std::common_type_t<T...>>
  bool parse(T&&... args);

  bool parse();

  template <typename Str>
  void format_error(const ErrorKey key, Str&& str);

  template <typename T>
  T& write_errors(T& out);

  template <typename T, typename U>
  T& write_error(T& out, const U pos);

  template <typename T, class... U>
  T& write_errors_with(T& out, const U... keys);

  template <typename T>
  T& write_errors_with(T&) = delete;

  [[nodiscard]] CanonNames registered_flags() const;
  [[nodiscard]] CanonNames registered_opts() const;

private:
  [[nodiscard]] String get_error_string(const ErrorKey key) const;

  void guess_types();

  void create_map();

  template <typename... T>
  void register_error(const ErrorKey key, T&&... data);

  void cache_errors();

  [[nodiscard]] String create_error_string(const ErrorInfo& error) const;

  [[nodiscard]] String get_error_string_prototype(const ErrorKey key) const;

  void convert_error_str(String& str, const ErrorInfo& error) const;

  [[nodiscard]] SearchAll search_flag_all(const StringView opt,
                                          const bool short_only) const;

  [[nodiscard]] SearchRes search_flag(const StringView opt, const bool stop,
                                      const bool short_only = false) const;

  [[nodiscard]] bool opt_is_flag(const StringView opt) const;

  [[nodiscard]] bool opt_is_arg(const StringView opt) const;

  void add_unrecognized_flag(const StringView flag, const CLType pos);

  void add_argument(const StringView arg, const CLType pos);

  [[nodiscard]] bool handle_unrecognized(const StringView opt,
                                         const CLType pos, const bool stop);

  [[nodiscard]] bool check_within_limit(const FlagPtr pflag,
                                        const NamePtr pname,
                                        const CLType pos, const bool is_flag);

  [[nodiscard]] bool check_within_limit(const Parsing& prev, const CLType pos,
                                        const bool is_flag,
                                        const bool check_subpos = false);

  [[nodiscard]] bool add_instance(const Parsing& prev, const CLType pos,
                                  const StringView input);

  void add_instance(const Parsing& prev, const CLType pos);

  void add_instance(const FlagPtr pflag, const NamePtr pname,
                    const CLType pos, const CLType subpos);

  void add_instance(const FlagPtr pflag, const NamePtr pname,
                    const CLType pos);

  [[nodiscard]] bool add_instance(const FlagPtr pflag, const NamePtr pname,
                                  const CLType pos, const CLType subpos,
                                  const StringView input, const InputType type);

  [[nodiscard]] bool add_instance(const FlagPtr pflag, const NamePtr pname,
                                  const CLType pos, const StringView input,
                                  const InputType type);

  [[nodiscard]] bool are_collecting_args() const noexcept;

  [[nodiscard]] bool args_within_limit() const noexcept;

  [[nodiscard]] bool are_collecting_unrecognized_flags() const noexcept;

  [[nodiscard]] bool unrecognized_flags_within_limit() const noexcept;

  [[nodiscard]] bool optional_can_take_no_input() const noexcept;

  [[nodiscard]] bool mandatory_and_greedy(const Parsing& prev) const noexcept;

  [[nodiscard]] bool optional_and_greedy(const Parsing& prev) const noexcept;

  template <typename T>
  [[nodiscard]] bool save_opts(T&);

  template <typename T>
  [[nodiscard]] bool save_opts(T&& argv);

  template <class... T>
  void save_opts_pack(T&&... args);

  void parse_opt(const CLType pos, const StringView opt, Parsing& prev,
                 bool& errors);

  void parse_opt_finish(const CLType last_pos, Parsing& prev, bool& errors);

  [[nodiscard]] bool parse_short_opt(const StringView opt, const FlagPtr pflag,
                                     const NamePtr pname, const CLType pos,
                                     Parsing& prev);

  [[nodiscard]] bool parse_long_opt(const StringView opt, const FlagPtr pflag,
                                    const NamePtr pname, const CLType pos,
                                    Parsing& prev);

private:
  // parsing behaviour
  bool         allow_empty_arg_{false};
  bool         allow_empty_input_{false};
  bool         can_chain_{true};
  Greedy       mandatory_greedy_{Greedy::no};
  bool         optional_greedy_{false};
  bool         unrecognized_greedy_{false};
  CLType       collect_unrecognized_flags_{0};
  CLType       collect_unrecognized_flags_count_{0};
  CLType       collect_args_{0};
  CLType       collect_args_count_{0};

  // user-defined formatting
  String       preamble_{};
  String       postscript_{};
  ErrorMap     custom_error_message_{};

  // flag characteristics
  FlagMarkers   flag_markers_{static_cast<Char>('-')};
  Char         input_marker_{static_cast<Char>('=')};

  // parsing results (also flag declarations)
  Flags        flags_{};
  Args         args_{};
  Unrecognized unrecognized_flags_{};
  Errors       cached_error_strings_{};

  // parsing generated data
  CLOpts       cl_opts_{};
  Map          map_{};
  ErrorInfos   errors_{};

  // internal (helper) switches
  bool         flag_type_tainted_{true};
  bool         map_tainted_{true};
};


// using statements for std string objects
using Opts    = BasicOpts<char>;
using WOpts   = BasicOpts<wchar_t>;
using U8Opts  = BasicOpts<char8_t>;
using U16Opts = BasicOpts<char16_t>;
using U32Opts = BasicOpts<char32_t>;

} // namespace CLUtils

#include <clutils.tcc>

#endif // DEFPROB_CLUTILS_CLUTILS_HPP
