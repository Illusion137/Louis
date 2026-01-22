#include <cstdint>
#include <glaze/glaze.hpp>
#include <cpr/cpr.h>
#include "cpr/body.h"
#include "cpr/response.h"
#include "sll.hpp"
#include "utils/cache.hpp"

LL_NAMESPACE_BEGIN

enum ResponseStatus: uint32_t {
    RESPONSE_STATUS_CONTINUE = 100,
    RESPONSE_STATUS_SWITCHING_PROTOCOLS = 101,
    RESPONSE_STATUS_PROCESSING = 102,
    RESPONSE_STATUS_EARLY_HINTS = 103,
    RESPONSE_STATUS_OK = 200,
    RESPONSE_STATUS_CREATED = 201,
    RESPONSE_STATUS_ACCEPTED = 202,
    RESPONSE_STATUS_NON_AUTHORITATIVE_INFORMATION = 203,
    RESPONSE_STATUS_NO_CONTENT = 204,
    RESPONSE_STATUS_RESET_CONTENT = 205,
    RESPONSE_STATUS_PARTIAL_CONTENT = 206,
    RESPONSE_STATUS_MULTI_STATUS = 207,
    RESPONSE_STATUS_ALREADY_REPORTED = 208,
    RESPONSE_STATUS_THIS_IS_FINE__APACHE_WEB_SERVER_ = 218,
    RESPONSE_STATUS_IM_USED = 226,
    RESPONSE_STATUS_MULTIPLE_CHOICES = 300,
    RESPONSE_STATUS_MOVED_PERMANENTLY = 301,
    RESPONSE_STATUS_FOUND = 302,
    RESPONSE_STATUS_SEE_OTHER = 303,
    RESPONSE_STATUS_NOT_MODIFIED = 304,
    RESPONSE_STATUS_SWITCH_PROXY = 306,
    RESPONSE_STATUS_TEMPORARY_REDIRECT = 307,
    RESPONSE_STATUS_RESUME_INCOMPLETE = 308,
    RESPONSE_STATUS_BAD_REQUEST = 400,
    RESPONSE_STATUS_UNAUTHORIZED = 401,
    RESPONSE_STATUS_PAYMENT_REQUIRED = 402,
    RESPONSE_STATUS_FORBIDDEN = 403,
    RESPONSE_STATUS_NOT_FOUND = 404,
    RESPONSE_STATUS_METHOD_NOT_ALLOWED = 405,
    RESPONSE_STATUS_NOT_ACCEPTABLE = 406,
    RESPONSE_STATUS_PROXY_AUTHENTICATION_REQUIRED = 407,
    RESPONSE_STATUS_REQUEST_TIMEOUT = 408,
    RESPONSE_STATUS_CONFLICT = 409,
    RESPONSE_STATUS_GONE = 410,
    RESPONSE_STATUS_LENGTH_REQUIRED = 411,
    RESPONSE_STATUS_PRECONDITION_FAILED = 412,
    RESPONSE_STATUS_REQUEST_ENTITY_TOO_LARGE = 413,
    RESPONSE_STATUS_REQUEST_URI_TOO_LONG = 414,
    RESPONSE_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
    RESPONSE_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    RESPONSE_STATUS_EXPECTATION_FAILED = 417,
    RESPONSE_STATUS_IM_A_TEAPOT = 418,
    RESPONSE_STATUS_PAGE_EXPIRED_LARAVEL_FRAMEWORK = 419,
    RESPONSE_STATUS_METHOD_FAILURE_SPRING_FRAMEWORK = 420,
    RESPONSE_STATUS_MISDIRECTED_REQUEST = 421,
    RESPONSE_STATUS_UNPROCESSABLE_ENTITY = 422,
    RESPONSE_STATUS_LOCKED = 423,
    RESPONSE_STATUS_FAILED_DEPENDENCY = 424,
    RESPONSE_STATUS_UPGRADE_REQUIRED = 426,
    RESPONSE_STATUS_PRECONDITION_REQUIRED = 428,
    RESPONSE_STATUS_TOO_MANY_REQUESTS = 429,
    RESPONSE_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    RESPONSE_STATUS_LOGIN_TIME_OUT = 440,
    RESPONSE_STATUS_CONNECTION_CLOSED_WITHOUT_RESPONSE = 444,
    RESPONSE_STATUS_RETRY_WITH = 449,
    RESPONSE_STATUS_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS = 450,
    RESPONSE_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    RESPONSE_STATUS_REQUEST_HEADER_TOO_LARGE = 494,
    RESPONSE_STATUS_SSL_CERTIFICATE_ERROR = 495,
    RESPONSE_STATUS_SSL_CERTIFICATE_REQUIRED = 496,
    RESPONSE_STATUS_HTTP_REQUEST_SENT_TO_HTTPS_PORT = 497,
    RESPONSE_STATUS_INVALID_TOKEN_ESRI = 498,
    RESPONSE_STATUS_CLIENT_CLOSED_REQUEST = 499,
    RESPONSE_STATUS_INTERNAL_SERVER_ERROR = 500,
    RESPONSE_STATUS_NOT_IMPLEMENTED = 501,
    RESPONSE_STATUS_BAD_GATEWAY = 502,
    RESPONSE_STATUS_SERVICE_UNAVAILABLE = 503,
    RESPONSE_STATUS_GATEWAY_TIMEOUT = 504,
    RESPONSE_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
    RESPONSE_STATUS_VARIANT_ALSO_NEGOTIATES = 506,
    RESPONSE_STATUS_INSUFFICIENT_STORAGE = 507,
    RESPONSE_STATUS_LOOP_DETECTED = 508,
    RESPONSE_STATUS_BANDWIDTH_LIMIT_EXCEEDED = 509,
    RESPONSE_STATUS_NOT_EXTENDED = 510,
    RESPONSE_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,
    RESPONSE_STATUS_UNKNOWN_ERROR = 520,
    RESPONSE_STATUS_WEB_SERVER_IS_DOWN = 521,
    RESPONSE_STATUS_CONNECTION_TIMED_OUT = 522,
    RESPONSE_STATUS_ORIGIN_IS_UNREACHABLE = 523,
    RESPONSE_STATUS_A_TIMEOUT_OCCURRED = 524,
    RESPONSE_STATUS_SSL_HANDSHAKE_FAILED = 525,
    RESPONSE_STATUS_INVALID_SSL_CERTIFICATE = 526,
    RESPONSE_STATUS_RAILGUN_LISTENER_TO_ORIGIN_ERROR = 527,
    RESPONSE_STATUS_ORIGIN_DNS_ERROR = 530,
    RESPONSE_STATUS_NETWORK_READ_TIMEOUT_ERROR = 598,
    RESPONSE_STATUS_JSON_ERROR = UINT32_MAX,
};
template<typename T> using ResponseValue = std::expected<T, ResponseStatus>;

