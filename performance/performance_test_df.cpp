///////////////////////////////////////////////////////////////
//  Copyright John Maddock 2019.
//  Copyright Christopher Kormanyos 2021 - 2023.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#if !defined(TEST_CPP_DOUBLE_FLOAT)
#define TEST_CPP_DOUBLE_FLOAT
#endif

#if !defined(TEST_CPP_BIN_FLOAT)
#define TEST_CPP_BIN_FLOAT
#endif

//
// This is the main entry point for our operator performance test suite.
// In order to build this program, you must compile and link this file against
// all the libs/multiprecision/performance/performance_test_files/*.cpp files.
// 

#include <boost/version.hpp>

#if defined(TEST_CPP_DOUBLE_FLOAT)
#include <boost/multiprecision/cpp_double_fp.hpp>
#endif

#if defined(TEST_CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_bin_float.hpp>
#endif

#include "performance_test.hpp"

//
// Keys in order are:
// Category
// Operator
// Type
// Precision
// Time
//
std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > > result_table;

unsigned bits_wanted; // for integer types

void quickbook_results()
{
   //
   // Keys in order are:
   // Category
   // Operator
   // Type
   // Precision
   // Time
   //
   typedef std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > >::const_iterator category_iterator;
   typedef std::map<std::string, std::map<std::string, std::map<int, double> > >::const_iterator                         operator_iterator;
   typedef std::map<std::string, std::map<int, double> >::const_iterator                                                 type_iterator;
   typedef std::map<int, double>::const_iterator                                                                         precision_iterator;

   for (category_iterator i = result_table.begin(); i != result_table.end(); ++i)
   {
      std::string cat = i->first;
      cat[0]          = (char)std::toupper((char)cat[0]);
      std::cout << "[section:" << i->first << "_performance " << cat << " Type Perfomance]" << std::endl;

      for (operator_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
         std::string op = j->first;
         std::cout << "[table Operator " << op << std::endl;
         std::cout << "[[Backend]";

         for (precision_iterator k = j->second.begin()->second.begin(); k != j->second.begin()->second.end(); ++k)
         {
            std::cout << "[" << k->first << " Bits]";
         }
         std::cout << "]\n";

         std::vector<double> best_times(j->second.begin()->second.size(), (std::numeric_limits<double>::max)());
         for (unsigned m = 0; m < j->second.begin()->second.size(); ++m)
         {
            for (type_iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
               if (m < k->second.size())
               {
                  precision_iterator l = k->second.begin();
                  std::advance(l, m);
                  if (best_times[m] > l->second)
                     best_times[m] = l->second ? l->second : best_times[m];
               }
            }
         }

         for (type_iterator k = j->second.begin(); k != j->second.end(); ++k)
         {
            std::cout << "[[" << k->first << "]";

            unsigned m = 0;
            for (precision_iterator l = k->second.begin(); l != k->second.end(); ++l)
            {
               double rel_time = l->second / best_times[m];
               if (rel_time == 1)
                  std::cout << "[[*" << rel_time << "]";
               else
                  std::cout << "[" << rel_time;
               std::cout << " (" << l->second << "s)]";
               ++m;
            }

            std::cout << "]\n";
         }

         std::cout << "]\n";
      }

      std::cout << "[endsect]" << std::endl;
   }
}

#if defined(__HAS_INCLUDE)
#if __has_include(<sys/utsname.h>)
#define HAS_UTSNAME
#include <sys/utsname.h>
#endif
#endif
#ifdef _WIN32
#include <windows.h>
#endif

void quickbook_platform_details()
{
   std::cout << "[table:platform Platform Details\n[[Platform][";
#ifdef HAS_UTSNAME
   utsname name;
   uname(&name);
   std::cout << name.sysname << " " << name.release << ", version " << name.version << ", " << name.machine << "]]\n";
#elif defined(_WIN32)
   std::cout << "Windows ";
#ifdef _M_AMD64
   std::cout << "x64";
#elif defined(_M_IX86)
   std::cout << "x86";
#endif
   std::cout << "]]\n";
#endif
   std::cout << "[[Compiler][" << BOOST_COMPILER << "]]\n";
#ifdef TEST_MPZ
   std::cout << "[[GMP][" << gmp_version << "]]\n";
#endif
#ifdef TEST_MPFR
   std::cout << "[[MPFR][" << MPFR_VERSION << "]]\n";
#endif
   std::cout << "[[Boost][" << BOOST_VERSION << "]]\n";
   std::cout << "[[Run date][" << __DATE__ << "]]\n";
   std::cout << "]\n\n";
}

int main()
{
   quickbook_platform_details();

   test32();
   test52();

   quickbook_results();
}

#ifdef TEST_CPP_DOUBLE_FLOAT
using double_float_of_double_type = boost::multiprecision::cpp_double_double;

constexpr auto digits10_for_performance_test = std::numeric_limits<double_float_of_double_type>::digits10;
#else
constexpr auto digits10_for_performance_test = 31;
#endif

void test32()
{
#ifdef TEST_CPP_BIN_FLOAT
   static_assert(digits10_for_performance_test >= 31, "Error: Too few digits for performance comparison");

   using cpp_bin_float_type =
      boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<digits10_for_performance_test>,
                                    boost::multiprecision::et_off>;

   test<cpp_bin_float_type>("cpp_bin_float", digits10_for_performance_test);
#endif
}

void test52()
{
#ifdef TEST_CPP_DOUBLE_FLOAT
   test<double_float_of_double_type>("cpp_double_fp_backend<double>", digits10_for_performance_test);
#endif
}
