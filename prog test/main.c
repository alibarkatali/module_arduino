/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */
#include <stdio.h>
#include <curl/curl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    CURL *curl;
    CURLcode res;
    int fd;
    int test=0;
    char buf[255];

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    struct curl_slist *list = NULL;

    if ((fd=open("/dev/ttyACM0",O_RDWR)) == -1){
        perror("open");
        exit(-1);
    }
    while (test == 0) {
        int nb = read(fd, &buf, 255);
        buf[nb] = '\0';


        if (nb > 0) {
            printf("%s\n", buf);
            test = 1;
        }
    }

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, "http://fast-wave-77815.herokuapp.com/metrology");
        //curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/metrology");
        /* Now specify the POST data */
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"timestamp\" : \"24\",\"weather\" : {\"dfn\" : \"0\", \"weather\" : \"RAINNY\"}}");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

        list = curl_slist_append(list, "content-Type:application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}