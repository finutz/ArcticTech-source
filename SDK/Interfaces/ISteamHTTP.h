#pragma once
// Handle to a HTTP Request handle

#include <cstdint>

enum EHTTPMethod
{
	k_EHTTPMethodInvalid = 0,
	k_EHTTPMethodGET,
	k_EHTTPMethodHEAD,
	k_EHTTPMethodPOST,
	k_EHTTPMethodPUT,
	k_EHTTPMethodDELETE,
	k_EHTTPMethodOPTIONS,
	k_EHTTPMethodPATCH,

	// The remaining HTTP methods are not yet supported, per rfc2616 section 5.1.1 only GET and HEAD are required for 
	// a compliant general purpose server.  We'll likely add more as we find uses for them.

	// k_EHTTPMethodTRACE,
	// k_EHTTPMethodCONNECT
};


// HTTP Status codes that the server can send in response to a request, see rfc2616 section 10.3 for descriptions
// of each of these.
enum EHTTPStatusCode
{
	// Invalid status code (this isn't defined in HTTP, used to indicate unset in our code)
	k_EHTTPStatusCodeInvalid = 0,

	// Informational codes
	k_EHTTPStatusCode100Continue = 100,
	k_EHTTPStatusCode101SwitchingProtocols = 101,

	// Success codes
	k_EHTTPStatusCode200OK = 200,
	k_EHTTPStatusCode201Created = 201,
	k_EHTTPStatusCode202Accepted = 202,
	k_EHTTPStatusCode203NonAuthoritative = 203,
	k_EHTTPStatusCode204NoContent = 204,
	k_EHTTPStatusCode205ResetContent = 205,
	k_EHTTPStatusCode206PartialContent = 206,

	// Redirection codes
	k_EHTTPStatusCode300MultipleChoices = 300,
	k_EHTTPStatusCode301MovedPermanently = 301,
	k_EHTTPStatusCode302Found = 302,
	k_EHTTPStatusCode303SeeOther = 303,
	k_EHTTPStatusCode304NotModified = 304,
	k_EHTTPStatusCode305UseProxy = 305,
	//k_EHTTPStatusCode306Unused =				306, (used in old HTTP spec, now unused in 1.1)
	k_EHTTPStatusCode307TemporaryRedirect = 307,

	// Error codes
	k_EHTTPStatusCode400BadRequest = 400,
	k_EHTTPStatusCode401Unauthorized = 401, // You probably want 403 or something else. 401 implies you're sending a WWW-Authenticate header and the client can sent an Authorization header in response.
	k_EHTTPStatusCode402PaymentRequired = 402, // This is reserved for future HTTP specs, not really supported by clients
	k_EHTTPStatusCode403Forbidden = 403,
	k_EHTTPStatusCode404NotFound = 404,
	k_EHTTPStatusCode405MethodNotAllowed = 405,
	k_EHTTPStatusCode406NotAcceptable = 406,
	k_EHTTPStatusCode407ProxyAuthRequired = 407,
	k_EHTTPStatusCode408RequestTimeout = 408,
	k_EHTTPStatusCode409Conflict = 409,
	k_EHTTPStatusCode410Gone = 410,
	k_EHTTPStatusCode411LengthRequired = 411,
	k_EHTTPStatusCode412PreconditionFailed = 412,
	k_EHTTPStatusCode413RequestEntityTooLarge = 413,
	k_EHTTPStatusCode414RequestURITooLong = 414,
	k_EHTTPStatusCode415UnsupportedMediaType = 415,
	k_EHTTPStatusCode416RequestedRangeNotSatisfiable = 416,
	k_EHTTPStatusCode417ExpectationFailed = 417,
	k_EHTTPStatusCode4xxUnknown = 418, // 418 is reserved, so we'll use it to mean unknown
	k_EHTTPStatusCode429TooManyRequests = 429,

	// Server error codes
	k_EHTTPStatusCode500InternalServerError = 500,
	k_EHTTPStatusCode501NotImplemented = 501,
	k_EHTTPStatusCode502BadGateway = 502,
	k_EHTTPStatusCode503ServiceUnavailable = 503,
	k_EHTTPStatusCode504GatewayTimeout = 504,
	k_EHTTPStatusCode505HTTPVersionNotSupported = 505,
	k_EHTTPStatusCode5xxUnknown = 599,
};

