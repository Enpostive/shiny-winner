/*
  ==============================================================================

    SampleDatabaseClasses.h
    Created: 18 May 2024 9:23:55am
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "sqlite3/sqlite3.h"









class SampleDatabaseAccessor;
class SampleDatabaseModifier;

class SampleDatabaseConnection
{
public:
 typedef std::map<int, juce::String> CategoryMap;
 
 class Exception : public std::runtime_error
 {
 public:
  Exception(sqlite3* db) :
  std::runtime_error("SQLITE3 error")
  {
   juce::String errorStr = sqlite3_errmsg(db);
   int errOffset = sqlite3_error_offset(db);
   errOffset++;
  }
 };


private:
 friend class SampleDatabaseAccessor;
 friend class SampleDatabaseModifier;
 
 // Database path during development
 constexpr static char DatabasePath[] =
 "/Users/adam/Documents/Development/JUCE/SampleDatabase/SampleDatabase.sqlite";
 
 constexpr static char SelectCategoriesSQL[] =
 "SELECT rowid, category FROM categories;";
 
 sqlite3 *db;
 char *sqlError = 0;
 
 static bool accessorStatementsPrepared;
 static bool modifierStatementsPrepared;
 
 std::mutex mtx;

 void prepare(const juce::String &query, sqlite3_stmt **stmt)
 {
  dotry(sqlite3_prepare_v2(db,
                           query.toStdString().data(),
                           static_cast<int>(query.length() + 1),
                           stmt,
                           nullptr),
        SQLITE_OK);
 }

 CategoryMap _categories;

public:
 
 const CategoryMap &categories;
 
 SampleDatabaseConnection() :
 categories(_categories)
 {
  int rc = sqlite3_open(DatabasePath, &db);
  if (rc) throw std::runtime_error("Can't open database");
  
  reloadCategoriesIntoMap();
 }
 
 ~SampleDatabaseConnection()
 {
  std::unique_lock lock(mtx);
  
  accessorStatementsPrepared = false;
  modifierStatementsPrepared = false;
  
  sqlite3_stmt *stmt;
  do
  {
   stmt = sqlite3_next_stmt(db, nullptr);
   if (stmt) dotry(sqlite3_finalize(stmt), SQLITE_OK);
  } while (stmt != nullptr);
  
  dotry(sqlite3_close(db), SQLITE_OK);
 }
 
 void dotry(int result, int expected)
 {
  if (result != expected) throw Exception(db);
 }
 
 void reloadCategoriesIntoMap()
 {
  std::unique_lock lock(mtx);
  sqlite3_stmt *stmt;
  int rowid;
  const char *category;
  
  prepare(SelectCategoriesSQL, &stmt);
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
   rowid = sqlite3_column_int(stmt, 0);
   category = (const char*)sqlite3_column_text(stmt, 1);
   if (category) _categories[rowid] = juce::String::fromUTF8(category);
  }
  dotry(sqlite3_finalize(stmt), SQLITE_OK);
 }
 
 int getLastInsertedRowId()
 { return static_cast<int>(sqlite3_last_insert_rowid(db)); }
};










class SampleDatabaseAccessor
{
 // =================== Static query strings ==========================
 constexpr static char CountRowsSQL[] =
 "SELECT COUNT(path) FROM sampleFiles ";
 
 constexpr static char SelectRowPartialSQLFront[] =
 "SELECT rowid, path, categoryid "
 "FROM sampleFiles ";
 
 constexpr static char SelectAnalysisPartialSQLFront[] =
 "SELECT analysis FROM sampleFiles ";
 
 constexpr static char SelectRowNumberPartialSQLFront[] =
 "SELECT rownum FROM (SELECT *, ROW_NUMBER() OVER (";
 
 constexpr static char SelectRowNumberPartialSQLAfterOrderBy[] =
 ") AS rownum FROM sampleFiles ";
 
 constexpr static char SelectRowNumberPartialSQLAfterWhereClause[] =
 ") WHERE path = ?4;";
 
 constexpr static char SelectRowPartialSQLEnd[] =
 "LIMIT 1 OFFSET ?1;";
 
 constexpr static char SelectRowIdPartialSQLEnd[] =
 "WHERE rowid = ?1;";
 
 constexpr static char SelectRowSearchTermSQL[] =
 "WHERE path LIKE '%'||?2||'%' ";
 
 constexpr static char SelectRowFilterSQL[] =
 "WHERE categoryid = ?3 ";
 
 constexpr static char SelectRowFilterAndSearchSQL[] =
 "WHERE path LIKE '%' || ?2 || '%' AND categoryid = ?3 ";
 
 constexpr static char SelectUnanalysedRowIDSQL[] =
 "SELECT rowid FROM sampleFiles "
 "WHERE analysis='' OR analysis IS NULL;";
 
 // sqlite3 database objects
 SampleDatabaseConnection &dbConn;
 
 static std::mutex stmtMtx;
 static sqlite3_stmt *selectAnalysisStatement;
 static sqlite3_stmt *selectUnanalysedRowsStatement;
 static sqlite3_stmt *selectRowIdStatement;
 
 sqlite3_stmt *countRowsStatement;
 sqlite3_stmt *selectRowsStatement;

 juce::String orderBy {"ORDER BY path ASC "};
 juce::String searchTerm {""};
 bool categoryFilterEnable {false};
 int categoryFilterTerm {0};
 
 int selectedRowNumber {-1};
 int rowid;
 juce::String path {""};
 int categoryId {0};
 
 juce::String chooseWhereClause()
 {
  juce::String whereClause {""};
  
  if (searchTerm.isNotEmpty() && categoryFilterEnable)
  {
   whereClause = SelectRowFilterAndSearchSQL;
  }
  else if (searchTerm.isNotEmpty())
  {
   whereClause = SelectRowSearchTermSQL;
  }
  else if (categoryFilterEnable)
  {
   whereClause = SelectRowFilterSQL;
  }

  return whereClause;
 }
 
 void prepareRowSelect()
 {
  juce::String whereClause {chooseWhereClause()};
  
  juce::String query = juce::String::fromUTF8(SelectRowPartialSQLFront);
  query += whereClause + orderBy;
  query += SelectRowPartialSQLEnd;
  dbConn.prepare(query, &selectRowsStatement);
  
  query = CountRowsSQL + whereClause + ";";
  dbConn.prepare(query, &countRowsStatement);
 }
 
 void finaliseRowSelect()
 {
  dbConn.dotry(sqlite3_finalize(selectRowsStatement), SQLITE_OK);
  dbConn.dotry(sqlite3_finalize(countRowsStatement), SQLITE_OK);
 }
 
 void finaliseAndPrepareRowSelect()
 {
  finaliseRowSelect();
  prepareRowSelect();
 }
 
 void bindFilterTerms(sqlite3_stmt *stmt, int rowNumber = -1)
 {
  if (rowNumber != -1)
  {
   dbConn.dotry(sqlite3_bind_int(stmt, 1, rowNumber), SQLITE_OK);
  }
  if (searchTerm.isNotEmpty())
  {
   dbConn.dotry(sqlite3_bind_text(stmt, 2,
                                  searchTerm.toStdString().data(),
                                  -1, SQLITE_TRANSIENT),
                SQLITE_OK);
  }
  if (categoryFilterEnable)
  {
   dbConn.dotry(sqlite3_bind_int(stmt, 3, categoryFilterTerm), SQLITE_OK);
  }
 }

public:
 SampleDatabaseAccessor(SampleDatabaseConnection &db) :
 dbConn(db)
 {
  std::unique_lock lockConn(dbConn.mtx, std::defer_lock);
  std::unique_lock lockStmt(stmtMtx, std::defer_lock);
  std::lock(lockConn, lockStmt);
  
  prepareRowSelect();

  if (!dbConn.accessorStatementsPrepared)
  {
   juce::String query = juce::String(SelectRowPartialSQLFront);
   query += SelectRowIdPartialSQLEnd;
   dbConn.prepare(query, &selectRowIdStatement);

   query = juce::String::fromUTF8(SelectAnalysisPartialSQLFront);
   query += SelectRowIdPartialSQLEnd;
   dbConn.prepare(query, &selectAnalysisStatement);
   
   dbConn.prepare(SelectUnanalysedRowIDSQL, &selectUnanalysedRowsStatement);
   dbConn.accessorStatementsPrepared = true;
  }
 }
 
 ~SampleDatabaseAccessor()
 {
  std::unique_lock lockConn(dbConn.mtx);
  finaliseRowSelect();
 }
 
 int getNumRows()
 {
  std::unique_lock lock(dbConn.mtx);
  int count = 0;
  
  bindFilterTerms(countRowsStatement);
  
  if (sqlite3_step(countRowsStatement) == SQLITE_ROW)
  {
   count = sqlite3_column_int(countRowsStatement, 0);
  }
  dbConn.dotry(sqlite3_reset(countRowsStatement), SQLITE_OK);
  return count;
 }
 
 void sortByPath(bool isForward)
 {
  std::unique_lock lock(dbConn.mtx);
  orderBy = juce::String("ORDER BY path ") + (isForward ? "ASC " : "DESC ");
  finaliseAndPrepareRowSelect();
 }
 
 void sortByCategory(bool isForward)
 {
  std::unique_lock lock(dbConn.mtx);
  orderBy = juce::String("ORDER BY categoryid ") + (isForward ? "ASC " : "DESC ");
  finaliseAndPrepareRowSelect();
 }
 
 void search(juce::String term)
 {
  std::unique_lock lock(dbConn.mtx);
  searchTerm = term;
  finaliseAndPrepareRowSelect();
 }
 
 void resetSearch()
 {
  std::unique_lock lock(dbConn.mtx);
  searchTerm = "";
  finaliseAndPrepareRowSelect();
 }
 
 void filterByCategory(int categoryid)
 {
  std::unique_lock lock(dbConn.mtx);
  categoryFilterEnable = true;
  categoryFilterTerm = categoryid;
  finaliseAndPrepareRowSelect();
 }
 
 void resetCategoryFilter()
 {
  std::unique_lock lock(dbConn.mtx);
  categoryFilterEnable = false;
  finaliseAndPrepareRowSelect();
 }
 
 int getRowNumberFromPath(const juce::String &path)
 {
  int result = -1;
  juce::String query;
  query = (juce::String::fromUTF8(SelectRowNumberPartialSQLFront) +
           orderBy +
           juce::String::fromUTF8(SelectRowNumberPartialSQLAfterOrderBy) +
           chooseWhereClause() +
           juce::String::fromUTF8(SelectRowNumberPartialSQLAfterWhereClause));
  
  sqlite3_stmt *stmt;
  dbConn.prepare(query, &stmt);
  bindFilterTerms(stmt);
  dbConn.dotry(sqlite3_bind_text(stmt, 4, path.toUTF8(), -1, nullptr), SQLITE_OK);
  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
   result = sqlite3_column_int(stmt, 0);
  }
  dbConn.dotry(sqlite3_finalize(stmt), SQLITE_OK);
  return result;
 }
 
 void selectRow(int rowNumber)
 {
  std::unique_lock lock(dbConn.mtx);
  if (rowNumber != selectedRowNumber)
  {
   selectedRowNumber = rowNumber;
   if (rowNumber == -1)
   {
    path = "";
    categoryId = 0;
   }
   else
   {
    bindFilterTerms(selectRowsStatement, rowNumber);
    if (sqlite3_step(selectRowsStatement) == SQLITE_ROW)
    {
     rowid = sqlite3_column_int(selectRowsStatement, 0);
     path = juce::String::fromUTF8((const char*)sqlite3_column_text(selectRowsStatement, 1));
     categoryId = sqlite3_column_int(selectRowsStatement, 2);
    }
    dbConn.dotry(sqlite3_reset(selectRowsStatement), SQLITE_OK);
   }
  }
 }
 
 void selectRowId(int rowIdent)
 {
  std::unique_lock lock(dbConn.mtx);
  dbConn.dotry(sqlite3_bind_int(selectRowIdStatement, 1, rowIdent), SQLITE_OK);
  if (sqlite3_step(selectRowIdStatement) == SQLITE_ROW)
  {
   selectedRowNumber = -1;
   rowid = sqlite3_column_int(selectRowIdStatement, 0);
   path = juce::String::fromUTF8((const char*)sqlite3_column_text(selectRowIdStatement, 1));
   categoryId = sqlite3_column_int(selectRowIdStatement, 2);
  }
  dbConn.dotry(sqlite3_reset(selectRowIdStatement), SQLITE_OK);
 }
 
 int getSelectedRow(int rowNumber)
 { return selectedRowNumber; }
 
 juce::String getPath() const
 { return path; }
 
 int getCategoryID() const
 { return categoryId; }
 
 juce::String getCategory() const
 { return categoryId == 0 ? "" : dbConn.categories.at(categoryId); }
 
 int getRowId() const
 { return rowid; }
 
 juce::String getAnalysisForID(int rowIdent)
 {
  std::unique_lock lock(dbConn.mtx);
  juce::String result {""};
  dbConn.dotry(sqlite3_bind_int(selectAnalysisStatement, 1, rowIdent), SQLITE_OK);
  if (sqlite3_step(selectAnalysisStatement) == SQLITE_ROW)
  {
   result = juce::String::fromUTF8((const char*)sqlite3_column_text(selectAnalysisStatement, 0));
  }
  dbConn.dotry(sqlite3_reset(selectAnalysisStatement), SQLITE_OK);
  
  return result;
 }
 
 std::vector<int> getUnanalysedRows()
 {
  std::unique_lock lock(dbConn.mtx);
  std::vector<int> rows;
  
  while (sqlite3_step(selectUnanalysedRowsStatement) == SQLITE_ROW)
  {
   rows.push_back(sqlite3_column_int(selectUnanalysedRowsStatement, 0));
  }
  dbConn.dotry(sqlite3_finalize(selectUnanalysedRowsStatement), SQLITE_OK);

  return rows;
 }
};










class SampleDatabaseModifier
{
 SampleDatabaseConnection &dbConn;
 
 constexpr static char InsertRowSQL[] =
 "INSERT OR REPLACE INTO sampleFiles (path, categoryid) VALUES (?1, ?2);";
 
 constexpr static char RemoveRowSQL[] =
 "DELETE FROM sampleFiles WHERE rowid = ?1;";
 
 constexpr static char UpdateCategorySQL[] =
 "UPDATE sampleFiles SET categoryid = ?1 WHERE rowid = ?2;";
 
 constexpr static char UpdateAnalysisSQL[] =
 "UPDATE sampleFiles SET analysis = ?1 WHERE rowid = ?2;";
 
 static std::mutex stmtMtx;
 static sqlite3_stmt *insertRowStatement;
 static sqlite3_stmt *removeRowStatement;
 static sqlite3_stmt *updateCategoryStatement;
 static sqlite3_stmt *updateAnalysisStatement;

public:
 SampleDatabaseModifier(SampleDatabaseConnection &db) :
 dbConn(db)
 {
  std::unique_lock lockConn(dbConn.mtx, std::defer_lock);
  std::unique_lock lockStmt(stmtMtx, std::defer_lock);
  std::lock(lockConn, lockStmt);

  if (!dbConn.modifierStatementsPrepared)
  {
   dbConn.prepare(InsertRowSQL, &insertRowStatement);
   dbConn.prepare(RemoveRowSQL, &removeRowStatement);
   dbConn.prepare(UpdateCategorySQL, &updateCategoryStatement);
   dbConn.prepare(UpdateAnalysisSQL, &updateAnalysisStatement);
   dbConn.modifierStatementsPrepared = true;
  }
 }
 
 ~SampleDatabaseModifier()
 { }
 
 int insertRow(const juce::String &path, int categoryid)
 {
  std::unique_lock lock(dbConn.mtx);
  int insertedRowId = -1;
  
  dbConn.dotry(sqlite3_bind_text(insertRowStatement, 1,
                                 path.toStdString().data(),
                                 -1, SQLITE_TRANSIENT),
               SQLITE_OK);
  dbConn.dotry(sqlite3_bind_int(insertRowStatement, 2, categoryid), SQLITE_OK);
  
  if (sqlite3_step(insertRowStatement) == SQLITE_DONE)
  {
   insertedRowId = dbConn.getLastInsertedRowId();
  }
  dbConn.dotry(sqlite3_reset(insertRowStatement), SQLITE_OK);
  
  return insertedRowId;
 }
 
 void deleteRow(int rowid)
 {
  std::unique_lock lock(dbConn.mtx);
  dbConn.dotry(sqlite3_bind_int(removeRowStatement, 1, rowid), SQLITE_OK);
  dbConn.dotry(sqlite3_step(removeRowStatement), SQLITE_DONE);
  dbConn.dotry(sqlite3_reset(removeRowStatement), SQLITE_OK);
 }
 
 void updateCategory(int rowid, int categoryid)
 {
  std::unique_lock lock(dbConn.mtx);
  dbConn.dotry(sqlite3_bind_int(updateCategoryStatement, 1, categoryid), SQLITE_OK);
  dbConn.dotry(sqlite3_bind_int(updateCategoryStatement, 2, rowid), SQLITE_OK);
  dbConn.dotry(sqlite3_step(updateCategoryStatement), SQLITE_DONE);
  dbConn.dotry(sqlite3_reset(updateCategoryStatement), SQLITE_OK);
 }
 
 void updateAnalysis(int rowid, const juce::String &analysis)
 {
  std::unique_lock lock(dbConn.mtx);
  dbConn.dotry(sqlite3_bind_text(updateAnalysisStatement, 1,
                                 analysis.toStdString().data(),
                                 -1, SQLITE_TRANSIENT),
               SQLITE_OK);
  dbConn.dotry(sqlite3_bind_int(updateAnalysisStatement, 2, rowid), SQLITE_OK);
  dbConn.dotry(sqlite3_step(updateAnalysisStatement), SQLITE_DONE);
  dbConn.dotry(sqlite3_reset(updateAnalysisStatement), SQLITE_OK);
 }
};
