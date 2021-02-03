// 
// This file is part of the libclutils library.
// Copyright (c) 2021, Karta Kooner, GNU GPL v3+
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


namespace CLUtils {


template <typename TChar>
std::basic_string<TChar> to_string(const CLType& val) {
  std::basic_stringstream<TChar> ss;
  ss << val;
  std::basic_string<TChar> ret;
  ss >> ret;
  return ret;
}

template <>
std::string to_string<char>(const CLType& val) {
  return std::to_string(val);
}

template <>
std::wstring to_string<wchar_t>(const CLType& val) {
  return std::to_wstring(val);
}


inline namespace Exception {

struct CLUtilsException : std::logic_error {
  explicit CLUtilsException(const char* mess)
    : std::logic_error{mess} {}
};

struct FlagNameError : CLUtilsException {
  explicit FlagNameError(const char* mess)
    : CLUtilsException{mess} {}
};

struct NameConflictError : CLUtilsException {
  explicit NameConflictError(const char* mess)
    : CLUtilsException{mess} {}
};

struct InternalError : CLUtilsException {
  explicit InternalError(const char* mess)
    : CLUtilsException{mess} {}
};

} // namespace Exception


inline namespace Literals {

BasicName<char> operator"" _short(const char* str, std::size_t) {
  return BasicName<char>(str, FlagType::short_type);
}

BasicName<char> operator"" _long(const char* str, std::size_t) {
  return BasicName<char>(str, FlagType::long_type);
}

BasicName<wchar_t> operator"" _short(const wchar_t* str, std::size_t) {
  return BasicName<wchar_t>(str, FlagType::short_type);
}

BasicName<wchar_t> operator"" _long(const wchar_t* str, std::size_t) {
  return BasicName<wchar_t>(str, FlagType::long_type);
}

BasicName<char8_t> operator"" _short(const char8_t* str, std::size_t) {
  return BasicName<char8_t>(str, FlagType::short_type);
}

BasicName<char8_t> operator"" _long(const char8_t* str, std::size_t) {
  return BasicName<char8_t>(str, FlagType::long_type);
}

BasicName<char16_t> operator"" _short(const char16_t* str, std::size_t) {
  return BasicName<char16_t>(str, FlagType::short_type);
}

BasicName<char16_t> operator"" _long(const char16_t* str, std::size_t) {
  return BasicName<char16_t>(str, FlagType::long_type);
}

BasicName<char32_t> operator"" _short(const char32_t* str, std::size_t) {
  return BasicName<char32_t>(str, FlagType::short_type);
}

BasicName<char32_t> operator"" _long(const char32_t* str, std::size_t) {
  return BasicName<char32_t>(str, FlagType::long_type);
}

} // namespace Literals


namespace helper {

template <typename TChar>
struct ErrorStrings {
  static constexpr const char* arg_empty{"error: arg %pos: argument is empty."};

