%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%expect 16
%define api.namespace {mfc}
/**
 * bison 3.3.2 change
 * %define parser_class_name to this, updated
 * should work for previous bison versions as
 * well. -jcb 24 Jan 2020
 */
%define api.parser.class {TecplotParser}

%code requires{
   namespace mfc {
      class TecplotDriver;
      class TecplotScanner;
   }

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { TecplotScanner &scanner  }
%parse-param { TecplotDriver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <string>

   /* include for all driver functions */
   #include "tecplot_driver.hpp"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token               END        0     "end of file"
%token               TITLE
%token               VARIABLES
%token               ZONE
%token               T
%token               N
%token               E
%token               F
%token               FEBLOCK
%token               ET
%token               TETRAHEDRON
%token               VARLOCATION
%token               CELLCENTERED
%token               EQUAL
%token               OPAREN
%token               CPAREN
%token               OSQUARE
%token               CSQUARE
%token               COMMA
%token <double>      FLOAT
%token <int>         INT
%token <std::string> STRING

%locations

%%

expression
    : scalar_expression END
    | OANGLE vector_field CANGLE END
    ;

vector_field
    : three_field
    | two_field
    ;

two_field
    : scalar_expression {driver.expression(); }
      COMMA
      scalar_expression
    ;

three_field
    : two_field { driver.expression(); }
      COMMA
      scalar_expression
    ;

scalar_expression
    : SIN OPAREN scalar_expression CPAREN { driver.sin(); }
    | COS OPAREN scalar_expression CPAREN { driver.cos(); }
    | TAN OPAREN scalar_expression CPAREN { driver.tan(); }
    | scalar_expression MULTIPLY scalar_expression { driver.multiply(); }
    | scalar_expression DIVIDE scalar_expression { driver.divide(); }
    | scalar_expression PLUS scalar_expression { driver.plus(); }
    | scalar_expression MINUS scalar_expression { driver.minus(); }
    | OPAREN scalar_expression CPAREN
    | variable
    | FLOAT { driver.real($1); }

    ;

variable
    : XVAR { driver.xvar(); }
    | YVAR { driver.yvar(); }
    | ZVAR { driver.zvar(); }
    ;

%%

void
simple_expression::SimpleExpressionParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
