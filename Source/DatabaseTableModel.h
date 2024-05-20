/*
  ==============================================================================

    DatabaseTableModel.h
    Created: 14 May 2024 7:02:40pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SampleDatabaseClasses.h"
#include "XDDSP/XDDSP_Functions.h"















class DatabaseTableModel : public juce::TableListBoxModel, public juce::ComponentListener
{
 // Column IDs
 enum
 {
  StatusColumn = 1,
  FilePathColumn,
  CategoryColumn
 };
 
 // ===================================================================

 // Juce Table Object that it is connected to
 juce::TableListBox *table {nullptr};
 
 // Data cache
 struct TableRowCacheLine
 {
  int rowNumber {-1};
  bool fileError {false};
  juce::File sampleFile;
  int category {0};
 };

 XDDSP::PowerSize cacheSize;
 std::vector<TableRowCacheLine> rowCache;
 
 void clearCache()
 {
  for (auto &row: rowCache) row.rowNumber = -1;
 }
 
 void resizeCache()
 {
  if (table)
  {
   cacheSize.setToNextPowerTwo(table->getNumRowsOnScreen());
   rowCache.resize(cacheSize.size());
  }
 }
 
 TableRowCacheLine& touchCache(int rowNumber)
 {
  int cacheLine = rowNumber & cacheSize.mask();
  if (rowCache[cacheLine].rowNumber != rowNumber)
  {
   dbAccess.selectRow(rowNumber);
   rowCache[cacheLine].sampleFile = juce::File(dbAccess.getPath());
   rowCache[cacheLine].category = dbAccess.getCategoryID();
  }
   
  return rowCache[cacheLine];
 }
 
 // Table properties
 juce::Font font {14.0f};
 
 
 
 SampleDatabaseConnection &dbConn;
 SampleDatabaseAccessor dbAccess;
 
 
 
 //======================== PUBLIC MEMBERS =====================================
public:
 
 std::function<void (int rowNumber)> onRowSelected;
 
 DatabaseTableModel(SampleDatabaseConnection &_db) :
 dbConn(_db),
 dbAccess(_db)
 {
 }
 
 virtual ~DatabaseTableModel() override
 {
  connectToTable(nullptr);
 }
 
 void connectToTable(juce::TableListBox *t)
 {
  if (table)
  {
   table->removeComponentListener(this);
  }
  
  table = t;

  if (table)
  {
   table->addComponentListener(this);
   
   table->setColour(juce::ListBox::backgroundColourId, juce::Colours::black);
   
   table->getHeader().removeAllColumns();
   table->getHeader().addColumn("", StatusColumn, 20, 20, 20,
                                juce::TableHeaderComponent::notResizableOrSortable);
   table->getHeader().addColumn("File Name", FilePathColumn, 300);
   table->getHeader().addColumn("Category", CategoryColumn, 150);
  }
  resizeCache();
 }
 
 void setSearchTerm(const juce::String &term)
 {
  dbAccess.search(term);
  clearCache();
  if (table) table->updateContent();
 }
 
 void resetSearch()
 {
  dbAccess.resetSearch();
  clearCache();
  if (table) table->updateContent();
 }
 
 void setFilterCategory(int categoryid)
 {
  dbAccess.filterByCategory(categoryid);
  clearCache();
  if (table) table->updateContent();
 }
 
 void clearFilterCategory()
 {
  dbAccess.resetCategoryFilter();
  clearCache();
  if (table) table->updateContent();
 }
 
 void componentMovedOrResized(juce::Component &component, bool wasMoved, bool wasResized) override
 {
  if (wasResized) resizeCache();
 }
 
 void sortOrderChanged(int newSortColumnId, bool isForwards) override
 {
  if (newSortColumnId == FilePathColumn) dbAccess.sortByPath(isForwards);
  else if (newSortColumnId == CategoryColumn) dbAccess.sortByCategory(isForwards);
  clearCache();
  if (table) table->updateContent();
 }
 
 virtual int getNumRows() override
 {
  return dbAccess.getNumRows();
 }
 
 virtual void paintRowBackground(juce::Graphics &g,
                                 int rowNumber,
                                 int width,
                                 int height,
                                 bool rowIsSelected) override
 {
  if (rowIsSelected) g.fillAll(juce::Colours::white.withBrightness(0.3));
  else if (rowNumber % 2) g.fillAll (juce::Colours::white.withBrightness(0.1));
 }
 
 virtual void paintCell(juce::Graphics &g,
                        int rowNumber,
                        int columnId,
                        int width,
                        int height,
                        bool rowIsSelected) override
 {
  auto row = touchCache(rowNumber);
  g.setColour(juce::Colours::white.withBrightness(0.8));
  g.setFont(font);
  switch (columnId)
  {
   case StatusColumn:
    if (row.fileError)
    {
     g.setColour(juce::Colours::red);
     g.drawText("!",
                2, 0, width - 4, height,
                juce::Justification::centredLeft, true);
    }
    break;
    
   case FilePathColumn:
    g.drawText(row.sampleFile.getFileName(),
               2, 0, width - 4, height,
               juce::Justification::centredLeft, true);
    break;
    
   case CategoryColumn:
    g.drawText(dbConn.categories.at(row.category),
               2, 0, width - 4, height,
               juce::Justification::centredLeft, true);
    break;
  }
  
//  g.setColour(juce::Colours::white.withBrightness(0.5));
//  g.fillRect(width - 1, 0, 1, height);
 }
 
 virtual juce::String getCellTooltip(int rowNumber, int columnId) override
 {
  auto row = touchCache(rowNumber);
  return row.sampleFile.getFullPathName();
 }
 
 virtual juce::Component* refreshComponentForCell(int rowNumber,
                                                  int columnId,
                                                  bool rowIsSelected,
                                                  juce::Component *existingComponent) override
 {
  return existingComponent;
 }
 
 virtual void cellClicked(int rowNumber,
                          int columnId,
                          const juce::MouseEvent &m) override
 {
  auto row = touchCache(rowNumber);
  
  
 }
};
