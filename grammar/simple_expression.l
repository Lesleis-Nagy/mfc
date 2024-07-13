%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexScanner */
#include "tecplot_scanner.hpp"
#undef  YY_DECL
#define YY_DECL int mfc::TecplotScanner::yylex(mfc::TecplotParser::semantic_type * const lval, mfc::TecplotParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = mfc::TecplotParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="mfc::TecplotScanner"
%option noyywrap
%option c++

%%
%{ /** Code executed at the beginning of yylex **/
    yylval = lval;
%}

"TITLE"                                          {
                                                    return(token::TITLE);
                                                 }

"VARIABLES"                                      {
                                                    return(token::VARIABLES);
                                                 }

"ZONE"                                           {
                                                    return(token::ZONE);
                                                 }

"T"                                              {
                                                    return(token::T);
                                                 }

"N"                                              {
                                                    return(token::N);
                                                 }

"E"                                              {
                                                    return(token::E);
                                                 }

"F"                                              {
                                                    return(token::F);
                                                 }

"FEBLOCK"                                        {
                                                    return(token::FEBLOCK);
                                                 }

"ET"                                             {
                                                    return(token::ET);
                                                 }

"TETRAHEDRON"                                    {
                                                    return(token::TETRAHEDRON);
                                                 }

"VARLOCATION"                                    {
                                                    return(token::VARLOCATION);
                                                 }

"CELLCENTERED"                                   {
                                                    return(token::CELLCENTERED);
                                                 }

"="                                              {
                                                    return(token::EQUAL);
                                                 }

"("                                              {
                                                    return(token::OPAREN);
                                                 }

")"                                              {
                                                    return(token::CPAREN);
                                                 }

"["                                              {
                                                    return(token::OSQUARE);
                                                 }

"]"                                              {
                                                    return(token::CSQUARE);
                                                 }

","                                              {
                                                    return(token::COMMA);
                                                 }

[0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)? {
                                                    yylval->build<double>(atof(yytext));
                                                    return(token::FLOAT);
                                                 }

[0-9]+                                           {
                                                    yylval->build<int>(atoi(yytext));
                                                    return(token::INT);
                                                 }

\"([^\\\"]|\\.)*\"                               {
                                                    yylval->build<std::string>(yytext);
                                                    return(token::STRING);
                                                 }

[ \n\t]                                          {;}

.                                                {;}

%%
