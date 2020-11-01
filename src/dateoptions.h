#pragma once
#include "ofMain.h"
#include "optionparser.h"

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};


enum  optionIndex { DATE_UNKNOWN, DATE_HELP, DATE_DATA_DIR, DATE_DEBUG, DATE_PROXY, DATE_OSC_IP };
const option::Descriptor usage[] = {
{ DATE_UNKNOWN, 0,"", "",         Arg::Unknown, "USAGE: example_arg [options]\n\n"
                                          "Options:" },
{ DATE_HELP,    0,"", "help",     Arg::None,    "  \t--help  \tPrint usage and exit." },
{ DATE_DATA_DIR,0,"o","data_root" ,Arg::Required,"  -d[<arg>], \t--data_root[=<arg>]"  },
{ DATE_DEBUG   ,0,"p","proxy"     ,Arg::None    ,"  -p[<arg>], \t--proxy[=<arg>]"  },
{ DATE_PROXY   ,0,"d","debug"     ,Arg::None    ,"  -p[<arg>], \t--proxy[=<arg>]"  },
{ DATE_OSC_IP  ,0,"","osc_ip     ",Arg::Required,"  -d[<arg>], \t--data_root[=<arg>]"  },
{ DATE_UNKNOWN, 0,"", "",         Arg::None,
 "\nExamples:\n"
 "  example_arg --unknown -o -n10 \n"
},
{ 0, 0, 0, 0, 0, 0 } };


class Args {

public:
    bool           proxy = false;
    bool           debug = false;
    std::string data_root = "";
    std::string   osc_ip = "127.0.0.1";

    Args(){};

    Args(int argc, char *argv[]) {
        argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
        option::Stats  stats(usage, argc, argv);
        std::vector<option::Option> options(stats.options_max);
        std::vector<option::Option> buffer(stats.buffer_max);
        option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

        if (!parse.error()){
            for (int i = 0; i < parse.optionsCount(); ++i)
            {
                option::Option& opt = buffer[i];
                fprintf(stdout, "Argument #%d is ", i);
                switch (opt.index())
                {
                case DATE_HELP:
                    fprintf(stdout, "--help was pressed\n");
                    break;
                case DATE_DATA_DIR:
                    data_root = std::string(opt.arg);
                    break;
                case DATE_DEBUG:
                    debug = true;
                    break;
                case DATE_PROXY:
                    proxy = true;
                    break;
                case DATE_OSC_IP:
                    data_root = std::string(opt.arg);
                    break;
                case DATE_UNKNOWN:
                    // not possible because Arg::Unknown returns ARG_ILLEGAL
                    // which aborts the parse with an error
                    break;
                }
            }

            for (int i = 0; i < parse.nonOptionsCount(); ++i)
                fprintf(stdout, "Non-option argument #%d is %s\n", i, parse.nonOption(i));
        } else {
            ofLogError("OptionsParser::OptionsParser") << "Parser error!";
        }

    }

};