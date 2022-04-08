#include "ParsedQuery.h"

bool ParsedQuery::isDeclared(const std::string& synonym) {
	return true;
	//return declarations.find(synonym) != declarations.end();
}

EntityType ParsedQuery::getType(std::string& synonym) {
	return EntityType::ASSIGN;
	//return declarations.at(synonym);
}

std::unordered_map<std::string, EntityType> ParsedQuery::generateDeclarationMap() {
	std::unordered_map<std::string, EntityType> declarationMap;
	for (const auto& [entityType, synonym] : declarations) {
		if (declarationMap.find(synonym) != declarationMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		declarationMap.insert({ synonym, entityType });
	}
	return declarationMap;
}

std::unordered_set<std::string> ParsedQuery::getColumns(const std::unordered_map<std::string, EntityType>& declarationMap) {
	std::unordered_set<std::string> filteredColumns;
	for (const auto& [refType, synonym] : columns) {
		if (declarationMap.find(synonym) == declarationMap.end()) {
			/* Only case where this is valid - non-tuple select, synonym is "BOOLEAN"
			and no synonym attributes are selected -> we are actually projecting BOOLEAN.
			All other cases -> undeclared synonym, throw a validator error. */
			if (!(!tupleSelect && synonym == "BOOLEAN" && refType == PqlReferenceType::SYNONYM)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		} else {
			filteredColumns.insert(synonym);
		}
	}
	return filteredColumns;
}

std::vector<PqlReference> ParsedQuery::getAttributes(const std::unordered_map<std::string, EntityType>& declarationMap) {
	std::vector<PqlReference> attributes;
	for (const auto& [refType, synonym] : columns) {
		if (declarationMap.find(synonym) == declarationMap.end()) {
			/* Only case where this is valid - non-tuple select, synonym is "BOOLEAN"
			and no synonym attributes are selected -> we are actually projecting BOOLEAN.
			All other cases -> undeclared synonym, throw a validator error. */
			if (!(!tupleSelect && synonym == "BOOLEAN" && refType == PqlReferenceType::SYNONYM)) {
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		} else {
			attributes.emplace_back(refType, synonym);
		}
	}
	return attributes;
}

std::vector<Instruction*> ParsedQuery::getSuchThatInstructions(
	const std::unordered_map<std::string, EntityType>& declarationMap, const Processors& processors, PKBGetter* pkbGetter) {
	size_t numSuchThat = relationships.size();
	std::vector<Instruction*> suchThatInstructions;
	suchThatInstructions.reserve(numSuchThat * 3);

	for (ParsedRelationship& relationship : relationships) {
		suchThatInstructions.push_back(relationship.toInstruction(declarationMap, pkbGetter, processors));
		Instruction* leftFilter = relationship.getLeftFilter(declarationMap, pkbGetter);
		if (leftFilter != nullptr) {
			suchThatInstructions.push_back(leftFilter);
		}
		Instruction* rightFilter = relationship.getRightFilter(declarationMap, pkbGetter);
		if (rightFilter != nullptr) {
			suchThatInstructions.push_back(rightFilter);
		}
	}

	return suchThatInstructions;
}

std::vector<Instruction*> ParsedQuery::getPatternInstructions(
	const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter) {
	size_t numPattern = patterns.size();
	std::vector<Instruction*> patternInstructions;
	patternInstructions.reserve(numPattern);

	for (ParsedPattern& pattern : patterns) {
		patternInstructions.push_back(pattern.toInstruction(declarationMap, pkbGetter));
	}

	return patternInstructions;
}

std::vector<Instruction*> ParsedQuery::getWithInstructions(const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter) {
	size_t numWith = withs.size();
	std::vector<Instruction*> withInstructions;
	withInstructions.reserve(numWith);

	for (ParsedWith& with : withs) {
		withInstructions.push_back(with.toInstruction(declarationMap, pkbGetter));
	}

	return withInstructions;
}

ParsedQuery::ParsedQuery(const std::vector<PqlEntity>& declarations,
	const std::vector<PqlReference>& columns,
	const std::vector<ParsedRelationship>& relationships,
	const std::vector<ParsedPattern>& patterns,
	const std::vector<ParsedWith>& withs,
	bool tupleSelect)
	: declarations(declarations),
	columns(columns),
	relationships(relationships),
	patterns(patterns),
	withs(withs),
	tupleSelect(tupleSelect) {
}

bool ParsedQuery::isStmtSubtype(PqlReference ref) {
	std::string& refName = ref.second;
	EntityType entType = getType(refName);
	return (entType == EntityType::ASSIGN)
		|| (entType == EntityType::CALL)
		|| (entType == EntityType::IF)
		|| (entType == EntityType::PRINT)
		|| (entType == EntityType::READ)
		|| (entType == EntityType::STMT)
		|| (entType == EntityType::WHILE);
}

ProjectionType ParsedQuery::getProjectionType(std::vector<PqlReference> attributesProjected) {
	if (attributesProjected.size() == 0) {
		return ProjectionType::BOOLEAN;
	} else if (attributesProjected.size() == 1) {
		return ProjectionType::SINGLE;
	} else if (attributesProjected.size() >= 1) {
		return ProjectionType::TUPLE;
	} else {
		throw EvaluatorException(EvaluatorException::PROJECTION_TYPE_ERROR);
	}
}

bool ParsedQuery::isClausePresent(ParsedQuery& parsedQuery) {
	std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
	std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
	std::vector<ParsedWith> withs = parsedQuery.getWiths();
	return !(relationships.empty() && patterns.empty() && withs.empty());
}

std::unordered_map<std::string, EntityType> ParsedQuery::getDeclarations() {
	std::unordered_map<std::string, EntityType> typeMap;
	for (const auto& [entType, synonym] : declarations) {
		typeMap.insert({ synonym, entType });
	}
	return typeMap;
}

std::unordered_set<std::string> ParsedQuery::getColumns() {
	size_t numCols = columns.size();
	std::unordered_set<std::string> columnNames;
	columnNames.reserve(numCols);
	for (const PqlReference& reference : columns) {
		columnNames.insert(reference.second);
	}
	return columnNames;
}

std::vector<ParsedRelationship> ParsedQuery::getRelationships() {
	return relationships;
}

std::vector<ParsedPattern> ParsedQuery::getPatterns() {
	return patterns;
}

std::vector<ParsedWith> ParsedQuery::getWiths() {
	return withs;
}

std::vector<Instruction*> ParsedQuery::getClauseInstructions(const Processors& processors, PKBGetter* pkbGetter) {
	std::unordered_map<std::string, EntityType> declarationMap = generateDeclarationMap();
	std::vector<Instruction*> instructions;

	std::vector<Instruction*> suchThatInstructions = getSuchThatInstructions(declarationMap, processors, pkbGetter);
	for (Instruction* instruction : suchThatInstructions) {
		instructions.push_back(instruction);
	}

	std::vector<Instruction*> patternInstructions = getPatternInstructions(declarationMap, pkbGetter);
	for (Instruction* instruction : patternInstructions) {
		instructions.push_back(instruction);
	}

	std::vector<Instruction*> withInstructions = getWithInstructions(declarationMap, pkbGetter);
	for (Instruction* instruction : withInstructions) {
		instructions.push_back(instruction);
	}

	return instructions;
}

std::vector<Instruction*> ParsedQuery::getSelectInstructions(PKBGetter* pkbGetter) {
	size_t numSelects = columns.size();
	std::unordered_map<std::string, EntityType> declarationMap = generateDeclarationMap();
	std::unordered_set<std::string> uniqueColumns;
	uniqueColumns.reserve(numSelects);
	std::vector<Instruction*> instructions;

	for (const auto& [refType, synonym] : columns) {
		uniqueColumns.insert(synonym);
	}

	for (const std::string& synonym : uniqueColumns) {
		if (declarationMap.find(synonym) == declarationMap.end()) {
			throw QPSException(QPSException::VALIDATOR_ERROR);
		}
		EntityType entType = declarationMap.at(synonym);
		ParsedGetAll getAll(entType, synonym);
		instructions.push_back(getAll.toInstruction(pkbGetter));
	}

	return instructions;
}

std::vector<PqlReference> ParsedQuery::getAttributes() {
	size_t numSelects = columns.size();
	std::vector<PqlReference> attributes;
	attributes.reserve(numSelects);

	std::unordered_map<std::string, EntityType> typeMap = getDeclarations();
	
	for (const PqlReference& pqlRef : columns) {
		const auto& [refType, synonym] = pqlRef;

		if (typeMap.find(synonym) == typeMap.end()) {
			if (!(synonym == "BOOLEAN" && refType == PqlReferenceType::SYNONYM && !tupleSelect)) {
				/* Only one corner case - if the "synonym" is BOOLEAN, selected as the synonym and not as an attribute,
				and it is not selected in a tuple, then we treat it as a BOOLEAN projection.
				All other cases are treated as semantically invalid queries. */
				throw QPSException(QPSException::VALIDATOR_ERROR);
			}
		} else {
			attributes.push_back(pqlRef);
		}
	}

	return attributes;
}
