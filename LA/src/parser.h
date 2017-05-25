// by: Zhiping

#pragma once

#include <pegtl.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>
#include <LA.h>

namespace LA {
  Program LA_parse_file (char *fileName);
}
