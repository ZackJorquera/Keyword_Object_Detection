#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "MongoReaderWriter.h"

using namespace mongocxx;
using namespace bsoncxx;
using namespace std;

mongocxx::instance _instance{};
mongocxx::uri _uri;
mongocxx::client _client;

mongocxx::database _database;
mongocxx::collection _keywordCollection;

int ConnectToMongoDB()
{
	try
	{
		_uri = uri("mongodb://localhost:27017");
		_client = client(_uri);

		_database = _client["KeywordObjectDetection"];

		_keywordCollection = _database["KeyWords"];

		return 0;
	}
	catch(mongocxx::exception ex)
	{
		cout << ex.what() << endl;
		return -1;
	}
}

int WriteWordToMongo(std::string objName, int rangeVector[4]/*, Mat keyword*/)
{
	bsoncxx::stdx::optional<result::insert_one> _result;
	try
	{
		auto d = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc_value = d
			<< "objName" << objName.c_str()
			<< "range" << open_array
			<< rangeVector[0] << rangeVector[1] << rangeVector[2] << rangeVector[3]
			<< close_array
			//<< "keyword" << keyWord
			<< finalize;
		bsoncxx::document::view view = doc_value.view();

		_result = _keywordCollection.insert_one(view);
		return 0;
	}
	catch (bsoncxx::exception ex)
	{
		cout << ex.what() << endl;
		return -1;
	}
	catch (...)
	{
		cout << "unknown error" << endl;
		return -1;
	}
}