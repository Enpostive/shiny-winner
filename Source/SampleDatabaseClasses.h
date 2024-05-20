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

class SampleDatabaseConnection
{
public:
 typedef std::map<int, juce::String> CatMap;
 
private:
 friend class SampleDatabaseAccessor;
 
 // Database path during development
 constexpr static char DatabasePath[] =
 "/Users/adam/Documents/Development/JUCE/SampleDatabase/SampleDatabase.sqlite";
 
 constexpr static char SelectCategoriesSQL[] =
 "SELECT rowid, category FROM categories;";
 
 sqlite3 *db;
 char *sqlError = 0;
 
 std::mutex mtx;

 void prepare(const char *query, sqlite3_stmt **stmt)
 {
  int p = sqlite3_prepare_v2(db,
                             query,
                             static_cast<int>(strlen(query) + 1),
                             stmt,
                             nullptr);
  if (p != SQLITE_OK)
  {
   juce::String errorStr = sqlite3_errmsg(db);
   int errOffset = sqlite3_error_offset(db);
   errOffset++;
  }
  jassert(p == SQLITE_OK);
 }

 CatMap _categories;

public:
 
 const CatMap &categories;
 
 SampleDatabaseConnection() :
 categories(_categories)
 {
  int rc = sqlite3_open(DatabasePath, &db);
  if (rc) throw std::runtime_error("Can't open database");
  
  reloadCategoriesIntoMap();
 }
 
 ~SampleDatabaseConnection()
 {
  sqlite3_close(db);
 }
 
 void reloadCategoriesIntoMap()
 {
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
  sqlite3_finalize(stmt);
 }
};










class SampleDatabaseAccessor
{
 // =================== Static query strings ==========================
 constexpr static char CountRowsSQL[] =
 "SELECT COUNT(path) FROM sampleFiles ";
 
 constexpr static char SelectRowPartialSQLFront[] =
 "SELECT path, categoryid "
 "FROM sampleFiles ";
 
 constexpr static char SelectRowPartialSQLEnd[] =
 "LIMIT 1 OFFSET ?1;";
 
 constexpr static char SelectRowSearchTermSQL[] =
 "WHERE path LIKE '%'||?2||'%' ";
 
 constexpr static char SelectRowFilterSQL[] =
 "WHERE categoryid = ?3 ";
 
 constexpr static char SelectRowFilterAndSearchSQL[] =
 "WHERE path LIKE '%' || ?2 || '%' AND categoryid = ?3 ";
 
 // sqlite3 database objects
 SampleDatabaseConnection &db;
 
 sqlite3_stmt *countRowsStatement;
 
 sqlite3_stmt *selectRowsStatement;
 juce::String orderBy {"ORDER BY path ASC "};
 juce::String searchTerm {""};
 bool categoryFilterEnable {false};
 int categoryFilterTerm {0};
 
 int selectedRowNumber {-1};
 juce::String path {""};
 int categoryId {0};
 
 void prepareRowSelect()
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

  juce::String query = juce::String::fromUTF8(SelectRowPartialSQLFront);
  query += whereClause;
  query += orderBy;
  query += juce::String::fromUTF8(SelectRowPartialSQLEnd);
  
  db.prepare(query.toStdString().data(), &selectRowsStatement);
  
  query = CountRowsSQL + whereClause + ";";
  db.prepare(query.toStdString().data(), &countRowsStatement);
 }
 
 void finaliseAndPrepareRowSelect()
 {
  sqlite3_finalize(selectRowsStatement);
  sqlite3_finalize(countRowsStatement);
  prepareRowSelect();
 }
 
 void bindFilterTerms(sqlite3_stmt *stmt)
 {
  if (searchTerm.isNotEmpty())
  {
   jassert(sqlite3_bind_text(stmt,
                             2,
                             searchTerm.toStdString().data(),
                             -1,
                             SQLITE_TRANSIENT) == SQLITE_OK);
  }
  if (categoryFilterEnable)
  {
   jassert(sqlite3_bind_int(stmt, 3, categoryFilterTerm) == SQLITE_OK);
  }
 }

public:
 SampleDatabaseAccessor(SampleDatabaseConnection &_db) :
 db(_db)
 {
  prepareRowSelect();
 }
 
 ~SampleDatabaseAccessor()
 {
  sqlite3_finalize(countRowsStatement);
  sqlite3_finalize(selectRowsStatement);
 }
 
 int getNumRows()
 {
  std::unique_lock lock(db.mtx);
  int count = 0;
  
  bindFilterTerms(countRowsStatement);
  
  if (sqlite3_step(countRowsStatement) == SQLITE_ROW)
  {
   count = sqlite3_column_int(countRowsStatement, 0);
  }
  jassert(sqlite3_reset(countRowsStatement) == SQLITE_OK);
  return count;
 }
 
 void sortByPath(bool isForward)
 {
  std::unique_lock lock(db.mtx);
  orderBy = juce::String("ORDER BY path ") + (isForward ? "ASC " : "DESC ");
  finaliseAndPrepareRowSelect();
 }
 
 void sortByCategory(bool isForward)
 {
  std::unique_lock lock(db.mtx);
  orderBy = juce::String("ORDER BY categoryid ") + (isForward ? "ASC " : "DESC ");
  finaliseAndPrepareRowSelect();
 }
 
 void search(juce::String term)
 {
  std::unique_lock lock(db.mtx);
  searchTerm = term;
  finaliseAndPrepareRowSelect();
 }
 
 void resetSearch()
 {
  std::unique_lock lock(db.mtx);
  searchTerm = "";
  finaliseAndPrepareRowSelect();
 }
 
 void filterByCategory(int categoryid)
 {
  std::unique_lock lock(db.mtx);
  categoryFilterEnable = true;
  categoryFilterTerm = categoryid;
  finaliseAndPrepareRowSelect();
 }
 
 void resetCategoryFilter()
 {
  std::unique_lock lock(db.mtx);
  categoryFilterEnable = false;
  finaliseAndPrepareRowSelect();
 }
 
 void selectRow(int rowNumber)
 {
  std::unique_lock lock(db.mtx);
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
    jassert(sqlite3_bind_int(selectRowsStatement, 1, rowNumber) == SQLITE_OK);
    bindFilterTerms(selectRowsStatement);
    if (sqlite3_step(selectRowsStatement) == SQLITE_ROW)
    {
     path = juce::String((const char*)sqlite3_column_text(selectRowsStatement, 0));
     categoryId = sqlite3_column_int(selectRowsStatement, 1);
    }
    jassert(sqlite3_reset(selectRowsStatement) == SQLITE_OK);
   }
  }
 }
 
 int getSelectedRow(int rowNumber)
 { return selectedRowNumber; }
 
 juce::String getPath() const
 {
  return path;
 }
 
 int getCategoryID() const
 {
  return categoryId;
 }
 
 juce::String getCategory() const
 {
  return db.categories.at(categoryId);
 }
};