typedef uint32_t HTTPRequestHandle;
#define INVALID_HTTPREQUEST_HANDLE		0

typedef uint32_t HTTPCookieContainerHandle;
#define INVALID_HTTPCOOKIE_HANDLE		0

typedef uint64_t SteamAPICall_t;
const SteamAPICall_t k_uAPICallInvalid = 0x0;

//-----------------------------------------------------------------------------
// Purpose: interface to http client
//-----------------------------------------------------------------------------
class ISteamHTTP
{
public:

	// Initializes a new HTTP request, returning a handle to use in further operations on it.  Requires
	// the method (GET or POST) and the absolute URL for the request.  Both http and https are supported,
	// so this string must start with http:// or https:// and should look like http://store.steampowered.com/app/250/ 
	// or such.
	virtual HTTPRequestHandle CreateHTTPRequest(EHTTPMethod eHTTPRequestMethod, const char* pchAbsoluteURL) = 0;

	// Set a context value for the request, which will be returned in the HTTPRequestCompleted_t callback after
	// sending the request.  This is just so the caller can easily keep track of which callbacks go with which request data.
	virtual bool SetHTTPRequestContextValue(HTTPRequestHandle hRequest, uint64_t ulContextValue) = 0;

	// Set a timeout in seconds for the HTTP request, must be called prior to sending the request.  Default
	// timeout is 60 seconds if you don't call this.  Returns false if the handle is invalid, or the request
	// has already been sent.
	virtual bool SetHTTPRequestNetworkActivityTimeout(HTTPRequestHandle hRequest, uint32_t unTimeoutSeconds) = 0;

