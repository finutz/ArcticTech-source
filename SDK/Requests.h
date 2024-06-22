#pragma once

#include <string>
#include <vector>

#include "Interfaces/ISteamHTTP.h"

struct HttpHeader {
	std::string key;
	std::string value;
};

class CRequests {
public:
	std::string Create(EHTTPMethod method, std::string url, std::vector<HttpHeader> headers = {});
};

extern CRequests* Requests;