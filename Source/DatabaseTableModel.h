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
  int rowid;
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
   rowCache[cacheLine].rowid = dbAccess.getRowId();
   rowCache[cacheLine].sampleFile = juce::File(dbAccess.getPath());
   rowCache[cacheLine].category = dbAccess.getCategoryID();
  }
   
  return rowCache[cacheLine];
 }
 
 // Table properties
 juce::Font font {14.0f};
 
 
 
 SampleDatabaseConnection &dbConn;
 SampleDatabaseAccessor dbAccess;
 SampleDatabaseModifier dbMod;
 
 void triggerRedraw()
 {
  if (onRedrawRequired) onRedrawRequired();
 }
 
 //======================== PUBLIC MEMBERS =====================================
public:
 
 std::function<void (int)> onRowSelected;
 std::function<void ()> onRedrawRequired;
 
 DatabaseTableModel(SampleDatabaseConnection &_db) :
 dbConn(_db),
 dbAccess(_db),
 dbMod(_db)
 { }
 
 virtual ~DatabaseTableModel() override
 {
  connectToTable(nullptr);
 }
 
 void connectToTable(juce::TableListBox *t)
 {
  if (table)
  {
   table->removeComponentListener(this);
   table->setModel(nullptr);
  }
  
  table = t;

  if (table)
  {
   table->setModel(this);
   table->addComponentListener(this);
   table->setMultipleSelectionEnabled(true);
   
   table->setColour(juce::ListBox::backgroundColourId, juce::Colours::black);
   
   table->getHeader().removeAllColumns();
   table->getHeader().addColumn("", StatusColumn, 20, 20, 20,
                                juce::TableHeaderComponent::notResizableOrSortable);
   table->getHeader().addColumn("File Name", FilePathColumn, 300);
   table->getHeader().addColumn("Category", CategoryColumn, 150);
  }
  resizeCache();
 }
 
 void refreshTable()
 {
  clearCache();
  if (table) table->updateContent();
  triggerRedraw();
 }
 
 void setSearchTerm(const juce::String &term)
 {
  dbAccess.search(term);
  refreshTable();
 }
 
 void resetSearch()
 {
  dbAccess.resetSearch();
  refreshTable();
 }
 
 void setFilterCategory(int categoryid)
 {
  dbAccess.filterByCategory(categoryid);
  refreshTable();
 }
 
 void clearFilterCategory()
 {
  dbAccess.resetCategoryFilter();
  refreshTable();
 }
 
 void componentMovedOrResized(juce::Component &component, bool wasMoved, bool wasResized) override
 {
  if (wasResized) resizeCache();
 }
 
 void sortOrderChanged(int newSortColumnId, bool isForwards) override
 {
  if (newSortColumnId == FilePathColumn) dbAccess.sortByPath(isForwards);
  else if (newSortColumnId == CategoryColumn) dbAccess.sortByCategory(isForwards);
  refreshTable();
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
    if (row.category > 0) g.drawText(dbConn.categories.at(row.category),
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
  
  
  if (m.mods.isLeftButtonDown() && onRowSelected) onRowSelected(row.rowid);
  
  if (m.mods.isRightButtonDown())
  {
   juce::PopupMenu categoryMenu;
   for (auto i: dbConn.categories)
   {
    categoryMenu.addItem(100 + i.first, i.second);
   }
   
   juce::PopupMenu menu;
   menu.addItem(1, "Remove");
   menu.addSeparator();
   menu.addSubMenu("Change Category", categoryMenu);
   
   menu.showMenuAsync(juce::PopupMenu::Options(), [&, this, rowNumber](int result)
   {
    dbAccess.selectRow(rowNumber);
    if (result == 1)
    {
     deleteRows(table->getSelectedRows());
    }
    else if (result > 100)
    {
     int newCategory = result - 100;
     changeCategoryOfRows(table->getSelectedRows(), newCategory);
    }
   });
  }
 }
 
 std::vector<int> getRowIds(const juce::SparseSet<int> &rows)
 {
  std::vector<int> rowids;
  int rowCount = rows.size();
  rowids.reserve(rowCount);
  for (int i = 0; i < rowCount; ++i)
  {
   dbAccess.selectRow(rows[i]);
   rowids.push_back(dbAccess.getRowId());
  }
  
  return rowids;
 }
 
 void deleteRows(const juce::SparseSet<int> &rows)
 {
  auto rowids = getRowIds(rows);
  
  for (int i: rowids)
  {
   dbMod.deleteRow(i);
  }
  refreshTable();
 }
 
 void changeCategoryOfRows(const juce::SparseSet<int> &rows, int newCategory)
 {
  auto rowids = getRowIds(rows);

  for (int i: rowids)
  {
   dbMod.updateCategory(i, newCategory);
  }
  refreshTable();
 }
 
 void deleteKeyPressed(int lastRowSelected) override
 {
  if (table)
  {
   deleteRows(table->getSelectedRows());
   table->deselectAllRows();
  }
 }
};
