#include <iostream>
#include <string>
#include <curl/curl.h>  // Основной заголовочный файл libcurl
#include "json.hpp"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string post(std::string file_url){
    CURL *hnd = curl_easy_init();
    std::string response_data;
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://www.virustotal.com/api/v3/files");
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "x-apikey: 08f7a3b09cc9b1cddce55f9f9b6478d8f0740be639ecaa4a79999199c1f7a0e6");
    

    curl_mime *mime = curl_mime_init(hnd);
    curl_mimepart *part = curl_mime_addpart(mime);

    curl_mime_name(part, "file");  
    curl_mime_filedata(part, file_url.c_str()); 
    
    curl_easy_setopt(hnd, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);

    nlohmann::json parsed_json = nlohmann::json::parse(response_data);

    return parsed_json["data"]["id"].get<std::string>();
}

std::string get(std::string analysis_id){
    CURL *hnd = curl_easy_init();
    std::string response_data;

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response_data);
    std::string VTurl = "https://www.virustotal.com/api/v3/analyses/" + analysis_id;
    curl_easy_setopt(hnd, CURLOPT_URL, VTurl.c_str());
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "x-apikey: 08f7a3b09cc9b1cddce55f9f9b6478d8f0740be639ecaa4a79999199c1f7a0e6");
    
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);
    if (ret != CURLE_OK) 
        std::cerr << "CURL error: " << curl_easy_strerror(ret) << std::endl;
    return response_data;
}

int main() {
    std::string url;
    std::cout << "Enter filepath: ";
    std::cin >> url;
    std::string result = get(post(url));
    // std::cout << result;
    nlohmann::json parsed_json = nlohmann::json::parse(result);
    std::cout << parsed_json["data"]["attributes"]["stats"];
}


