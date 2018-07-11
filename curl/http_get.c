/*************************************************************************
  > File Name: client_sample.c
  > Author: lin_engier
  > Mail: linengier@126.com 
  > Created Time: 2017年01月12日 星期四 16时14分20秒
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#define SKIP_PEER_VERIFICATION 1
#define SKIP_HOSTNAME_VERIFICATION 1

typedef struct config_storage
{
    char * data;
    int len;
}config_storage;

char *test_url = "https://127.0.0.1:8000/config?name=vpn&pid=1234";


static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *argv)
{
    int len; 
    config_storage * storage = (config_storage *)argv;
    len = size*nmemb;
    if(len < storage->len)
        storage->len = len;
    memcpy(storage->data, buffer, storage->len);
    return storage->len;
}

int config_https_get(char * url, char *data, int len)
{
    CURL *curl;
    int ret = -1;
    CURLcode res;
    config_storage config;
    if(!url || !data)
        return -1;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
#ifdef SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */ 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
#ifdef SKIP_HOSTNAME_VERIFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */ 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        config.data = data;
        config.len = len;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &config);
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        else
            ret = config.len;
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return ret;
}


int main()
{
    char buf[1024]={0};
    int ret;
    ret = config_https_get(test_url, buf, sizeof(buf));
    printf("get config len %d, val:%s\r\n", ret, buf);
    return 0;
}
