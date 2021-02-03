#include <iostream>
#include <clutils.hpp>

using CLUtils::Opts;
//using Opts = CLUtils::BasicOpts<std::string_view>;

// outputs:
//   stdout: I:{flag}:{pos}:{subpos}[:{input}:{input_internal}]
//           U:{flag}:{pos}
//           A:{arg}:{pos}
//   stderr: E:{errno}:{pos}:{subpos}:{type}:{opt}:{input}
//           ret:{parse_retval}
// returns:
//     0 - if no exceptions thrown
//     1 - std::exception thrown
//     2 - any other exception thrown

void stdconfigA(Opts& cl) {
	cl.add_bare("-a", "--aflag");
	cl.add_bare("-b");
	cl.add_bare("--cflag");

	cl.add_optional("-o", "--oflag");
	cl.add_optional("-p");
	cl.add_optional("--qflag");

	cl.add_mandatory("-x", "--xflag");
	cl.add_mandatory("-y");
	cl.add_mandatory("--zflag");

	cl.add_stop("--", "--stop", "-s");
}

void stdconfigB(Opts& cl) {
	cl.add_bare("-a", "--aflag");
	cl.add_bare(2, "-b");
	cl.add_bare(2, "--cflag");
	cl.add_bare(1, "-d", "--dflag");

	cl.add_optional("-o", "--oflag");
	cl.add_optional(2, "-p");
	cl.add_optional(1, "--qflag");
	cl.add_optional(3, "-r", "--rflag");

	cl.add_mandatory("-x", "--xflag");
	cl.add_mandatory(2, "-y");
	cl.add_mandatory(3, "--zflag");
	cl.add_mandatory(1, "-w", "--wflag");

	cl.add_stop(2, "--", "--stop", "-s");
}

void stdconfigC(Opts& cl) {
	cl.add_bare("-a", "+a", "--aflag");
	cl.add_bare(2, "-b", "+b", "--bflag");
	cl.add_bare("-c", "--cflag");

	cl.add_optional("-o", "+o", "--oflag");
	cl.add_optional(2, "-p", "+p", "--pflag");
	cl.add_optional("-q", "--qflag");

	cl.add_mandatory("-x", "+x", "--xflag");
	cl.add_mandatory(2, "-y", "+y", "--yflag");
	cl.add_mandatory("-z", "--zflag");

	cl.add_bare("-");

	cl.set_flag_markers('-', '+');
	cl.add_flag_marker(95); // char: '_'
}

void stdconfigD(Opts& cl) {
	using namespace CLUtils::Literals;
	cl.add_bare("a");
	cl.add_bare("bflag");
	cl.add_bare("c"_short, "cflag"_long);
	cl.add_bare("d"_long, "dflag"_short);
	cl.add_bare(-1, "e", "eflag");

	cl.add_optional("o");
	cl.add_optional("pflag");
	cl.add_optional("q"_short, "qflag"_long);
	cl.add_optional("r"_long, "rflag"_short);
	cl.add_optional(-1231, "n"_long, "nflag"_short);

	cl.add_mandatory("x");
	cl.add_mandatory("yflag");
	cl.add_mandatory("z"_short, "zflag"_long);
	cl.add_mandatory("w"_long, "wflag"_short);
	cl.add_mandatory(0, "v", "vflag");

	cl.add_stop("s", "stop");
	cl.add_stop(0, "t", "tstop");
	cl.add_stop("u"_long, "ustop"_short);
}

void stdconfigE(Opts& cl) {
	using namespace CLUtils::Literals;
	cl.add_bare("-a", "a", "aa", "--aa");
	cl.add_bare("aaa", "--aaa");
	cl.add_bare("aaaa"_short);

	cl.add_optional("+o", "o", "oo", "++oo");
	cl.add_optional("ooo", "++ooo");
	cl.add_optional("oooo"_short);

	cl.add_mandatory("-x", "+x", "x", "xx", "--xx", "++xx");
	cl.add_mandatory("xxx");
	cl.add_mandatory("xxxx"_short);

	cl.add_flag_marker('+');
}