	// Set a request header value for the request, must be called prior to sending the request.  Will 
	// return false if the handle is invalid or the request is already sent.
	virtual bool SetHTTPRequestHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, const char* pchHeaderValue) = 0;

	// Set a GET or POST parameter value on the request, which is set will depend on the EHTTPMethod specified
	// when creating the request.  Must be called prior to sending the request.  Will return false if the 
	// handle is invalid or the request is already sent.
	virtual bool SetHTTPRequestGetOrPostParameter(HTTPRequestHandle hRequest, const char* pchParamName, const char* pchParamValue) = 0;

	// Sends the HTTP request, will return false on a bad handle, otherwise use SteamCallHandle to wait on
	// asynchronous response via callback.
	//
	// Note: If the user is in offline mode in Steam, then this will add a only-if-cached cache-control 
	// header and only do a local cache lookup rather than sending any actual remote request.
	virtual bool SendHTTPRequest(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;

	// Sends the HTTP request, will return false on a bad handle, otherwise use SteamCallHandle to wait on
	// asynchronous response via callback for completion, and listen for HTTPRequestHeadersReceived_t and 
	// HTTPRequestDataReceived_t callbacks while streaming.
	virtual bool SendHTTPRequestAndStreamResponse(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;

	// Defers a request you have sent, the actual HTTP client code may have many requests queued, and this will move
	// the specified request to the tail of the queue.  Returns false on invalid handle, or if the request is not yet sent.
	virtual bool DeferHTTPRequest(HTTPRequestHandle hRequest) = 0;

	// Prioritizes a request you have sent, the actual HTTP client code may have many requests queued, and this will move
	// the specified request to the head of the queue.  Returns false on invalid handle, or if the request is not yet sent.
	virtual bool PrioritizeHTTPRequest(HTTPRequestHandle hRequest) = 0;

	// Checks if a response header is present in a HTTP response given a handle from HTTPRequestCompleted_t, also 
	// returns the size of the header value if present so the caller and allocate a correctly sized buffer for
	// GetHTTPResponseHeaderValue.
	virtual bool GetHTTPResponseHeaderSize(HTTPRequestHandle hRequest, const char* pchHeaderName, uint32_t* unResponseHeaderSize) = 0;

	// Gets header values from a HTTP response given a handle from HTTPRequestCompleted_t, will return false if the
	// header is not present or if your buffer is too small to contain it's value.  You should first call 
	// BGetHTTPResponseHeaderSize to check for the presence of the header and to find out the size buffer needed.
	virtual bool GetHTTPResponseHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, uint8_t* pHeaderValueBuffer, uint32_t unBufferSize) = 0;

	// Gets the size of the body data from a HTTP response given a handle from HTTPRequestCompleted_t, will return false if the 
	// handle is invalid.
	virtual bool GetHTTPResponseBodySize(HTTPRequestHandle hRequest, uint32_t* unBodySize) = 0;

	// Gets the body data from a HTTP response given a handle from HTTPRequestCompleted_t, will return false if the 
	// handle is invalid or is to a streaming response, or if the provided buffer is not the correct size.  Use BGetHTTPResponseBodySize first to find out
	// the correct buffer size to use.
	virtual bool GetHTTPResponseBodyData(HTTPRequestHandle hRequest, uint8_t* pBodyDataBuffer, uint32_t unBufferSize) = 0;

	// Gets the body data from a streaming HTTP response given a handle from HTTPRequestDataReceived_t. Will return false if the 
	// handle is invalid or is to a non-streaming response (meaning it wasn't sent with SendHTTPRequestAndStreamResponse), or if the buffer size and offset 
	// do not match the size and offset sent in HTTPRequestDataReceived_t.
	virtual bool GetHTTPStreamingResponseBodyData(HTTPRequestHandle hRequest, uint32_t cOffset, uint8_t* pBodyDataBuffer, uint32_t unBufferSize) = 0;

	// Releases an HTTP response handle, should always be called to free resources after receiving a HTTPRequestCompleted_t
	// callback and finishing using the response.
	virtual bool ReleaseHTTPRequest(HTTPRequestHandle hRequest) = 0;

	// Gets progress on downloading the body for the request.  This will be zero unless a response header has already been
	// received which included a content-length field.  For responses that contain no content-length it will report
	// zero for the duration of the request as the size is unknown until the connection closes.
	virtual bool GetHTTPDownloadProgressPct(HTTPRequestHandle hRequest, float* pflPercentOut) = 0;

	// Sets the body for an HTTP Post request.  Will fail and return false on a GET request, and will fail if POST params
	// have already been set for the request.  Setting this raw body makes it the only contents for the post, the pchContentType
	// parameter will set the content-type header for the request so the server may know how to interpret the body.
	virtual bool SetHTTPRequestRawPostBody(HTTPRequestHandle hRequest, const char* pchContentType, uint8_t* pubBody, uint32_t unBodyLen) = 0;

	// Creates a cookie container handle which you must later free with ReleaseCookieContainer().  If bAllowResponsesToModify=true
	// than any response to your requests using this cookie container may add new cookies which may be transmitted with
	// future requests.  If bAllowResponsesToModify=false than only cookies you explicitly set will be sent.  This API is just for
	// during process lifetime, after steam restarts no cookies are persisted and you have no way to access the cookie container across
	// repeat executions of your process.
	virtual HTTPCookieContainerHandle CreateCookieContainer(bool bAllowResponsesToModify) = 0;

	// Release a cookie container you are finished using, freeing it's memory
	virtual bool ReleaseCookieContainer(HTTPCookieContainerHandle hCookieContainer) = 0;

	// Adds a cookie to the specified cookie container that will be used with future requests.
	virtual bool SetCookie(HTTPCookieContainerHandle hCookieContainer, const char* pchHost, const char* pchUrl, const char* pchCookie) = 0;

	// Set the cookie container to use for a HTTP request
	virtual bool SetHTTPRequestCookieContainer(HTTPRequestHandle hRequest, HTTPCookieContainerHandle hCookieContainer) = 0;

	// Set the extra user agent info for a request, this doesn't clobber the normal user agent, it just adds the extra info on the end
	virtual bool SetHTTPRequestUserAgentInfo(HTTPRequestHandle hRequest, const char* pchUserAgentInfo) = 0;

	// Set that https request should require verified SSL certificate via machines certificate trust store
	virtual bool SetHTTPRequestRequiresVerifiedCertificate(HTTPRequestHandle hRequest, bool bRequireVerifiedCertificate) = 0;

	// Set an absolute timeout on the HTTP request, this is just a total time timeout different than the network activity timeout
	// which can bump everytime we get more data
	virtual bool SetHTTPRequestAbsoluteTimeoutMS(HTTPRequestHandle hRequest, uint32_t unMilliseconds) = 0;

	// Check if the reason the request failed was because we timed it out (rather than some harder failure)
	virtual bool GetHTTPRequestWasTimedOut(HTTPRequestHandle hRequest, bool* pbWasTimedOut) = 0;
};