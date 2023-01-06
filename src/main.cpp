
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

#include <cppcodec/base64_default_rfc4648.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Info.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json_t = nlohmann::json;


//--
//-- http
//--

std::string http_request(const std::string& authorization, const std::string& url, const std::string& accept, std::string& out_redirectUrl, bool verbose)
{
    out_redirectUrl.clear();

    std::stringstream stream;
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        std::list<std::string> header;
        header.push_back("accept: " + accept);
        header.push_back("Authorization: " + authorization);
        request.setOpt(curlpp::options::HttpHeader(header));
        request.setOpt(curlpp::options::Url(url));
        request.setOpt(curlpp::options::Verbose(verbose));
        request.setOpt(curlpp::options::WriteStream(&stream));
        request.setOpt(curlpp::options::FollowLocation(false));
        request.perform();

        const char* redirectUrl = curlpp::Info<CURLINFO_REDIRECT_URL, const char*>::get(request);
        if (redirectUrl != nullptr) { out_redirectUrl = std::string(redirectUrl); }
    }
    catch (curlpp::LogicError& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cout << e.what() << std::endl;
    }

    return stream.str();
}

json_t http_request_json(const std::string authorization, const std::string& url, std::string& out_redirectUrl, bool verbose)
{
    std::string result = http_request(authorization, url, "application/json;charset=UTF-8;qs=0.09", out_redirectUrl, verbose);
    if (result.empty()) { return json_t(); }
    return json_t::parse(result);
}

std::string http_request_octet(const std::string& authorization, const std::string& url, std::string& out_redirectUrl, bool verbose)
{
    return http_request(authorization, url, "application/octet-stream", out_redirectUrl, verbose);
}


//--
//-- authorization
//--

std::string onshape_apikeys_getauthorization(const std::string& access_key, const std::string& secret_key)
{
    return "Basic " + base64::encode(access_key + ":" + secret_key);
}

//-- todo: this needs a timeout so it does not potentially block forever
std::string onshape_oauth_getauthorization(const std::string& client_id)
{
    std::string eclient_id = curlpp::escape(client_id);
    const uint16_t localhostPort = 8293;

    std::string url = "https://oauth.onshape.com/oauth/authorize?response_type=code&client_id=" + eclient_id;
    url += "&redirect_uri=http://localhost:" + std::to_string(localhostPort);
    ShellExecute(NULL, L"open", std::wstring(url.begin(), url.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);

    std::string code;
    httplib::Server svr;
    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res)
    {
        res.set_content("You can close this window or tab now.", "text/plain");
        code = req.get_param_value("code");
        svr.stop();
    });
    svr.listen("localhost", localhostPort);

    if (code.empty()) { return ""; }
    printf("code [%s]\n", code.c_str());

    std::string bearer;
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        std::stringstream stream;

        std::list<std::string> header;
        header.push_back("Content-Type: application/x-www-form-urlencoded");
        request.setOpt(curlpp::options::HttpHeader(header));

        request.setOpt(curlpp::options::Url("https://oauth.onshape.com/oauth/token"));
        request.setOpt(curlpp::options::Verbose(false));
        request.setOpt(curlpp::options::WriteStream(&stream));

        std::string ecode = curlpp::escape(code);// "JoJUeTKTjBUvjoXAqdEauHsv");
        std::string eclient_secret = curlpp::escape("MBIFJA2JOYP2FN2J2AP7AQXJVVVPPVRKUODCM2DSMNUQUH7XM6QQ====");
        std::string body = "grant_type=authorization_code&code=" + ecode + "&client_id=" + eclient_id + "&client_secret=" + eclient_secret;
        body += "&redirect_uri=http://localhost:" + std::to_string(localhostPort);
        request.setOpt(new curlpp::options::PostFields(body));
        request.setOpt(new curlpp::options::PostFieldSize((long)body.length()));

        request.perform();

        json_t json = json_t::parse(stream.str());
        bearer = json.at("access_token");
    }
    catch (curlpp::LogicError& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cout << e.what() << std::endl;
    }
    if (bearer.empty()) { return ""; }

    printf("bearer [%s]\n", bearer.c_str());
    return "Bearer " + bearer;
}


//--
//-- export stls
//--

std::vector<std::pair<std::string, std::string>> onshape_listsolids(const std::string& authorization, const std::string& urldocument, const std::string& units, double angleTolerance, double chordTolerance)
{
    std::string urlbase = urldocument;
    urlbase.erase(0, std::string("https://cad.onshape.com/documents").length());
    urlbase = "https://cad.onshape.com/api/v5/parts/d" + urlbase;
    std::string url = urlbase + "?withThumbnails=true&includePropertyDefaults=true";

    std::string redirectUrl;
    json_t parts = http_request_json(authorization, url, redirectUrl, false);
    //printf("[%s]\n", parts.dump(true).c_str());
    std::vector<std::pair<std::string,std::string>> solids;
    for (auto& part : parts)
    {
        if (part.at("bodyType") == "solid")
        {
            std::string name = part.at("name");
            std::string partId = part.at("partId");;
            //printf("[%s] [%s]\n", name.c_str(), partId.c_str());
            solids.push_back({ name, partId });
        }
    }

    return solids;
}


