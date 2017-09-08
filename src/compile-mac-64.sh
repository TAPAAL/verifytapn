#!/bin/bash

parser=Core/QueryParser

export CPLUS_INCLUDE_PATH=/usr/local/Cellar/google-sparsehash/2.0.3/include

inc64="-I/Users/srba/dev/ia64/uppaal/include/ -I/usr/local/include/" 
lib64="-L$HOME/dev/ia64/uppaal/lib -ludbm"

compatibility="-mmacosx-version-min=10.7"

src=`find . -name "*.cpp"`

#flex -o $parser/Generated/lexer.cpp $parser/flex.ll
bison -o $parser/Generated/parser.cpp $parser/grammar.yy

g++ -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -stdlib=libc++ -O3 -Wall -mtune=core2 \
    $src $inc64 $lib64 $compatibility -o verifytapn64 
 

