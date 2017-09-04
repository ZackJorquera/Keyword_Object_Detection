#include <iostream>
#include "bsoncxx\builder\stream\document.hpp"
#include "bsoncxx\json.hpp"
#include "bsoncxx\exception\exception.hpp"
#include "mongocxx\exception\exception.hpp"
#include "mongocxx\client.hpp"
#include "mongocxx\instance.hpp"
#include "mongocxx\stdx.hpp"
#include "mongocxx\uri.hpp"
#include "mongocxx\instance.hpp"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

#pragma once

int ConnectToMongoDB();
int WriteWordToMongo(std::string, int[4]);//*, Mat keyword*/);