bool onshape_export( const std::string& authorization, const std::string redirectUrl, bool binarymode, const std::string outputfilename )
{
    //printf("[%s]\n", redirectUrl.c_str());
    std::string anotherRedirectUrl;
    std::string stl = http_request_octet(authorization, redirectUrl, anotherRedirectUrl, false);
    if (stl.empty()) { return false; }
  
    printf("%s [%zd]\n", outputfilename.c_str(), stl.length());
    auto mode = std::ios_base::out;
    if (binarymode) { mode = std::ios::binary; }
    std::ofstream outfile(outputfilename, mode);
    outfile << stl;
    outfile.close();
    
    return true;
}

void onshape_exportstls_individual(const std::string& authorization, const std::string& urldocument, bool binarymode, const std::string& units, double angleTolerance, double chordTolerance)
{
    std::string urlbase = urldocument;
    urlbase.erase(0, std::string("https://cad.onshape.com/documents").length());
    urlbase = "https://cad.onshape.com/api/v5/partstudios/d" + urlbase;
    
    std::vector<std::pair<std::string, std::string>> solids;
    solids=onshape_listsolids(authorization, urldocument, units, angleTolerance, chordTolerance);    
       
    for(const auto& [name, partId] : solids)
    {
        bool zipped = false;
        std::string url = urlbase + "/stl?partIds=" + curlpp::escape(partId);
        url += "&mode=" + std::string( binarymode?"binary":"text");
        url += "&grouping=" + std::string( (!zipped)?"true":"false" );
        url += "&scale=" + std::to_string(1.0);
        url += "&units=" + units;
        url += "&angleTolerance=" + std::to_string(angleTolerance);
        url += "&chordTolerance=" + std::to_string(chordTolerance);

        std::string redirectUrl;
        http_request_json(authorization, url, redirectUrl, false);
        if (redirectUrl.empty() == true) { continue; }

        onshape_export( authorization, redirectUrl, binarymode, name+".stl" );
    }
}

void onshape_exportstls_combined(const std::string& authorization, const std::string& urldocument, bool binarymode, const std::string& units, double angleTolerance, double chordTolerance, bool zipped, const std::string outputfilename )
{
    std::string urlbase = urldocument;
    urlbase.erase(0, std::string("https://cad.onshape.com/documents").length());
    urlbase = "https://cad.onshape.com/api/v5/partstudios/d" + urlbase;

    std::vector<std::pair<std::string, std::string>> solids;
    solids = onshape_listsolids(authorization, urldocument, units, angleTolerance, chordTolerance);

    std::string partIds;
    for (const auto& [name, partId] : solids){ partIds += "," + partId; }
    partIds.erase(0, 1);

    std::string url = urlbase + "/stl?partIds=" + partIds;
    url += "&mode=" + std::string(binarymode ? "binary" : "text");
    url += "&grouping=" + std::string((!zipped) ? "true" : "false");
    url += "&scale=" + std::to_string(1.0);
    url += "&units=" + units;
    url += "&angleTolerance=" + std::to_string(angleTolerance);
    url += "&chordTolerance=" + std::to_string(chordTolerance);

    std::string redirectUrl;
    http_request_json(authorization, url, redirectUrl, false);
    if (redirectUrl.empty() == false)
    {
        onshape_export(authorization, redirectUrl, true, outputfilename);
    }
}







//--
//-- main
//--

int main(int argc, char* argv[])
{
    //-- API keys authorization (Option A)
    std::string access_key=getenv("ONSHAPE_ACCESS_KEY");
    std::string secret_key=getenv("ONSHAPE_SECRET_KEY");
    std::string authorization = onshape_apikeys_getauthorization(access_key, secret_key);

    //-- OAuth authorization (Option B)
    //std::string client_id= getenv("ONSHAPE_CLIENT_ID");
    //std::string authorization=onshape_oauth_getauthorization(client_id);

    std::string urldocument = "https://cad.onshape.com/documents/bd9401ba05b5d74bf12bb1a6/w/998c0499cee6d8fc96af5cbf/e/ecd24da899ea1976c376c6e7";
    bool binarymode = false;
    std::string units = "millimeter";
    double angleTolerance = 0.5236;
    double chordTolerance = 0.05;

    onshape_exportstls_individual(authorization, urldocument, binarymode, units, angleTolerance, chordTolerance );

    std::string zipoutputfilename = "partstudio.zip";
    onshape_exportstls_combined(authorization, urldocument, binarymode, units, angleTolerance, chordTolerance, true, zipoutputfilename );

    std::string stloutputfilename = "partstudio.stl";
    onshape_exportstls_combined(authorization, urldocument, binarymode, units, angleTolerance, chordTolerance, false, stloutputfilename);

    return EXIT_SUCCESS;
}



