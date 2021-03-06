#include "dbc_store.h"
#include "dbc_db_loader.h"


namespace zq{

DBCStorageBase::DBCStorageBase(char const* fmt) : _fieldCount(0), _fileFormat(fmt), _dataTable(nullptr), _indexTableSize(0)
{
}

DBCStorageBase::~DBCStorageBase()
{
    delete[] _dataTable;
    for (char* strings : _stringPool)
        delete[] strings;
}

bool DBCStorageBase::Load(char const* path, char**& indexTable)
{
    indexTable = nullptr;

    DBCFileLoader dbc;
    // Check if load was sucessful, only then continue
    if (!dbc.Load(path, _fileFormat))
        return false;

    _fieldCount = dbc.GetCols();

    // load raw non-string data
    _dataTable = dbc.AutoProduceData(_fileFormat, _indexTableSize, indexTable);

    // load strings from dbc data
    if (char* stringBlock = dbc.AutoProduceStrings(_fileFormat, _dataTable))
        _stringPool.push_back(stringBlock);

    // error in dbc file at loading if NULL
    return indexTable != nullptr;
}

bool DBCStorageBase::LoadStringsFrom(char const* path, char** indexTable)
{
    // DBC must be already loaded using Load
    if (!indexTable)
        return false;

    DBCFileLoader dbc;
    // Check if load was successful, only then continue
    if (!dbc.Load(path, _fileFormat))
        return false;

    // load strings from another locale dbc data
    if (char* stringBlock = dbc.AutoProduceStrings(_fileFormat, _dataTable))
        _stringPool.push_back(stringBlock);

    return true;
}

void DBCStorageBase::LoadFromDB(char const* table, char const* format, char const* index, char**& indexTable)
{
    _stringPool.push_back(DBCDatabaseLoader(table, format, index, _fileFormat, _stringPool).Load(_indexTableSize, indexTable));
}


}