  static constexpr const char* surplus{"error: arg %pos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char* surplus_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char* proscribed{"error: arg %pos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char* proscribed_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char* unrecognized{"error: arg %pos: %type \u2018%opt\u2019: unrecognized %type."};

  static constexpr const char* empty_input{"error: arg %pos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char* empty_input_{
      "error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char* bare_empty_input{"error: arg %pos: %type \u2018%opt\u2019: %type does not expect an input."};

  static constexpr const char* bare_input{"error: arg %pos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char* bare_input_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char* missing_input{"error: arg %pos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char* missing_input_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char* stop_empty_input{"error: arg %pos: %type \u2018%opt\u2019: stop %type does not expect an input."};

  static constexpr const char* stop_input{"error: arg %pos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char* stop_input_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char* stopproscribed{"error: arg %pos: %type \u2018%opt\u2019: stop %opt may not be present."};

  static constexpr const char* stopproscribed_{"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %opt may not be present."};
};

template <>
struct ErrorStrings<char8_t> {
  static constexpr const char8_t* arg_empty{u8"error: arg %pos: argument is empty."};

  static constexpr const char8_t* surplus{u8"error: arg %pos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char8_t* surplus_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char8_t* proscribed{u8"error: arg %pos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char8_t* proscribed_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char8_t* unrecognized{u8"error: arg %pos: %type \u2018%opt\u2019: unrecognized %type."};

  static constexpr const char8_t* empty_input{u8"error: arg %pos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char8_t* empty_input_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char8_t* bare_empty_input{u8"error: arg %pos: %type \u2018%opt\u2019: %type does not expect an input."};

  static constexpr const char8_t* bare_input{u8"error: arg %pos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char8_t* bare_input_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char8_t* missing_input{u8"error: arg %pos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char8_t* missing_input_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char8_t* stop_empty_input{u8"error: arg %pos: %type \u2018%opt\u2019: stop %type does not expect an input."};

  static constexpr const char8_t* stop_input{u8"error: arg %pos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char8_t* stop_input_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char8_t* stopproscribed{u8"error: arg %pos: %type \u2018%opt\u2019: stop %opt may not be present."};

  static constexpr const char8_t* stopproscribed_{u8"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %opt may not be present."};
};

template <>
struct ErrorStrings<char16_t> {
  static constexpr const char16_t* arg_empty{u"error: arg %pos: argument is empty."};

  static constexpr const char16_t* surplus{u"error: arg %pos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char16_t* surplus_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char16_t* proscribed{u"error: arg %pos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char16_t* proscribed_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char16_t* unrecognized{u"error: arg %pos: %type \u2018%opt\u2019: unrecognized %type."};

  static constexpr const char16_t* empty_input{u"error: arg %pos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char16_t* empty_input_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char16_t* bare_empty_input{u"error: arg %pos: %type \u2018%opt\u2019: %type does not expect an input."};

  static constexpr const char16_t* bare_input{u"error: arg %pos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char16_t* bare_input_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char16_t* missing_input{u"error: arg %pos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char16_t* missing_input_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char16_t* stop_empty_input{u"error: arg %pos: %type \u2018%opt\u2019: stop %type does not expect an input."};

  static constexpr const char16_t* stop_input{u"error: arg %pos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char16_t* stop_input_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char16_t* stopproscribed{u"error: arg %pos: %type \u2018%opt\u2019: stop %opt may not be present."};

  static constexpr const char16_t* stopproscribed_{u"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %opt may not be present."};
};

template <>
struct ErrorStrings<char32_t> {
  static constexpr const char32_t* arg_empty{U"error: arg %pos: argument is empty."};

  static constexpr const char32_t* surplus{U"error: arg %pos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char32_t* surplus_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const char32_t* proscribed{U"error: arg %pos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char32_t* proscribed_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const char32_t* unrecognized{U"error: arg %pos: %type \u2018%opt\u2019: unrecognized %type."};

  static constexpr const char32_t* empty_input{U"error: arg %pos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char32_t* empty_input_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const char32_t* bare_empty_input{U"error: arg %pos: %type \u2018%opt\u2019: %type does not expect an input."};

  static constexpr const char32_t* bare_input{U"error: arg %pos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char32_t* bare_input_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const char32_t* missing_input{U"error: arg %pos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char32_t* missing_input_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const char32_t* stop_empty_input{U"error: arg %pos: %type \u2018%opt\u2019: stop %type does not expect an input."};

  static constexpr const char32_t* stop_input{U"error: arg %pos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char32_t* stop_input_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const char32_t* stopproscribed{U"error: arg %pos: %type \u2018%opt\u2019: stop %opt may not be present."};

  static constexpr const char32_t* stopproscribed_{U"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %opt may not be present."};
};

template <>
struct ErrorStrings<wchar_t> {
  static constexpr const wchar_t* arg_empty{L"error: arg %pos: argument is empty."};

  static constexpr const wchar_t* surplus{L"error: arg %pos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const wchar_t* surplus_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: exceeded presence limit of \u2018%input\u2019."};

  static constexpr const wchar_t* proscribed{L"error: arg %pos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const wchar_t* proscribed_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %opt may not be present."};

  static constexpr const wchar_t* unrecognized{L"error: arg %pos: %type \u2018%opt\u2019: unrecognized %type."};

  static constexpr const wchar_t* empty_input{L"error: arg %pos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const wchar_t* empty_input_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: empty input at pos \u2018%input\u2019."};

  static constexpr const wchar_t* bare_empty_input{L"error: arg %pos: %type \u2018%opt\u2019: %type does not expect an input."};

  static constexpr const wchar_t* bare_input{L"error: arg %pos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const wchar_t* bare_input_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: %type does not take an input: \u2018%input\u2019."};

  static constexpr const wchar_t* missing_input{L"error: arg %pos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const wchar_t* missing_input_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: expected input missing."};

  static constexpr const wchar_t* stop_empty_input{L"error: arg %pos: %type \u2018%opt\u2019: stop %type does not expect an input."};

  static constexpr const wchar_t* stop_input{L"error: arg %pos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const wchar_t* stop_input_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %type does not take an input: \u2018%input\u2019."};

  static constexpr const wchar_t* stopproscribed{L"error: arg %pos: %type \u2018%opt\u2019: stop %opt may not be present."};

  static constexpr const wchar_t* stopproscribed_{L"error: arg %pos, subpos %subpos: %type \u2018%opt\u2019: stop %opt may not be present."};
};

} // namespace helper



// BasicName

template <typename TChar>
BasicName<TChar>::BasicName(const Char* name, const FlagType type)
  : name_{ StringView{name} }, type_{type},
    type_explicit_{type != FlagType::unset ? true : false}
{ check_name(); }

template <typename TChar>
BasicName<TChar>::BasicName(const StringView name, const FlagType type)
  : name_{name}, type_{type},
    type_explicit_{type != FlagType::unset ? true : false}
{ check_name(); }

template <typename TChar>
template <typename T>
BasicName<TChar>::BasicName(T&& name, const FlagType type)
  : name_{ String{name} }, type_{type},
    type_explicit_{type != FlagType::unset ? true : false}
{ check_name(); }

template <typename TChar>
template <typename T>
BasicName<TChar>::BasicName(const T& name, const FlagType type)
  : name_{ StringView{name} }, type_{type},
    type_explicit_{type != FlagType::unset ? true : false}
{ check_name(); }

template <typename TChar>
void BasicName<TChar>::check_name() const {
  if (name().empty()) {
    throw FlagNameError("flag declaration error: flag name empty");
  }
}

template <typename TChar>
[[nodiscard]] FlagType BasicName<TChar>::type() const noexcept {
  return type_;
}

template <typename TChar>
[[nodiscard]] auto BasicName<TChar>::name() const noexcept -> StringView {
  return std::holds_alternative<String>(name_) ?
            std::get<String>(name_) : std::get<StringView>(name_);
}

template <typename TChar>
[[nodiscard]] auto BasicName<TChar>::length() const noexcept
      -> typename StringView::size_type {
  return name().length();
}

template <typename TChar>
void BasicName<TChar>::guess_type(
      const FlagMarkers& flag_markers, const bool tainted) {
  // if type has been explicitly set, then don't re-set them, even if
  // 'tainted' is set to true
  if (type_explicit_) {
    return;
  }

  if (!tainted && type_ != FlagType::unset) {
    return;
  }
  const auto len = length();
  const bool has_flag_marker = flag_markers.contains( name()[0] );
    // Note: unordered_set::contains is not noexcept

  if (has_flag_marker) {
    // if name length is 1, then interpret as a long flag
    if (len == 1) {
      type_ = FlagType::long_type;
    }

    // if name length is 2, then as long as the final char is not also the
    // flag marker that is the first char, then it is a short flag; otherwise
    // interpret the doubled flag marker as a long flag.
    else if (len == 2) {
      type_ = (name()[1] == name()[0])
                ? FlagType::long_type : FlagType::short_type;
    }

    // if name length is greater than 2, then always interpret as a long flag,
    // so, for example, "-flag" is a long flag.
    else {
      type_ = FlagType::long_type;
    }
  }
  else {
    // no flag marker, here if it is length-1, then interpret it as short,
    // otherwise as long
    type_ = (len == 1) ? FlagType::short_type : FlagType::long_type;
  }
}



// BasicArg

template <typename TChar>
BasicArg<TChar>::BasicArg(
      const StringView name, const CLType pos, const CLType subpos)
  : name_{name}, pos_{pos}, subpos_{subpos}
{}

template <typename TChar>
[[nodiscard]] auto BasicArg<TChar>::name() const noexcept -> StringView {
  return name_;
}

template <typename TChar>
[[nodiscard]] CLType BasicArg<TChar>::pos() const noexcept {
  return pos_;
}

template <typename TChar>
[[nodiscard]] CLType BasicArg<TChar>::subpos() const noexcept {
  return subpos_;
}



// BasicInput

template <typename TChar>
BasicInput<TChar>::BasicInput(
      const StringView value, const InputType type)
  : is_set_{true}, value_{value}, type_{type}
{}

template <typename TChar>
[[nodiscard]] bool BasicInput<TChar>::operator()() const noexcept {
  return is_set_;
}

template <typename TChar>
[[nodiscard]] auto BasicInput<TChar>::value() const noexcept -> StringView {
  return value_;
}

template <typename TChar>
[[nodiscard]] InputType BasicInput<TChar>::type() const noexcept {
  return type_;
}



// BasicInstance

template <typename TChar>
BasicInstance<TChar>::BasicInstance(const StringView name, const CLType pos)
  : name_{name}, pos_{pos}
{}

template <typename TChar>
BasicInstance<TChar>::BasicInstance(const StringView name, const CLType pos,
                                    const CLType subpos)
  : name_{name}, pos_{pos}, subpos_{subpos}
{}

template <typename TChar>
template <class... IArgs>
BasicInstance<TChar>::BasicInstance(const StringView name, const CLType pos,
  IArgs... input)
  : name_{name}, pos_{pos}, input_{input...}
{}

template <typename TChar>
template <class... IArgs>
BasicInstance<TChar>::BasicInstance(const StringView name, const CLType pos,
  const CLType subpos, IArgs... input)
  : name_{name}, pos_{pos}, subpos_{subpos}, input_{input...}
{}

template <typename TChar>
[[nodiscard]] auto BasicInstance<TChar>::name() const noexcept -> StringView {
  return name_;
}

template <typename TChar>
[[nodiscard]] CLType BasicInstance<TChar>::pos() const noexcept {
  return pos_;
}

template <typename TChar>
[[nodiscard]] CLType BasicInstance<TChar>::subpos() const noexcept {
  return subpos_;
}

template <typename TChar>
[[nodiscard]] auto BasicInstance<TChar>::input() const noexcept
      -> const Input& {
  return input_;
}



// BasicFlag

template <typename TChar>
template <class... N, typename>
BasicFlag<TChar>::BasicFlag(const FlagClass flag_class, N&&... names)
  : class_{flag_class}, names_{std::forward<N>(names)...}
{}

template <typename TChar>
template <PosType C, class... N, typename>
BasicFlag<TChar>::BasicFlag(const FlagClass flag_class,
                              const C max, N&&... names)
  : class_{flag_class}, names_{std::forward<N>(names)...},
    max_{max < 0 ? 0 : static_cast<CLType>(max)}
{}

template <typename TChar>
void BasicFlag<TChar>::guess_types(
      const FlagMarkers& flag_markers, const bool tainted) {
  // pass on to each flag name
  for (auto& i: names_) {
    i.guess_type(flag_markers, tainted);
  }
}

template <typename TChar>
[[nodiscard]] FlagClass BasicFlag<TChar>::flag_class() const noexcept {
  return class_;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::names() const noexcept -> const Names& {
  return names_;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::names() noexcept -> Names& {
  return names_;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::canonical_name() const noexcept
      -> StringView {
  return names_.front().name();
}

template <typename TChar>
[[nodiscard]] CLType BasicFlag<TChar>::max() const noexcept {
  return max_;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::within_limit() const noexcept
      -> LimitType {
  if (max_ == 0) {
    return LimitType::Proscribed;
  }
  // next line is correct: instance will yet to be increased when this
  // check is called, so as long as it is strictly less than max _now_,
  // then we are within the limit
  return instances_.size() < max_ ? LimitType::Within : LimitType::Without;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::instances() const noexcept
      -> const Instances& {
  return instances_;
}

template <typename TChar>
[[nodiscard]] auto BasicFlag<TChar>::instances() noexcept -> Instances& {
  return instances_;
}

template <typename TChar>
void BasicFlag<TChar>::clear() noexcept {
  instances_.clear();
}

template <typename TChar>
void BasicFlag<TChar>::add_instance(
      const StringView name, const CLType pos, const CLType subpos,
      const StringView input, const InputType type) {
  if (subpos > 0) {
    // if subpos is set
    instances_.emplace_back(name, pos, subpos, input, type);
  }
  else {
    instances_.emplace_back(name, pos, input, type);
  }
}

template <typename TChar>
void BasicFlag<TChar>::add_instance(
      const StringView name, const CLType pos, const CLType subpos) {
  if (subpos > 0) {
    instances_.emplace_back(name, pos, subpos);
  }
  else {
    instances_.emplace_back(name, pos);
  }
}



// BasicOpts::Parsing

template <typename TChar>
void BasicOpts<TChar>::Parsing::clear() noexcept {
  // preserve stop state, so don't reset 'stop'
  pflag = nullptr;
  pname = nullptr;
  subpos = 0;
}

template <typename TChar>
void BasicOpts<TChar>::Parsing::reset_all() noexcept {
  clear();
  stop = false;
}

// return true if 'set' and false if clear.
template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::Parsing::operator()() const noexcept {
  return pflag != nullptr;
}


// BasicOpts::ErrorInfo

// error: pos P: ...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(const ErrorKey k, const CLType p)
  : key{k}, pos{p}
{}

// error: pos P: flag 'O': ... 'I'...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const StringView o, const bool f,
      const CLType i)
  : key{k}, pos{p}, have_opt{true}, opt{o}, is_flag{f},
    have_input{true}, input{i}
{}

// error: pos P: flag 'O': ... 'I'...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const StringView o, const bool f,
      const StringView i)
  : key{k}, pos{p}, have_opt{true}, opt{o}, is_flag{f},
    have_input{true}, input{i}
{}

// error: pos P: flag 'O': ...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const StringView o, const bool f)
  : key{k}, pos{p}, have_opt{true}, opt{o}, is_flag{f}
{}

// error: pos P: subpos SP: flag 'O': ... 'I'...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const CLType sp, const StringView o,
      const bool f, const CLType i)
  : key{k}, pos{p}, subpos{sp}, have_opt{true}, opt{o}, is_flag{f},
    have_input{true}, input{i}
{}

// error: pos P, subpos SP: flag 'O': ... 'I'...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const CLType sp, const StringView o,
      const bool f, const StringView i)
  : key{k}, pos{p}, subpos{sp}, have_opt{true}, opt{o}, is_flag{f},
    have_input{true}, input{i}
{}

// error: pos P, subpos SP: flag 'O': ...
template <typename TChar>
BasicOpts<TChar>::ErrorInfo::ErrorInfo(
      const ErrorKey k, const CLType p, const CLType sp, const StringView o,
      const bool f)
  : key{k}, pos{p}, subpos{sp}, have_opt{true}, opt{o}, is_flag{f}
{}


// BasicOpts

template <typename TChar>
void BasicOpts<TChar>::convert_error_str(
      String& str, const ErrorInfo& error) const {
  typename String::size_type i = 0;
  while (i = str.find(static_cast<Char>('%'), i),
    i < str.size() || i != String::npos) {
    if (str[i + 1] == static_cast<Char>('%')) {
      str.erase(i, 1);
    }
    else if (str.substr(i, 4) == String{ '%','o', 'p', 't' }) {
      if (error.have_opt) {
        str.replace(i, 4, error.opt);
      }
    }
    else if (str.substr(i, 5) == String{ '%', 't', 'y', 'p', 'e' }) {
      if (error.have_opt) {
        if (error.is_flag) {
          str.replace(i, 5, String{ 'f', 'l', 'a', 'g' });
        }
        else {
          str.replace(i, 5, String{ 'a', 'r', 'g' });
        }
      }
    }
    else if (str.substr(i, 4) == String{ '%', 'p', 'o', 's' }) {
      str.replace(i, 4, to_string<Char>(error.pos));
    }
    else if (str.substr(i, 7) == String{ '%', 's', 'u', 'b', 'p', 'o', 's' }) {
      if (error.subpos > 0) {
        str.replace(i, 7, to_string<Char>(error.subpos));
      }
    }
    else if (str.substr(i, 6) == String{ '%', 'i', 'n', 'p', 'u', 't' }) {
      if (error.have_input) {
        if (std::holds_alternative<CLType>(error.input)) {
          str.replace(i, 6, to_string<Char>(std::get<CLType>(error.input)));
        }
        else {
          str.replace(i, 6, std::get<StringView>(error.input));
        }
      }
    }
    else if (str.substr(i, 6) == String{ '%', 'e', 'r', 'r', 'n', 'o' }) {
      str.replace(i, 6, to_string<Char>(static_cast<int>(error.key)));
    }
    ++i;
  }
}

template <typename TChar>
template <class... N>
void BasicOpts<TChar>::add_bare(N&&... names) {
  flags_.emplace_back(FlagClass::bare, std::forward<N>(names)...);
  map_tainted_ = true;
}

template <typename TChar>
template <class... N>
void BasicOpts<TChar>::add_optional(N&&... names) {
  flags_.emplace_back(FlagClass::optional, std::forward<N>(names)...);
  map_tainted_ = true;
}

template <typename TChar>
template <class... N>
void BasicOpts<TChar>::add_mandatory(N&&... names) {
  flags_.emplace_back(FlagClass::mandatory, std::forward<N>(names)...);
  map_tainted_ = true;
}

template <typename TChar>
template <class... N>
void BasicOpts<TChar>::add_stop(N&&... names) {
  flags_.emplace_back(FlagClass::stop, std::forward<N>(names)...);
  map_tainted_ = true;
}

template <typename TChar>
void BasicOpts<TChar>::allow_empty_arguments(const bool state) noexcept {
  allow_empty_arg_ = state;
}

template <typename TChar>
void BasicOpts<TChar>::allow_empty_inputs(const bool state) noexcept {
  allow_empty_input_ = state;
}

template <typename TChar>
template <PosType T>
void BasicOpts<TChar>::allow_arguments(const T max) noexcept {
  collect_args_ = max < 0 ? 0 : max;
}

template <typename TChar>
void BasicOpts<TChar>::allow_arguments() noexcept {
  collect_args_ = std::numeric_limits<CLType>::max();
}

template <typename TChar>
template <PosType T>
void BasicOpts<TChar>::allow_unrecognized_flags(
      const T max, const Aggressive state) noexcept {
  collect_unrecognized_flags_ = max < 0 ? 0 : max;
  unrecognized_greedy_ = state == Aggressive::no ? false : true;
}

template <typename TChar>
void BasicOpts<TChar>::allow_unrecognized_flags(
      const Aggressive state) noexcept {
  collect_unrecognized_flags_ = std::numeric_limits<CLType>::max();
  unrecognized_greedy_ = state == Aggressive::no ? false : true;
}

template <typename TChar>
template <PosType T>
void BasicOpts<TChar>::allow_unrecognized_opts(const T max) noexcept {
  allow_unrecognized_flags(max);
}

template <typename TChar>
void BasicOpts<TChar>::allow_unrecognized_opts() noexcept {
  allow_unrecognized_flags();
}

template <typename TChar>
void BasicOpts<TChar>::allow_chaining(const bool state) noexcept {
  can_chain_ = state;
}

template <typename TChar>
void BasicOpts<TChar>::set_chaining(const bool state) noexcept {
  allow_chaining(state);
}

template <typename TChar>
void BasicOpts<TChar>::set_no_chaining() noexcept {
  allow_chaining(false);
}

template <typename TChar>
void BasicOpts<TChar>::set_greedy(const bool state) noexcept {
  mandatory_greedy_ = state ? Greedy::yes : Greedy::no;
}

template <typename TChar>
void BasicOpts<TChar>::set_not_greedy() noexcept {
  mandatory_greedy_ = Greedy::no;
}

template <typename TChar>
void BasicOpts<TChar>::set_lax_greedy() noexcept {
  mandatory_greedy_ = Greedy::lax;
}

template <typename TChar>
void BasicOpts<TChar>::set_greedy(const Greedy state) noexcept {
  mandatory_greedy_ = state;
}

template <typename TChar>
void BasicOpts<TChar>::set_optional_greedy(const bool state) noexcept {
  optional_greedy_ = state;
}

template <typename TChar>
void BasicOpts<TChar>::set_optional_not_greedy() noexcept {
  optional_greedy_ = false;
}

template <typename TChar>
template <typename Str>
void BasicOpts<TChar>::set_preamble(Str&& str) {
  preamble_ = std::forward<Str>(str);
}

template <typename TChar>
template <typename Str>
void BasicOpts<TChar>::set_postscript(Str&& str) {
  postscript_ = std::forward<Str>(str);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::set_flag_marker(Chars... flag_markers) {
  set_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::set_flag_markers(Chars... flag_markers) {
  flag_markers_.clear();
  add_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::add_flag_marker(Chars... flag_markers) {
  add_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::add_flag_markers(Chars... flag_markers) {
  (flag_markers_.emplace(flag_markers), ...);
  flag_type_tainted_ = true;
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::set_opt_marker(Chars... flag_markers) {
  set_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::set_opt_markers(Chars... flag_markers) {
  set_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::add_opt_marker(Chars... flag_markers) {
  add_flag_markers(flag_markers...);
}

template <typename TChar>
template <class... Chars, typename>
void BasicOpts<TChar>::add_opt_markers(Chars... flag_markers) {
  add_flag_markers(flag_markers...);
}

template <typename TChar>
void BasicOpts<TChar>::set_input_marker(const Char input_marker) noexcept {
  input_marker_ = input_marker;
}

template <typename TChar>
void BasicOpts<TChar>::clear() noexcept {
  // clear saved opts
  cl_opts_.clear();

  // clear arguments
  args_.clear();

  // clear unrecognized flags
  unrecognized_flags_.clear();

  // clear errors
  clear_errors();

  // clear flag instances
  for (auto& flag: flags_) {
    flag.clear();
  }
}

template <typename TChar>
void BasicOpts<TChar>::clear_errors() noexcept {
  errors_.clear();
  cached_error_strings_.clear();
}

template <typename TChar>
void BasicOpts<TChar>::clear_declarations() noexcept {
  flags_.clear();
  flag_type_tainted_ = true;
  map_.clear();
  map_tainted_ = true;
  collect_unrecognized_flags_ = 0;
  collect_unrecognized_flags_count_ = 0;
  collect_args_ = 0;
  collect_args_count_ = 0;
}

template <typename TChar>
void BasicOpts<TChar>::default_error_formats() noexcept {
  custom_error_message_.clear();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_errors() const noexcept
      -> const Errors& {
  return errors_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_errors() noexcept -> Errors& {
  return errors_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_map() -> const Map& {
  create_map();
  return map_;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::have_opt(const StringView name) const {
  return !std::get<1>( map_.at(name) )->instances().empty();
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::has_opt(const StringView name) const {
  return have_opt(name);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::have_flag(const StringView name) const {
  return have_opt(name);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::has_flag(const StringView name) const {
  return have_opt(name);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::has_input(
      const StringView name, const InstancePos pos) const {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return has_input(instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::has_input(
      const Instance& instance) const noexcept {
  return instance.input()();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_instances(const StringView name)
      -> Instances& {
  return const_cast<Instances&>(std::as_const(*this).get_all_instances(name));
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_instances(
      const StringView name) const -> const Instances& {
  return std::get<1>( map_.at(name) )->instances();
}

// pos can be 0, ..., N-1 to get instance k from N total, or else negative
// fetching retrograde from -1, ..., -N.
// return will be: <flag_has_input?, input>
template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_input(
      const StringView name, const InstancePos pos) const -> InputResult {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return get_input(instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_input(
      const Instance& instance) const noexcept -> InputResult {
  const auto& input = instance.input();
  return {input(), input.value()};
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::is_input_internal(
      const StringView name, const InstancePos pos) const {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return is_input_internal(
            instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::is_input_external(
      const StringView name, const InstancePos pos) const {
  const auto& instances = std::get<1>(map_.at(name))->instances();
  return is_input_external(
            instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::is_input_internal(
      const Instance& instance) const noexcept {
  return instance.input().type() == InputType::internal;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::is_input_external(
      const Instance& instance) const noexcept {
  return instance.input().type() == InputType::external;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_name(
      const StringView name, const InstancePos pos) const -> StringView {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return get_name(instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_name(
      const Instance& instance) const noexcept -> StringView {
  return instance.name();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_opt_name(
      const StringView name, const InstancePos pos) const -> StringView {
  return get_name(name, pos);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_opt_name(
      const Instance& instance) const noexcept -> StringView {
  return get_name(instance);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_flag_name(
  const StringView name, const InstancePos pos) const -> StringView {
  return get_name(name, pos);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_flag_name(
      const Instance& instance) const noexcept -> StringView {
  return get_name(instance);
}

template <typename TChar>
[[nodiscard]] CLType BasicOpts<TChar>::get_pos(
      const StringView name, const InstancePos pos) const {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return get_pos(instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] CLType BasicOpts<TChar>::get_pos(
      const Instance& instance) const noexcept {
  return instance.pos();
}

template <typename TChar>
[[nodiscard]] CLType BasicOpts<TChar>::get_subpos(
      const StringView name, const InstancePos pos) const {
  const auto& instances = std::get<1>( map_.at(name) )->instances();
  return get_subpos(instances.at(pos < 0 ? instances.size() + pos : pos));
}

template <typename TChar>
[[nodiscard]] CLType BasicOpts<TChar>::get_subpos(
      const Instance& instance) const noexcept {
  return instance.subpos();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_unrecognized_flags() noexcept
      -> Unrecognized& {
  return unrecognized_flags_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_unrecognized_flags() const noexcept
      -> const Unrecognized& {
  return unrecognized_flags_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_unrecognized_opts() noexcept
      -> Unrecognized& {
  return get_all_unrecognized_flags();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_unrecognized_opts() const noexcept
      -> const Unrecognized& {
  return get_all_unrecognized_flags();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_unrecognized_flag(
      const UnrecognizedPos pos) -> Arg& {
  return const_cast<Arg&>(std::as_const(*this).get_unrecognized_flag(pos));
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_unrecognized_flag(
      const UnrecognizedPos pos) const -> const Arg& {
  return unrecognized_flags_.at(
      pos < 0 ? unrecognized_flags_.size() + pos : pos );
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_unrecognized_opt(
      const UnrecognizedPos pos) -> Arg& {
  return get_unrecognized_flag(pos);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_unrecognized_opt(
      const UnrecognizedPos pos) const -> const Arg& {
  return get_unrecognized_flag(pos);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_arguments() noexcept -> Args& {
  return args_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_all_arguments() const noexcept
      -> const Args& {
  return args_;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_argument(const ArgPos pos) -> Arg& {
  return const_cast<Arg&>(std::as_const(*this).get_argument(pos));
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_argument(const ArgPos pos) const
      -> const Arg& {
  return args_.at( pos < 0 ? args_.size() + pos : pos );
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_preamble() const -> StringView {
  return StringView{preamble_};
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_postscript() const -> StringView {
  return StringView{postscript_};
}

template <typename TChar>
template <typename T>
bool BasicOpts<TChar>::parse(T&& args) {
  return parse(std::forward<T>(args), static_cast<CLType>(0));
}

template <typename TChar>
template <PosType T>
bool BasicOpts<TChar>::parse(T argc, Char** argv, T start_at) {
  guess_types();
  create_map();
  clear();

  if (argc < 1 || argv == nullptr || start_at >= argc) {
    return false;
  }
  if (start_at < 0) {
    start_at = argc + start_at;
  }
  if (start_at < 0) {
    return false;
  }

  // set to true if any parsing errors found
  bool errors = false;

  // keep track of prev parsing details on loop through argv
  Parsing prev;

  // loop over opts
  // let's always start 'pos' at 1 so that counting (and referencing) opts
  // at the command-line begins at 1 and not 0
  for (CLType pos = 1; pos < 1 + static_cast<CLType>(argc) - start_at; ++pos) {
    parse_opt(pos, argv[pos - 1 + start_at], prev, errors);
  }

  // if prev set, then last mandatory flag was expecting an input
  parse_opt_finish(static_cast<CLType>(argc) - start_at, prev, errors);

  return errors;
}

template <typename TChar>
template <typename T, PosType U>
bool BasicOpts<TChar>::parse(T&& argv, U start_at) {
  const auto argc = std::size(argv);
  guess_types();
  create_map();
  clear();

  if (argc == 0 || start_at >= argc) {
    return false;
  }
  if (start_at < 0) {
    start_at = argc + start_at;
  }
  if (start_at < 0) {
    return false;
  }

  // set to true if any parsing errors found
  bool errors = false;

  // keep track of prev parsing details on loop through argv
  Parsing prev;

  // save opts, if need be
  const bool saved = save_opts(std::forward<T>(argv));

  // loop over opts
  // let's always start 'pos' at 1 so that counting (and referencing) opts
  // at the command-line begins at 1 and not 0
  for (CLType pos = 1; pos < 1 + argc - start_at; ++pos) {
    if (saved) {
      parse_opt(pos, cl_opts_[pos - 1], prev, errors);
    }
    else {
      parse_opt(pos, argv[pos - 1 + start_at], prev, errors);
    }
  }

  // if prev set, then last mandatory flag was expecting an input
  parse_opt_finish(argc - start_at, prev, errors);

  return errors;
}

template <typename TChar>
template <class... T, typename>
bool BasicOpts<TChar>::parse(T&&... args) {
  guess_types();
  create_map();
  clear();

  // set to true if any parsing errors found
  bool errors = false;

  // keep track of prev parsing details on loop through argv
  Parsing prev;

  // save opts
  save_opts_pack(std::forward<T>(args)...);

  // loop over opts
  for (CLType pos = 1; pos < 1 + cl_opts_.size(); ++pos) {
    parse_opt(pos, cl_opts_[pos - 1], prev, errors);
  }

  // if prev set, then last mandatory flag was expecting an input
  parse_opt_finish(cl_opts_.size(), prev, errors);

  return errors;
}

template <typename TChar>
bool BasicOpts<TChar>::parse() {
  guess_types();
  create_map();
  clear();
  return false;
}

template <typename TChar>
template <typename Str>
void BasicOpts<TChar>::format_error(const ErrorKey key, Str&& str) {
  custom_error_message_[key] = std::forward<Str>(str);
}

template <typename TChar>
template <typename T>
T& BasicOpts<TChar>::write_errors(T& out) {
  // first, cache the errors
  cache_errors();

  // then, write the strings in cached_error_strings_
  for (const auto& error: cached_error_strings_) {
    out << std::get<1>(error) << static_cast<Char>('\n');
  }

  return out;
}

template <typename TChar>
template <typename T, typename U>
T& BasicOpts<TChar>::write_error(T& out, const U pos) {
  cache_errors();
  out << std::get<1>(cached_error_strings_.at(
                          pos < 0 ? cached_error_strings_.size() + pos : pos))
      << static_cast<Char>('\n');
  return out;
}

template <typename TChar>
template <typename T, class... U>
T& BasicOpts<TChar>::write_errors_with(T& out, const U... keys) {
  cache_errors();
  const std::unordered_set<ErrorKey> set{keys...};
  for (const auto& error: cached_error_strings_) {
    if (set.contains(std::get<0>(error).key)) {
      out << std::get<1>(error) << static_cast<Char>('\n');
    }
  }
  return out;
}

template <typename TChar>
void BasicOpts<TChar>::cache_errors() {
  // we will construct error strings (in cached_error_strings_) from the
  // registered errors (in errors_)

  for (auto& error: errors_) {
    // take an ErrorInfo object (in errors_) and convert it to a string
    String error_str = create_error_string(error);
    // store this string in cached_error_strings_:
    cached_error_strings_.emplace_back(std::move(error), std::move(error_str));
  }

  // since the registered errors have now been converted into strings
  // and inserted into cached_error_strings_, then clear errors_:
  errors_.clear();
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::create_error_string(
      const ErrorInfo& error) const -> String {
  // first, get the error string prototype
  String proto = preamble_ + get_error_string_prototype(error.key)
                           + postscript_;

  // next, convert the prototype string into the final error string
  convert_error_str(proto, error);

  return proto;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_error_string_prototype(
      const ErrorKey key) const -> String {
  if (custom_error_message_.contains(key)) {
    return custom_error_message_.at(key);
  }
  else {
    return get_error_string(key);
  }
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::get_error_string(const ErrorKey key) const
      -> String {
  switch(key) {
  case ErrorKey::ArgEmpty:
    return helper::ErrorStrings<Char>::arg_empty;
  case ErrorKey::BareEmptyInput:
    return helper::ErrorStrings<Char>::bare_empty_input;
  case ErrorKey::BareInput:
    return helper::ErrorStrings<Char>::bare_input;
  case ErrorKey::BareInput_:
    return helper::ErrorStrings<Char>::bare_input_;
  case ErrorKey::EmptyInput:
    return helper::ErrorStrings<Char>::empty_input;
  case ErrorKey::EmptyInput_:
    return helper::ErrorStrings<Char>::empty_input_;
  case ErrorKey::MissingInput:
    return helper::ErrorStrings<Char>::missing_input;
  case ErrorKey::MissingInput_:
    return helper::ErrorStrings<Char>::missing_input_;
  case ErrorKey::StopEmptyInput:
    return helper::ErrorStrings<Char>::stop_empty_input;
  case ErrorKey::StopInput:
    return helper::ErrorStrings<Char>::stop_input;
  case ErrorKey::StopInput_:
    return helper::ErrorStrings<Char>::stop_input_;
  case ErrorKey::Surplus:
    return helper::ErrorStrings<Char>::surplus;
  case ErrorKey::Surplus_:
    return helper::ErrorStrings<Char>::surplus_;
  case ErrorKey::Proscribed:
    return helper::ErrorStrings<Char>::proscribed;
  case ErrorKey::Proscribed_:
    return helper::ErrorStrings<Char>::proscribed_;
  case ErrorKey::StopProscribed:
    return helper::ErrorStrings<Char>::stopproscribed;
  case ErrorKey::StopProscribed_:
    return helper::ErrorStrings<Char>::stopproscribed_;
  case ErrorKey::Unrecognized:
    return helper::ErrorStrings<Char>::unrecognized;
  default:
    throw InternalError{"internal error: unhandled ErrorKey case"};
  }
}

template <typename TChar>
void BasicOpts<TChar>::guess_types() {
  // pass down to each flag to handle
  for (auto& i: flags_) {
    i.guess_types(flag_markers_, flag_type_tainted_);
  }
  flag_type_tainted_ = false;
}

template <typename TChar>
void BasicOpts<TChar>::create_map() {
  if ( !map_tainted_ ) {
    return;
  }
  for (auto& flag: flags_) {
    for (auto& name : flag.names()) {
      auto& m = map_[name.name()];
      // if m == {nullptr, nullptr}, then it is a new entry; add it!
      if (std::get<0>(m) == nullptr && std::get<1>(m) == nullptr) {
        m = { &name, &flag };
      }
      // otherwise, if the entries exactly match the current values,
      // then the mapping is already present and that is ok, otherwise
      // it is an error
      else if (std::get<0>(m) != &name || std::get<1>(m) != &flag) {
        throw NameConflictError(
          "name conflict error: duplicate flags found");
      }
    }
  }
  map_tainted_ = false;
}

template <typename TChar>
template <typename... T>
void BasicOpts<TChar>::register_error(const ErrorKey key, T&&... data) {
  errors_.emplace_back(key, std::forward<T>(data)...);
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::search_flag_all(
      const StringView opt, const bool short_only) const -> SearchAll {
  SearchAll all;
  for (const auto& m: map_) {
    if ( !opt.starts_with( m.first ) ) {
      continue;
    }

    // if opt is long and we are only accepting short opts, then skip it
    if (short_only && std::get<0>(m.second)->type() == FlagType::long_type) {
      continue;
    }

    // if no trailing characters, then we have found an opt:
    if ( opt.size() == m.first.size() ) {
      all.emplace_back( m.second );
      continue;
    } 

    // start of 'opt' matches an opt name, but we need to examine the
    // trailing characters.
    if ( std::get<0>(m.second)->type() == FlagType::short_type ) {
      // we _could_ check if we're chaining and exclude a bare flag
      // that has trailing input if we're not chaining here, but
      // we'll let the later code handle that scenario so that we
      // don't have to duplicate errors; for now, just return
      // the opt that was found
      all.emplace_back(m.second);
      continue;

      // // if we can chain, then no trailing characters will ever invalidate
      // // the opt, so we can return ok now
      // if (can_chain_) { 
      //   all.emplace_back( m.second );
      //   continue;
      // }

      // // if we cannot chain, then only trailing characters behind non-
      // // bare opts are allowed.
      // if ( std::get<1>(m.second)->flag_class() != FlagClass::bare ) {
      //   all.emplace_back( m.second );
      //   continue;
      // }
    }
    else {
      // for a long opt, the trailing character must be '=', and for this
      // method, that is as far as it need go
      if ( opt[ m.first.size() ] == input_marker_ ) {
        all.emplace_back( m.second );
        continue;
      }
    }
  }
  return all;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::search_flag(
      const StringView opt, const bool stop, const bool short_only) const
      -> SearchRes {
  if (stop) {
    return {nullptr, nullptr};
  }

  const SearchAll all = search_flag_all(opt, short_only);

  SearchRes ret = {nullptr, nullptr};
  typename String::size_type size = 0;

  for (const auto& i: all) {
    // Note: std::get<0>(i) is a NamePtr (pointer to BasicName) object
    if ( std::get<0>(i)->length() > size ) {
      ret = i;
      size = std::get<0>(i)->length();
    }
  }

  return ret;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::opt_is_flag(const StringView opt) const {
  if ( opt.empty() ) {
    return false;
  }
  return flag_markers_.contains(opt[0]);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::opt_is_arg(const StringView opt) const {
  return !opt_is_flag(opt);
}

template <typename TChar>
void BasicOpts<TChar>::add_unrecognized_flag(
      const StringView flag, const CLType pos) {
  unrecognized_flags_.emplace_back(flag, pos);
  ++collect_unrecognized_flags_count_;
}

template <typename TChar>
void BasicOpts<TChar>::add_argument(const StringView arg, const CLType pos) {
  args_.emplace_back(arg, pos);
  ++collect_args_count_;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::handle_unrecognized(
      const StringView opt, const CLType pos, const bool stop) {
  // if it's a flag:
  if ( opt_is_flag(opt) && !stop ) {
    if ( are_collecting_unrecognized_flags() ) {
      if ( unrecognized_flags_within_limit() ) {
        // add flag to list
        add_unrecognized_flag(opt, pos);
        return false;
      }
      else {
        // show error: over max limit
        register_error(ErrorKey::Surplus, pos, opt, true,
                       collect_unrecognized_flags_);
        return true;
      }
    }
    else {
      // show error message: unrecognized flag
      register_error(ErrorKey::Unrecognized, pos, opt, true);
      return true;
    }
  }
  // ...or is an arg:
  else {
    // check if is empty and we disallow this
    if ( opt.empty() && !allow_empty_arg_ ) {
      register_error(ErrorKey::ArgEmpty, pos);
      return true;
    }

    if ( are_collecting_args() ) {
      if ( args_within_limit() ) {
        // add input to list
        add_argument(opt, pos);
        return false;
      }
      else {
        // show error: over max limit
        register_error(ErrorKey::Surplus, pos, opt, false, collect_args_);
        return true;
      }
    }
    else {
      // show error: not accepting args
      register_error(ErrorKey::Unrecognized, pos, opt, false);
      return true;
    }
  }
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::check_within_limit(
      const FlagPtr pflag, const NamePtr pname, const CLType pos,
      const bool is_flag) {
  const LimitType limit = pflag->within_limit();
  if ( limit == LimitType::Without ) {
    register_error(ErrorKey::Surplus, pos, pname->name(), is_flag,
                   pflag->max());
    return true;
  }
  if (limit == LimitType::Proscribed) {
    register_error(pflag->flag_class() == FlagClass::stop
                            ? ErrorKey::StopProscribed : ErrorKey::Proscribed,
                   pos, pname->name(), is_flag);
    return true;
  }
  return false;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::check_within_limit(
      const Parsing& prev, const CLType pos, const bool is_flag,
      const bool check_subpos) {
  const LimitType limit = prev.pflag->within_limit();
  if ( limit == LimitType::Within ) {
    return false;
  }

  if (check_subpos) {
    if ( prev.subpos > 1 ) {
      if (limit == LimitType::Proscribed) {
        register_error(prev.pflag->flag_class() == FlagClass::stop
                          ? ErrorKey::StopProscribed_ : ErrorKey::Proscribed_,
                       pos, prev.subpos, prev.pname->name(), is_flag);
      }
      else {
        // stop flags can only ever be called once, so no need to
        // check for stop for surplus input errors as we did above
        register_error(ErrorKey::Surplus_, pos, prev.subpos,
          prev.pname->name(), is_flag, prev.pflag->max());
      }
      return true;
    }
    else {
      return check_within_limit(prev.pflag, prev.pname, pos, is_flag);
    }
  }
  else {
    if (limit == LimitType::Proscribed) {
      register_error(prev.pflag->flag_class() == FlagClass::stop
                        ? ErrorKey::StopProscribed_ : ErrorKey::Proscribed_,
                     pos, prev.subpos, prev.pname->name(), is_flag);
    }
    else {
      // stop flags can only ever be called once, so no need to check
      // for stop for surplus input errors as we did above
      register_error(ErrorKey::Surplus_, pos, prev.subpos,
                     prev.pname->name(), is_flag, prev.pflag->max());
    }
    return true;
  }
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::add_instance(
      const Parsing& prev, const CLType pos, const StringView input) {
  // if 'input' is empty, then print error if empty inputs not allowed
  if ( input.empty() && !allow_empty_input_ ) {
    if (prev.subpos > 1) {
      register_error(ErrorKey::EmptyInput_, pos - 1, prev.subpos,
                     prev.pname->name(), opt_is_flag(prev.pname->name()), pos);
    }
    else {
      register_error(ErrorKey::EmptyInput, pos - 1, prev.pname->name(),
                     opt_is_flag(prev.pname->name()), pos);
    }
    return true;
  }
  prev.pflag->add_instance(prev.pname->name(), pos-1, prev.subpos,
                           input, InputType::external);
  return false;
}

template <typename TChar>
void BasicOpts<TChar>::add_instance(const Parsing& prev, const CLType pos) {
  prev.pflag->add_instance(prev.pname->name(), pos-1, prev.subpos);
}

template <typename TChar>
void BasicOpts<TChar>::add_instance(
      const FlagPtr pflag, const NamePtr pname, const CLType pos,
      const CLType subpos) {
  pflag->add_instance(pname->name(), pos, subpos);
}

template <typename TChar>
void BasicOpts<TChar>::add_instance(
      const FlagPtr pflag, const NamePtr pname, const CLType pos) {
  pflag->add_instance(pname->name(), pos, 0);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::add_instance(
      const FlagPtr pflag, const NamePtr pname, const CLType pos,
      const CLType subpos, const StringView input, const InputType type) {
  if ( input.empty() ) {
    if ( allow_empty_input_ ) {
      pflag->add_instance(pname->name(), pos, subpos, input, type);
    }
    else {
      // this will never trip with subpos > 0
      register_error(ErrorKey::MissingInput, pos, pname->name(),
                     opt_is_flag(pname->name()));
      return true;
    }
  }
  else {
    pflag->add_instance(pname->name(), pos, subpos, input, type);
  }
  return false;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::add_instance(
      const FlagPtr pflag, const NamePtr pname, const CLType pos,
      const StringView input, const InputType type) {
  return add_instance(pflag, pname, pos, 0, input, type);
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::are_collecting_args() const noexcept {
  return collect_args_ > 0;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::args_within_limit() const noexcept {
  return collect_args_count_ < collect_args_;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::are_collecting_unrecognized_flags()
      const noexcept {
  return collect_unrecognized_flags_ > 0;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::unrecognized_flags_within_limit()
      const noexcept {
  return collect_unrecognized_flags_count_ < collect_unrecognized_flags_;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::optional_can_take_no_input()
      const noexcept {
  //           |     no      .    lax      .   yes       .  opt. greedy
  //     ------|-------------.-------------.-------------.--------------
  //     -o -o | no input    . no input    . no input    . input: -o
  //     -o -A | no input    . input: -A   . input: -A   . input: -A
  //     -o A  | input: A    . input: A    . input: A    . input: A
  // usually, whether to take an input depends on what the next
  // opt is, but there are exceptions when we will never take
  // an input (these correspond to situations where the COLUMNS are all
  // "no input"), which are:
  //  1) Greedy::no and allow arguments
  //  2) !Greedy::no and not optional greedy and allow arguments AND
  //     allow unrec. flags
  if (mandatory_greedy_ == Greedy::no) {
    return are_collecting_args();
  }
  else {
    return !optional_greedy_ && (are_collecting_args()
                             && are_collecting_unrecognized_flags());
  }
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::mandatory_and_greedy(
      const Parsing& prev) const noexcept {
  return prev.pflag->flag_class() == FlagClass::mandatory
            && mandatory_greedy_ == Greedy::yes;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::optional_and_greedy(
      const Parsing& prev) const noexcept {
  return prev.pflag->flag_class() == FlagClass::optional && optional_greedy_;
}

template <typename TChar>
template <typename T>
[[nodiscard]] bool BasicOpts<TChar>::save_opts(T&) {
  // do nothing; do not save opts
  return false;
}

template <typename TChar>
template <typename T>
[[nodiscard]] bool BasicOpts<TChar>::save_opts(T&& argv) {
  cl_opts_.reserve(std::size(argv));
  for (auto& i : argv) {
    cl_opts_.emplace_back(std::move(i));
  }
  return true;
}

template <typename TChar>
template <class... T>
void BasicOpts<TChar>::save_opts_pack(T&&... args) {
  cl_opts_ = {std::forward<T>(args)...};
}

template <typename TChar>
void BasicOpts<TChar>::parse_opt(
      const CLType pos, const StringView opt, Parsing& prev, bool& errors) {

  // take opt as an input to the previous opt if...
  // prev(): true if previous opt expects an input
  // if we are greedy, then take this opt unconditionally now
  if (prev() && (mandatory_and_greedy(prev) || optional_and_greedy(prev))) {
    if (add_instance(prev, pos, opt)) {
      errors = true;
    }
    prev.clear();
    return;
  }

  // loop over flags and match start of arg with flag name.
  const auto [pname, pflag] = search_flag(opt, prev.stop);

  // if opt not found in flags list...
  if (pflag == nullptr) {
    // (mandatory flag: -m, optional flag: -o, unrecognised flag: -A):
    //           |     no        .    lax        .   yes
    //     ------|---------------.---------------.-----------
    //     -m -m | missing input . missing input . input: -m
    //     -m -A | missing input . input: -A     . input: -A
    //     -m A  | input: A      . input: A      . input: A
    //     ------|---------------.---------------.-----------
    //     -o -o | no input      . no input      . no input
    //     -o -A | no input      . input: -A     . input: -A
    //     -o A  | input: A      . input: A      . input: A
    // where corresponding rows are blanked for -o (no input) if
    // allowing unrecognized flags or allowing arguments
    // respectively.
    // Above 'prev()' check covers the Greedy::yes scenario, which is the
    // only case that is an unconditional capture, otherwise whether we
    // capture or no depends on other factors that we now need to handle
    if (prev()) {
      // if prev was an optional flag
      if (prev.pflag->flag_class() == FlagClass::optional) {
        if (opt_is_arg(opt)) {
          // always capture unless we are allowing arguments
          if ( are_collecting_args() ) {
            add_instance(prev, pos);
            prev.clear();
            // don't return since _this_ opt still needs handling
          }
          else {
            if (add_instance(prev, pos, opt)) {
              errors = true;
            }
            prev.clear();
            return;
          }
        }
        else {
          // capture if not madatory greedy AND not allowing
          // unrecognized flags
          if (mandatory_greedy_ == Greedy::no
                                    || are_collecting_unrecognized_flags()) {
            add_instance(prev, pos);
            prev.clear();
            // don't return since _this_ opt still needs handling
          }
          else {
            if (add_instance(prev, pos, opt)) {
              errors = true;
            }
            prev.clear();
            return;
          }
        }
      }

      // otherwise it will be a mandatory flag
      else {
        if ((mandatory_greedy_ == Greedy::lax ||
                (mandatory_greedy_ == Greedy::no && opt_is_arg(opt)))) {
          if (add_instance(prev, pos, opt)) {
            errors = true;
          }
          prev.clear();
          return;
        }

        // if we hit here, then we have the missing input case ('-m -A')
        // in the subtable above. We need to print an error for '-m'
        // the previous opt having a missing input, and then let the code
        // continue to the code block that follows to handle the '-A' flag.
        if (prev.subpos > 1) {
          register_error(ErrorKey::MissingInput_, pos - 1, prev.subpos,
                         prev.pname->name(), opt_is_flag(prev.pname->name()));
        }
        else {
          register_error(ErrorKey::MissingInput, pos - 1, prev.pname->name(),
                         opt_is_flag(prev.pname->name()));
        }
        errors = true;
        prev.clear();
        // N.B., do NOT continue here: let the handle_unrecognized code
        // handle the current opt.
      }
    }

    if (handle_unrecognized(opt, pos, prev.stop)) {
      errors = true;
    }
    prev.clear();
    return;
  }

  // if 'prev' set here, then opt is a known flag; if prev was a madatory
  // flag then there is a missing flag error; for an optional flag, we
  // just add an instance with no input
  if (prev()) {
    if (prev.pflag->flag_class() == FlagClass::optional) {
      add_instance(prev, pos);
    }
    // else a mandatory flag (no other flag class will set 'prev')
    else {
      if (prev.subpos > 1) {
        register_error(ErrorKey::MissingInput_, pos - 1, prev.subpos,
                       prev.pname->name(), opt_is_flag(prev.pname->name()));
      }
      else {
        register_error(ErrorKey::MissingInput, pos - 1, prev.pname->name(),
                       opt_is_flag(prev.pname->name()));
      }
      errors = true;
    }
    // clear 'prev' since we've handled the previous opt contained
    // in 'prev' now; continue execution to handle _this_ opt
    prev.clear();
  }

  if (pname->type() == FlagType::short_type) {
    if (parse_short_opt(opt, pflag, pname, pos, prev)) {
      errors = true;
      return;
    }
  }
  else {
    if (parse_long_opt(opt, pflag, pname, pos, prev)) {
      errors = true;
      return;
    }
  }
}

template <typename TChar>
void BasicOpts<TChar>::parse_opt_finish(
      const CLType last_pos, Parsing& prev, bool& errors) {
  if (prev()) {
    if (prev.pflag->flag_class() == FlagClass::optional) {
      // optional flag left hanging; that's fine
      // increment last_pos since add_instance will subtract one
      add_instance(prev, last_pos + 1);
    }
    else {
      if (prev.subpos > 1) {
        register_error(ErrorKey::MissingInput_, last_pos, prev.subpos,
          prev.pname->name(), opt_is_flag(prev.pname->name()));
      }
      else {
        register_error(ErrorKey::MissingInput, last_pos, prev.pname->name(),
                       opt_is_flag(prev.pname->name()));
      }
      errors = true;
    }
  }
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::parse_short_opt(
      const StringView opt, const FlagPtr pflag, const NamePtr pname,
      const CLType pos, Parsing& prev) {
  using StringSize = typename String::size_type;

  // when chaining opts, we will fill in any present flag marker characters
  // that begin this chain
  const auto [flag_marker, fill_count] = opt_is_flag(opt)
    ? std::make_tuple<Char, StringSize>(static_cast<Char>(opt[0]), 1)
    : std::make_tuple<Char, StringSize>(static_cast<Char>('\0'), 0);

  // generally, we will return immediately if there are errors, since the
  // remaining opt string should be interpreted as unreliable, but for
  // things like bound limits we can continue parsing and just remember that
  // an error was encountered.
  bool errors = false;

  // the way that we will inspect the opt string for chaining is to set
  // prev for the first subopt and then skip to the second subopt to
  // analyze the previous one (as opposed to looking ahead to the next
  // subopt when handling the current subopt).
  for (StringSize i = 0, skip = 0; i < opt.size(); i += skip) {
    if ( prev() ) {
      // check prev is within limits
      //if ( check_within_limit(prev, pos, fill_count > 0 )) {
      //  // continue parsing
      //  errors = true;
      //}

      if ( prev.stop ) {
        // stop flag should not have an input

        // we will exit with error anyway, so don't worry about the
        // return value; cast to void to suppress the nodiscard
        // warning as per above
        (void)check_within_limit(prev, pos, fill_count > 0, true);

        register_error(ErrorKey::StopInput, pos, prev.pname->name(),
                            fill_count > 0, opt.substr(i));
        // don't clear 'prev.stop' set to true previously, since although
        // there is input that follows, we will interpret the presence of
        // the stop flag itself as not an error.
        prev.clear();
        return true;
      }

      // had a subopt previously, if it was optional or mandatory, then
      // add everything that follows as an input
      else if ( prev.pflag->flag_class() == FlagClass::optional
                  || prev.pflag->flag_class() == FlagClass::mandatory ) {

        if ( check_within_limit(prev, pos, fill_count > 0, true )) {
          errors = true;
        }

        if (prev.subpos > 1) {
          errors = add_instance(prev.pflag, prev.pname, pos, prev.subpos,
                       opt.substr(i), InputType::internal)
                || errors;
        }
        else {
          errors = add_instance(prev.pflag, prev.pname, pos,
                       opt.substr(i), InputType::internal)
                || errors;
        }
        prev.clear();
        // limits error: will add the instance but record the error. No
        // more chaining needs to be done since this is an optional or
        // mandatry flag, so return here
        return errors;
      }

      // bare flag or stop flag
      else {
        // search for flags
        const String subopt = String(fill_count, flag_marker)
                                + String{opt.substr(i)};
        // if we cannot chain, then we should not find a flag in subopt
        // even if it matches one; effectively, this is identical to having
        // a stop flag set, so when calling 'search_flag' we will check for
        // 'prev.stop' OR check if we cannot chain opts
        auto [ next_pname, next_pflag ] = search_flag(
                                            subopt, prev.stop || !can_chain_,
                                            true);

        // subopt is not a flag, so this must be an input to a bare opt,
        // which is an error
        if (next_pflag == nullptr) {

          // we will exit with error anyway, so don't worry about the
          // return value; cast to void to suppress the nodiscard
          // warning as per above
          (void)check_within_limit(prev, pos, fill_count > 0, true);

          if (prev.subpos > 1) {
            if ( prev.pflag->flag_class() == FlagClass::stop ) {
              // _will_ set prev.stop even though there is an error here
              prev.stop = true;
              register_error(ErrorKey::StopInput_, pos, prev.subpos,
                             prev.pname->name(),
                             fill_count > 0, opt.substr(i));
            }
            else {
              register_error(ErrorKey::BareInput_, pos, prev.subpos,
                             prev.pname->name(), fill_count > 0,
                             opt.substr(i));
            }
          }
          else {
            // stop flag doesn't ever reach this statement since it will
            // get caught above in the 'prev.stop' check, but I'll leave
            // this here anyway just in case...
            if ( prev.pflag->flag_class() == FlagClass::stop ) {
              // _will_ set prev.stop even though there is an error here
              prev.stop = true;
              register_error(ErrorKey::StopInput, pos, prev.pname->name(),
                             fill_count > 0, opt.substr(i) );
            }
            else {
              register_error(ErrorKey::BareInput, pos, prev.pname->name(),
                             fill_count > 0, opt.substr(i) );
            }
          }
          prev.clear();
          return true;
        }

        if ( check_within_limit(prev, pos, fill_count > 0, false) ) {
          errors = true;
        }
        else {
          // add instance, and set prev for next subopt
          add_instance(prev.pflag, prev.pname, pos, prev.subpos);
        }

        // set skip, which will point us to the next opt in the chain:
        // just take the size of this flag minus a flag marker if it
        // was present at the start of the chain
        skip = next_pname->length() - fill_count;

        // set prev for next subopt
        prev.pflag = next_pflag;
        prev.pname = next_pname;
        ++prev.subpos;
      }
    }

    // first opt in (potential) chain, just prepare 'prev' for subsequent
    // subopts
    else {
      if ( pflag->flag_class() == FlagClass::stop ) {
        prev.stop = true;
      }
      prev.pflag = pflag;
      prev.pname = pname;
      prev.subpos = 1;
      skip = pname->length();
    }
  }

  // all but the last subopt (or only opt if no chaining) has been handled,
  // which we need to do now. This last opt is stored in 'prev'.

  if ( check_within_limit(prev, pos, fill_count > 0, true) ) {
    // continue parsing
    errors = true;
  }

  if ( prev.pflag->flag_class() == FlagClass::mandatory ||
          (prev.pflag->flag_class() == FlagClass::optional
            && !optional_can_take_no_input()) ) {
    // leave 'prev' so that we should expect an input on the next opt; just
    // set subpos: if subpos is 1 so there was no chaining, reset it to zero
    if (prev.subpos == 1) {
      prev.subpos = 0;
    }
  }
  else {
    if (prev.subpos > 1) {
      add_instance(prev.pflag, prev.pname, pos, prev.subpos);
    }
    else {
      add_instance(prev.pflag, prev.pname, pos);
    }
    if ( prev.pflag->flag_class() == FlagClass::stop ) {
      prev.stop = true;
    }
    prev.clear();
  }

  return errors;
}

template <typename TChar>
[[nodiscard]] bool BasicOpts<TChar>::parse_long_opt(
      const StringView opt, const FlagPtr pflag, const NamePtr pname,
      const CLType pos, Parsing& prev) {
  bool errors = false;

  // check limits if given
  if ( check_within_limit(pflag, pname, pos, opt_is_flag(opt)) ) {
    errors = true;
  }

  if ( opt.size() == pname->length() ) {
    // --bare
    if ( pflag->flag_class() == FlagClass::bare ) {
      add_instance(pflag, pname, pos);
    }

    // --optional
    else if ( pflag->flag_class() == FlagClass::optional ) {
      //           |     no        .    lax        .   yes
      //     ------|---------------.---------------.-----------
      //     -o -o | no input      . no input      . no input
      //     -o -A | no input      . input: -A     . input: -A
      //     -o A  | input: A      . input: A      . input: A
      // usually, whether to take an input depends on what the next
      // opt is, but there are exceptions when we will never take
      // an input, which are:
      //  1) Greedy::no and allow arguments
      //  2) !Greedy::no and allow arguments AND allow unrec. flags
      if (optional_can_take_no_input()) {
        add_instance(pflag, pname, pos);
      }
      else {
        prev.pflag = pflag;
        prev.pname = pname;
      }
    }

    // --mandatory
    else if ( pflag->flag_class() == FlagClass::mandatory ) {
      // no input, so set prev state and wait for next opt
      prev.pflag = pflag;
      prev.pname = pname;
    }

    // --stop (stop opt)
    else {
      add_instance(pflag, pname, pos);
      prev.stop = true;
    }
  }
  else {
    // --bare=input
    if ( pflag->flag_class() == FlagClass::bare ) {
      const StringView input = opt.substr( pname->length() + 1 );
      if ( input.empty() ) {
        register_error(ErrorKey::BareEmptyInput, pos, pname->name(),
                       opt_is_flag(pname->name()));
      }
      else {
        register_error(ErrorKey::BareInput, pos, pname->name(),
                       opt_is_flag(pname->name()),
                       opt.substr( pname->length() + 1 ) );
      }
      return true;
    }

    // --stop=input
    if ( pflag->flag_class() == FlagClass::stop ) {
      // _will_ set prev.stop even though there is an error here
      prev.stop = true;
      const StringView input = opt.substr( pname->length() + 1 );
      if ( input.empty() ) {
        register_error(ErrorKey::StopEmptyInput, pos, pname->name(),
                       opt_is_flag(pname->name()));
      }
      else {
        register_error(ErrorKey::StopInput, pos, pname->name(),
                       opt_is_flag(pname->name()),
                       opt.substr( pname->length() + 1 ) );
      }
      return true;
    }

    // --man=input
    if ( pflag->flag_class() == FlagClass::mandatory ) {
      const StringView input = opt.substr( pname->length() + 1 );
      return add_instance(pflag, pname, pos, input, InputType::internal);
    }

    // --opt=input
    else {
      const StringView input = opt.substr( pname->length() + 1 );
      return add_instance(pflag, pname, pos, input, InputType::internal);
    }
  }
  return errors;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::registered_flags() const -> CanonNames {
  CanonNames ret;
  ret.reserve(flags_.size());
  for (const auto& flag: flags_) {
    ret.emplace_back(flag.canonical_name());
  }
  return ret;
}

template <typename TChar>
[[nodiscard]] auto BasicOpts<TChar>::registered_opts() const -> CanonNames {
  return registered_flags();
}


} // namespace CLUtils