class RozResponse {
    private:
        friend cpr::MultiPerform;
    public:
        std::shared_ptr<cpr::CurlHolder> curl_{nullptr};
        long status_code{};
        std::string text{};
        cpr::Header header{};
        cpr::Url url{};
        double elapsed{};
        cpr::Cookies cookies{};
        cpr::Error error{};
        std::string raw_header{};
        std::string status_line{};
        std::string reason{};
        cpr::cpr_off_t uploaded_bytes{};
        cpr::cpr_off_t downloaded_bytes{};
        long redirect_count{};
        std::string primary_ip{};
        std::uint16_t primary_port{};
};

static inline cpr::Response get_default_cpr_response(const char *url, std::string text){
    const RozResponse response{
        .url = url,
        .text = text,
        .header = {},
        .cookies = {},
        .status_code = 200,
        .status_line = "",
        .reason = "OK",
        .raw_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n",
        .error = cpr::Error(),
        .elapsed = 0.00
    };
    static_assert(sizeof(RozResponse) == sizeof(cpr::Response), "RozResponse should reflect cpr::Response");
    return *(cpr::Response*)(void*)(&response);
}

static inline ResponseValue<cpr::Response> rozfetch_get_response(const char *url){
    constexpr size_t expires_ms = 1000 * 60 * 60;
    const auto cache_payload = url;
    const auto cache_response = cache::check_cache_string(cache_payload, expires_ms);
    if(cache_response){
        LL_INFO("Successfully fetched {} from 'fs::cache'", cache_payload);
        return get_default_cpr_response(url, cache_response.value());
    }

    const cpr::Response response = cpr::Get(cpr::Url{url});

    if(response.status_code < 200 || response.status_code > 299){
        LL_WARN("Failed to fetch {} : code {} - {}\nreason - {}", url, response.status_code, response.status_line, response.reason);
        return std::unexpected(static_cast<ResponseStatus>(response.status_code));
    }
    LL_INFO("Successfully fetched {} in {}ms", url, response.elapsed);
    cache::insert_cache(cache_payload, response.text);
    return response;
}