void config1(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy(true);
}

void config2(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy(false);
	cl.allow_empty_arguments(true);
	cl.allow_empty_inputs(true);
}

void config3(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
}

void config4(Opts& cl) {
	stdconfigA(cl);
	cl.allow_arguments();
	cl.set_greedy(CLUtils::Greedy::no);
}

void config5(Opts& cl) {
	stdconfigA(cl);
	cl.allow_arguments();
	cl.set_greedy(CLUtils::Greedy::lax);
}

void config6(Opts& cl) {
	stdconfigA(cl);
	cl.allow_arguments();
	cl.set_greedy(CLUtils::Greedy::yes);
}

void config7(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.allow_unrecognized_flags();
}

void config8(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy(CLUtils::Greedy::lax);
	cl.allow_unrecognized_flags();
}

void config9(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy(CLUtils::Greedy::yes);
	cl.allow_unrecognized_flags();
}

void config10(Opts& cl) {
	stdconfigB(cl);
	cl.set_greedy();
	cl.allow_arguments(3);
}

void config11(Opts& cl) {
	stdconfigC(cl);
	cl.set_not_greedy();
}

void config12(Opts& cl) {
	stdconfigC(cl);
	cl.set_greedy();
	cl.allow_arguments(2);
}

void config13(Opts& cl) {
	stdconfigD(cl);
	cl.set_greedy();
	cl.allow_arguments(2);
	cl.set_input_marker('~');
}

void config14(Opts& cl) {
	stdconfigE(cl);
	cl.allow_empty_inputs();
}

void config15(Opts& cl) {
	stdconfigE(cl);
	cl.allow_empty_inputs();
	cl.allow_arguments(8);
	cl.allow_empty_arguments();
}

void config16(Opts& cl) {
	stdconfigE(cl);
	cl.set_chaining();
	cl.allow_unrecognized_opts();
}

void config17(Opts& cl) {
	stdconfigE(cl);
	cl.set_no_chaining();
	cl.allow_unrecognized_opts();
}

void config18(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_not_greedy();
}

void config19(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
}

void config20(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_not_greedy();
	cl.allow_unrecognized_flags();
}

void config21(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}

void config22(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_not_greedy();
}

void config23(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
}

void config24(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_not_greedy();
	cl.allow_unrecognized_flags();
}

void config25(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}

void config26(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_not_greedy();
}

void config27(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
}

void config28(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_not_greedy();
	cl.allow_unrecognized_flags();
}

void config29(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_not_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}

void config30(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_greedy();
}

void config31(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
}

void config32(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_greedy();
	cl.allow_unrecognized_flags();
}

void config33(Opts& cl) {
	stdconfigA(cl);
	cl.set_not_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}

void config34(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_greedy();
}

void config35(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
}

void config36(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_greedy();
	cl.allow_unrecognized_flags();
}

void config37(Opts& cl) {
	stdconfigA(cl);
	cl.set_lax_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}

void config38(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_greedy();
}

void config39(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
}

void config40(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_greedy();
	cl.allow_unrecognized_flags();
}

void config41(Opts& cl) {
	stdconfigA(cl);
	cl.set_greedy();
	cl.set_optional_greedy();
	cl.allow_arguments();
	cl.allow_unrecognized_opts();
}



