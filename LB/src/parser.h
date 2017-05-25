// by: Zhiping

#pragma once

#include <pegtl.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>
#include <LB.h>

namespace LB {
  Program LB_parse_file (char *fileName);
}
