#include "Requests.h"
#include "Interfaces.h"

CRequests* Requests = new CRequests;

std::string CRequests::Create(EHTTPMethod method, std::string url, std::vector<HttpHeader> headers) {
	HTTPRequestHandle handle = SteamHTTP->CreateHTTPRequest(method, url.c_str());
	for (auto& header : headers)
		SteamHTTP->SetHTTPRequestHeaderValue(handle, header.key.c_str(), header.value.c_str());

	if (!SteamHTTP->SendHTTPRequest(handle, 0))
		return "";

	bool timed_out = false;
	int iters = 0;
	uint32_t data_size = 0;
	while (!SteamHTTP->GetHTTPResponseBodySize(handle, &data_size)) {
		if (iters > 100) {
			timed_out = true;
			break;
		}

		Sleep(10);
		iters++;
	}

	if (timed_out)
		return "timed out";

	uint8_t* data = new uint8_t[data_size];
	SteamHTTP->GetHTTPResponseBodyData(handle, data, data_size);

	std::string result((char*)data, data_size);

	delete[] data;

	return result;
}