# libclutils

[![Build Status](https://travis-ci.com/definitelyprobably/libclutils.svg?branch=master)](https://travis-ci.com/definitelyprobably/libclutils)

C++ library for parsing command-line arguments.

## Building

Clone the repo and configure and install as usual; there is no need to build the library since it is header-only. A primitive check routine is also provided that requires /bin/bash. See the INSTALL.md file for more details. A C++20 compiler is required and autotools will need to be installed if building from a clone of the repo, but there are no other external dependencies for now.

## Documentation

Example code follows.

## Examples

### Basic Usage

```c++
#include <iostream>
#include <clutils.hpp>  // libclutils header

int main(int argc, char* argv[]) {
  using std::cout;
  using std::clog;

  CLUtils::Opts cl; // declare an Opts object.

  // declared options come in three types, called "bare", "optional" and
  // "mandatory".

  // declare a "bare" option. These options should not take an input, and
  // so the following forms should be recognized as errors:
  //  -hINPUT, --help=INPUT
  cl.add_bare("-h", "--help");

  // declare a "mandatory" option. These options must take an input, which
  // take the form of:
  //  -oINPUT, --output=INPUY
  cl.add_mandatory("-o", "--output");

  // declare an "optional" option. These options may or may not take an
  // input. The capture rules for inputs will be elucidated here and in
  // the examples that follow
  cl.add_optional("-l", "--log");


  // parse the arguments to the binary; a return status of "true" will
  // indicate that parsing errors were found
  if (cl.parse(argc, argv)) {

    // write errors to std::clog...
    cl.write_errors(clog);

    // ...and exit
    return 1;
  }


  // query if an opt was present with 'have_opt'. Any of the opt names
  // as synonyms of the opt when declared can be used (i.e. have_opt("-h")
  // and have_opt("--help") will behave identically)
  if (cl.have_opt("-h")) {

    // get_name: return the name of the opt passed last in argv
    // get_pos:  return the position of the opt passed last in argv
    //           (couting begins from 1).
    cout << " + opt '" << cl.get_name("-h") << "' at pos "
         << cl.get_pos("-h") << "\n";
  }

  // create a string object ('opt') to make the calls a bit cleaner
  if (const auto opt = "-l"; cl.have_opt(opt)) {

    cout << " + opt '" << cl.get_name(opt) << "' at pos "
         << cl.get_pos(opt);

    // get_input: return a tuple of containing the input to the opt, if
    //            present.
    if (const auto [has_input, input] = cl.get_input(opt); has_input) {
      cout << ", input: '" << input << "'";
    }
    cout << "\n";
  }

  // as above for the optional "-l" opt but the input is guaranteed to
  // be present for mandatory opt "-o".
  if (const auto opt = "-o"; cl.have_opt(opt)) {
    cout << " + opt '" << cl.get_name(opt) << "' at pos "
         << cl.get_pos(opt) << ", input: '"
         << std::get<1>(cl.get_input(opt)) << "'\n";
  }

}
```

Output:

```
$ ./bin -h -l -h -l file
 + flag '-h' at pos 3
 + flag '-l' at pos 4, input: 'file'

$ ./bin -h -l --help --log file
 + flag '--help' at pos 3
 + flag '--log' at pos 4, input: 'file'

$ ./bin -l -lfile
 + flag '-l' at pos 2, input: 'file'

$ ./bin -lfile -l
 + flag '-l' at pos 2

$ ./bin -lfile1 --log=file2
 + flag '--log' at pos 2, input: 'file2'

$ ./bin -o
error: arg 1: flag ‘-o’: expected input missing.
(returned: 1)

$ ./bin -o -o
error: arg 1: flag ‘-o’: expected input missing.
error: arg 2: flag ‘-o’: expected input missing.
(returned: 1)

$ ./bin -ofile
 + flag '-o' at pos 1, input: 'file'

$ ./bin -o file
 + flag '-o' at pos 1, input: 'file'

$ ./bin --output file1 --output=file2
 + flag '--output' at pos 3, input: 'file2'

$ ./bin -ho file
 + flag '-h' at pos 1
 + flag '-o' at pos 1, input: 'file'

$ ./bin -hl file -ho output
 + flag '-h' at pos 3
 + flag '-l' at pos 1, input: 'file'
 + flag '-o' at pos 3, input: 'output'

$ ./bin -hall
error: arg 1: flag ‘-h’: flag does not take an input: ‘all’.
(returned: 1)

$ ./bin --help=all
error: arg 1: flag ‘--help’: flag does not take an input: ‘all’.
(returned: 1)

$ ./bin --help all
error: arg 2: arg ‘all’: unrecognized arg.
(returned: 1)

$ ./bin --foo
error: arg 1: flag ‘--foo’: unrecognized flag.
(returned: 1)
```

## Licence

GNU GPL v3+

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
