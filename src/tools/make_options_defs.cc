#include "preprocessor.hpp"
#include <iostream>
#include <fstream>


const char* FILENAME = "options.autogen.h";

const char *TPL_HEADER = R"""(
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Automatically generated by make_options_def.py. !!!
// !!! do not edit manually                            !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef __OPTIONS_HH__
#define __OPTIONS_HH__

const char* option_defs[] = {
)""";


const char *TPL_FOOTER = R"""(
};

#endif // __OPTIONS_HH__
)""";

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
  if(from.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

int main(int argc, char** argv) {
  do_preprocess(argc, argv);

  std::ofstream file;
  file.open(FILENAME, std::ios::out);

  file << TPL_HEADER;
  for(auto p : defns) {
    for(; p; p=p->next) {
      // Ignore define name starts with _ , those are system defines.
      if (p->name[0] == '_') continue;
      if (p->flags & DEF_IS_UNDEFINED) continue;
      auto key = "__" + std::string(p->name) + "__";
      auto value = std::string(p->exps);
      replaceAll(value, "\"","\\\"" );

      file << "\"" << key << "\", \"" << value << "\"," << std::endl;
    }
  }
  file << TPL_FOOTER;

  std::cout << "Generate Success!";
  return 0;
}
