/*
  ==============================================================================

    SampleDatabaseClasses.cpp
    Created: 2 Jun 2024 1:41:26pm
    Author:  Adam Jackson

  ==============================================================================
*/

#include "SampleDatabaseClasses.h"

bool SampleDatabaseConnection::accessorStatementsPrepared {false};
bool SampleDatabaseConnection::modifierStatementsPrepared {false};

std::mutex SampleDatabaseAccessor::stmtMtx;
sqlite3_stmt *SampleDatabaseAccessor::selectAnalysisStatement;
sqlite3_stmt *SampleDatabaseAccessor::selectUnanalysedRowsStatement;
sqlite3_stmt *SampleDatabaseAccessor::selectRowIdStatement;

std::mutex SampleDatabaseModifier::stmtMtx;
sqlite3_stmt *SampleDatabaseModifier::insertRowStatement;
sqlite3_stmt *SampleDatabaseModifier::removeRowStatement;
sqlite3_stmt *SampleDatabaseModifier::updateCategoryStatement;
sqlite3_stmt *SampleDatabaseModifier::updateAnalysisStatement;
