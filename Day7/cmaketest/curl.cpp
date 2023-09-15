#include <iostream>
#include <curl/curl.h>

int main (int argc, char * argv[])
{
    CURL *curl;
    CURLcode res;
    if (argc != 2)
    {
        std::cout << "Usage: file url \n";
        abort();
    }
    
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }    

    return 0;
}