template<glz::write_supported<glz::JSON> T>
static inline ResponseValue<cpr::Response> rozfetch_post_response(const char *url, const T &body){
    const glz::expected<std::string, glz::error_ctx> payload = glz::write_json(body);
    if(!payload){
        LL_WARN("Failed to serialize payload for {}", url);
        return std::unexpected(ResponseStatus::RESPONSE_STATUS_JSON_ERROR);
    }

    constexpr size_t expires_ms = 1000 * 60 * 60;
    const auto cache_payload = url + payload.value();
    const auto cache_response = cache::check_cache_string(cache_payload, expires_ms);
    if(cache_response){
        LL_INFO("Successfully fetched {} from 'fs::cache'", cache_payload);
        return get_default_cpr_response(url, cache_response.value());
    }

    const cpr::Response response = cpr::Post(
        cpr::Url{url}, 
        cpr::Body{payload.value()},
        cpr::Header{{
            {"accept", "*/*"}, {"accept-language", "en-US,en;q=0.9"}, {"cache-control", "no-cache"}, {"content-type", "application/x-www-form-urlencoded"}, {"pragma", "no-cache"}, {"sec-ch-ua", "\"Chromium\";v=\"142\", \"Google Chrome\";v=\"142\", \"Not_A Brand\";v=\"99\""}, {"sec-ch-ua-mobile", "?0"}, {"sec-ch-ua-platform", "\"macOS\""}, {"sec-fetch-dest", "empty"}, {"sec-fetch-mode", "cors"}, {"sec-fetch-site", "same-origin"}, {"cookie", "_ga=GA1.2.530191851.1744055409; _uetvid=8319bc9013e911f09a91b11ca7dd1147; _clck=1g0pb57%7C2%7Cfuz%7C0%7C1927; _ga_FWLBPVRPRP=GS1.1.1744399952.2.1.1744400013.60.0.0; _ga_3Z09XZBPK5=GS1.1.1744399952.2.1.1744400013.0.0.1648161236; _ce.s=v~6d5f53747281e33d22943bfb66e5ab45a84c77d1~lcw~1744400013759~vir~returning~lva~1744399953240~vpv~1~v11.cs~242280~v11.s~b7497890-170b-11f0-8298-73509800c339~v11.sla~1744400013769~gtrk.la~m9d6q8tt~v11.send~1744400013759~lcw~1744400013769; _ga_YVBSE3F7LM=GS1.2.1744400147.2.1.1744400325.0.0.0; SignOnDefault=; ps_theme=node:SA portal:EMPLOYEE theme_id:N__NAU_FLUID css:N__BRAND_CLASSIC_TEMPLATE css_f:N__BRAND_FLUID_TEMPLATE accessibility:N macroset:N__DEFAULT_MACROSET formfactor:3 piamode:2; in._km_id-1a06d584b8d0a14abcbe6383c41e19293=YGSuDEnwoCfHGVrvuGpOMzrBbXsD6GN5; in._km_user_name-1a06d584b8d0a14abcbe6383c41e19293=Posh Narwhal; in._km_lead_collection-1a06d584b8d0a14abcbe6383c41e19293=false; IOS_FULLSCREEN=0; citrix_ns_id=6D66sOUay40Ni3gNkhEVtXpHDSU0000; _km_id-1a06d584b8d0a14abcbe6383c41e19293=gpLkZHzPCW4Dt4cQjTuSHQ3t75xog18Z; _km_user_name-1a06d584b8d0a14abcbe6383c41e19293=Posh Goldfish; _km_lead_collection-1a06d584b8d0a14abcbe6383c41e19293=false; ExpirePage=https://www.peoplesoft.nau.edu/psc/ps92prcs/; psprwebc1-8090-PORTAL-PSJSESSIONID=99auQ6_fYOl2kttHKVNvVMTDA2DPEtx5!1703492203; lcsrftoken=dPJw98vHKZ00cEEGBIXd5KRW16SPVdbBZz+/OX8Jl7Y=; psprwebn2-8090-PORTAL-PSJSESSIONID=6t6vRldNwgjbZNIZY5ejVqpGL5DUwmH-!946440318; psprwebc4-8090-PORTAL-PSJSESSIONID=l87Tca2VI99COSvVIpvw4cw6eGeMjowr!721681590; psprwebn3-8090-PORTAL-PSJSESSIONID=K9Xboh2GMSSUVbxDaHvar7j9FW6hlvoO!1789186573; PS_LOGINLIST=https://www.peoplesoft.nau.edu/ps92prcs; PS_TOKEN=qgAAAAQDAgEBAAAAvAIAAAAAAAAsAAAABABTaGRyAk4AfQg4AC4AMQAwABS/zHz+TEe9L61jUcagSgL96KDR/2oAAAAFAFNkYXRhXnicJYpbDkBADEWPR6xA7GLEeMYChj+RQXxaiZ1ZnDu06Tm3TW8gTeIokp+Yr/IFx8nBJnu1YyZzLFKx6jqxcxHSSC172ppKscNgZaPNfuwpaRjEP4evMIPIC31NDdc=; PS_TokenSite=https://www.peoplesoft.nau.edu/psc/ps92prcs/?psprwebn3-8090-PORTAL-PSJSESSIONID; PS_LASTSITE=https://www.peoplesoft.nau.edu/psc/ps92prcs/; PS_DEVICEFEATURES=width:1470 height:956 pixelratio:2 touch:0 geolocation:1 websockets:1 webworkers:1 datepicker:1 dtpicker:1 timepicker:1 dnd:1 sessionstorage:1 localstorage:1 history:1 canvas:1 svg:1 postmessage:1 hc:0 maf:0; psback=%22%22url%22%3A%22https%3A%2F%2Fwww.peoplesoft.nau.edu%2Fpsc%2Fps92prcs%2FEMPLOYEE%2FSA%2Fc%2FCOMMUNITY_ACCESS.CLASS_SEARCH.GBL%3FPage%3DSSR_CLSRCH_ENTRY%26inst%3DNAU00%26open%3DN%26search%3Dtrue%26utm_source%3Dchatgpt.com%26%22%20%22label%22%3A%22Class%20Search%22%20%22origin%22%3A%22PIA%22%20%22layout%22%3A%220%22%20%22refurl%22%3A%22https%3A%2F%2Fwww.peoplesoft.nau.edu%2Fpsc%2Fps92prcs%2FEMPLOYEE%2FSA%22%22; PS_TOKENEXPIRE=1_Dec_2025_20:43:10_GMT"}, {"Referer", "https://www.peoplesoft.nau.edu/psc/ps92prcs/EMPLOYEE/SA/c/COMMUNITY_ACCESS.CLASS_SEARCH.GBL?Page=SSR_CLSRCH_ENTRY&inst=NAU00&open=N&search=true&utm_source=chatgpt.com&"}, 
        }}
        );

    if(response.status_code < 200 || response.status_code > 299){
        LL_WARN("Failed to fetch {} : code {} - {}\nreason - {}", url, response.status_code, response.status_line, response.reason);
        return std::unexpected(static_cast<ResponseStatus>(response.status_code));
    }
    LL_INFO("Successfully fetched {} in {}ms", url, response.elapsed);
    cache::insert_cache(cache_payload, response.text);
    return response;
}

template<class T> static inline ResponseValue<T> rozfetch_get_json(const char *url){
    const ResponseValue<cpr::Response> response = rozfetch_get_response(url);
    if(!response.has_value()) return std::unexpected(response.error());
    T parsed;
    const auto json_err = glz::read_json<T>(parsed, response.value().text);

    if(json_err) {
        LL_LOG_ERROR("Failed to read JSON: {}", glz::format_error(json_err));
        return std::unexpected(ResponseStatus::RESPONSE_STATUS_JSON_ERROR);
    }
    return parsed;
}

static inline ResponseValue<std::string> rozfetch_get_text(const char *url){
    const ResponseValue<cpr::Response> response = rozfetch_get_response(url);
    if(!response.has_value()) return std::unexpected(response.error());
    return response.value().text;
}
LL_NAMESPACE_END