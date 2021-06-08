find_package(FLEX 2.6.4 REQUIRED)
find_package(BISON 3.0.5 REQUIRED)

flex_target(lexer-query "${CMAKE_CURRENT_SOURCE_DIR}/flex.ll"
                      "${CMAKE_CURRENT_SOURCE_DIR}/Generated/lexer.cpp")

bison_target(parser-query "${CMAKE_CURRENT_SOURCE_DIR}/grammar.yy"
                        "${CMAKE_CURRENT_SOURCE_DIR}/Generated/parser.cpp")

add_flex_bison_dependency(lexer-query parser-query)

add_library(QueryParser 
	${BISON_parser-query_OUTPUTS} 
	${FLEX_lexer-query_OUTPUTS} 
	TAPNQueryParser.cpp
	AST.cpp
	BadPlaceVisitor.cpp
	ExpressionSatisfiedVisitor.cpp
	NormalizationVisitor.cpp
	ToStringVisitor.cpp
	UpwardClosedVisitor.cpp
)

add_dependencies(QueryParser rapidxml-ext)