int main(int argc, char* argv[]) try {
	using std::cout;
	using std::clog;
	using namespace CLUtils::Literals;

	if (argc == 1) {
		return 1;
	}

	Opts cl;

	for (const auto& i: CLUtils::error_keys) {
		cl.format_error(i, "E:%errno:%pos:%subpos:%type:%opt:%input");
	}



	if ( std::string(argv[1]) == "config1" ) config1(cl);
	else if ( std::string(argv[1]) == "config2"  ) config2(cl);
	else if ( std::string(argv[1]) == "config3"  ) config3(cl);
	else if ( std::string(argv[1]) == "config4"  ) config4(cl);
	else if ( std::string(argv[1]) == "config5"  ) config5(cl);
	else if ( std::string(argv[1]) == "config6"  ) config6(cl);
	else if ( std::string(argv[1]) == "config7"  ) config7(cl);
	else if ( std::string(argv[1]) == "config8"  ) config8(cl);
	else if ( std::string(argv[1]) == "config9"  ) config9(cl);
	else if ( std::string(argv[1]) == "config10" ) config10(cl);
	else if ( std::string(argv[1]) == "config11" ) config11(cl);
	else if ( std::string(argv[1]) == "config12" ) config12(cl);
	else if ( std::string(argv[1]) == "config13" ) config13(cl);
	else if ( std::string(argv[1]) == "config14" ) config14(cl);
	else if ( std::string(argv[1]) == "config15" ) config15(cl);
	else if ( std::string(argv[1]) == "config16" ) config16(cl);
	else if ( std::string(argv[1]) == "config17" ) config17(cl);
	else if ( std::string(argv[1]) == "config18" ) config18(cl);
	else if ( std::string(argv[1]) == "config19" ) config19(cl);
	else if ( std::string(argv[1]) == "config20" ) config20(cl);
	else if ( std::string(argv[1]) == "config21" ) config21(cl);
	else if ( std::string(argv[1]) == "config22" ) config22(cl);
	else if ( std::string(argv[1]) == "config23" ) config23(cl);
	else if ( std::string(argv[1]) == "config24" ) config24(cl);
	else if ( std::string(argv[1]) == "config25" ) config25(cl);
	else if ( std::string(argv[1]) == "config26" ) config26(cl);
	else if ( std::string(argv[1]) == "config27" ) config27(cl);
	else if ( std::string(argv[1]) == "config28" ) config28(cl);
	else if ( std::string(argv[1]) == "config29" ) config29(cl);
	else if ( std::string(argv[1]) == "config30" ) config30(cl);
	else if ( std::string(argv[1]) == "config31" ) config31(cl);
	else if ( std::string(argv[1]) == "config32" ) config32(cl);
	else if ( std::string(argv[1]) == "config33" ) config33(cl);
	else if ( std::string(argv[1]) == "config34" ) config34(cl);
	else if ( std::string(argv[1]) == "config35" ) config35(cl);
	else if ( std::string(argv[1]) == "config36" ) config36(cl);
	else if ( std::string(argv[1]) == "config37" ) config37(cl);
	else if ( std::string(argv[1]) == "config38" ) config38(cl);
	else if ( std::string(argv[1]) == "config39" ) config39(cl);
	else if ( std::string(argv[1]) == "config40" ) config40(cl);
	else if ( std::string(argv[1]) == "config41" ) config41(cl);
	else {
		return 2;
	}


	const bool ret = cl.parse(argc, argv, 2);

	if (ret) {
		cl.write_errors(cout);
	}
	else {
		for (const auto& flag: cl.registered_opts()) {
			if ( cl.has_opt(flag) ) {
				for (const auto& instance: cl.get_all_instances(flag)) {
					cout << "I:" << cl.get_name(instance) << ":" << cl.get_pos(instance)
							 << ":" << cl.get_subpos(instance);
					if (auto [has_input, input] = cl.get_input(instance); has_input) {
						cout << ":" << (input.empty() ? "_empty_" : input)
								 << ":" << cl.is_input_external(instance);
					}
					cout << "\n";
				}
			}
		}
		for (const auto& unrec: cl.get_all_unrecognized_opts() ) {
			const auto name = unrec.name();
			cout << "U:" << (name.empty() ? "_empty_" : name)
					 << ":" << unrec.pos() << "\n";
		}
		for (const auto& arg: cl.get_all_arguments() ) {
			const auto name = arg.name();
			cout << "A:" << (name.empty() ? "_empty_" : name)
					 << ":" << arg.pos() << "\n";
		}
	}

	cout << "ret:" << ret << "\n";
	return 0;
}
catch (std::exception& exc) {
	std::clog << "exception: " << exc.what() << "\n";
	return 3;
}
catch (...) {
	return 4;
}
