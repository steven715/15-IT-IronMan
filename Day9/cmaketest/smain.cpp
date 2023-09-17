#include <string>
#include <vector>
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using namespace std;

// Create a new collection in the given database.
void createCollection(mongocxx::database& db, const string& collectionName)
{
	db.create_collection(collectionName);
}
// Create a document from the given key-value pairs.
bsoncxx::document::value createDocument(const vector<pair<string, string>>& keyValues)
{
	bsoncxx::builder::stream::document document{};
	for (auto& keyValue : keyValues)
	{
		document << keyValue.first << keyValue.second;
	}
	return document << bsoncxx::builder::stream::finalize;
}
// Insert a document into the given collection.
void insertDocument(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	collection.insert_one(document.view());
}

// Print the contents of the given collection.
void printCollection(mongocxx::collection& collection)
{
	// Check if collection is empty.
	if (collection.count_documents({}) == 0)
	{
		cout << "Collection is empty." << endl;
		return;
	}
	auto cursor = collection.find({});
	for (auto&& doc : cursor)
	{
		cout << bsoncxx::to_json(doc) << endl;
	}
}
// Find the document with given key-value pair.
void findDocument(mongocxx::collection& collection, const string& key, const string& value)
{
	// Create the query filter
	auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;
	//Add query filter argument in find
	auto cursor = collection.find({ filter });
	for (auto&& doc : cursor)
	{
          cout << bsoncxx::to_json(doc) << endl;
	}
}

int main()
{ 
  try
  {
    // Create an instance.
    mongocxx::instance inst{};
  
    // Replace the connection string with your MongoDB deployment's connection string.
    const auto uri = mongocxx::uri{"mongodb://172.17.0.1:27017"};
  
    // Set the version of the Stable API on the client.
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
    client_options.server_api_opts(api);
  
    // Setup the connection and get a handle on the "admin" database.
    mongocxx::client conn{ uri, client_options };
    mongocxx::database db = conn["admin"];
    
    // Ping the database.
    const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
    db.run_command(ping_cmd.view());
    std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;

    // create data
    createCollection(db, "mongotest");
    auto collection = db.collection("mongotest");
    vector<pair<string, string>> datas = { {"hello","world"} };
    auto documnet = createDocument(datas);
    insertDocument(collection, documnet);

    // read data
    printCollection(collection);
  }
  catch (const std::exception& e) 
  {
    // Handle errors.
    std::cout<< "Exception: " << e.what() << std::endl;
  }

  return 0;
}
