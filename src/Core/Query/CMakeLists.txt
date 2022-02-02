
add_library(Query
	AST.cpp
	BadPlaceVisitor.cpp
	ExpressionSatisfiedVisitor.cpp
	NormalizationVisitor.cpp
	ToStringVisitor.cpp
	UpwardClosedVisitor.cpp
        TranslationVisitor.cpp
)


add_dependencies(Query unfoldtacpn-ext)