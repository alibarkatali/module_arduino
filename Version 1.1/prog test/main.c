#include <stdio.h>
#include <curl/curl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void pushData();
int getData();
char data[255];

int main(void)
{
    while(1){
        int getTest = getData();
        if(getTest == 1){         //return 1 only when all data has been readed from com
            printf("%s\n", data);   //to verify data format
            pushData();             //send data to the server
        }
        else if(getTest == -1){ break;}
    }
    return 0;
}


int getData()
{
    // This function read the buffer char by char until '!'
    static int count = 0;
    char newChar = ' ';
    static int fd;

    if (count == 0){        // open the file only once at the beginning
        if ((fd=open("/dev/ttyACM0",O_RDWR)) == -1){
            perror("open");
            exit(-1);
            return -1;      // an error in openning com break the while(1)
        }
    }

    ssize_t nb = read(fd, &newChar, 1);
    if (nb == 1){
        if (newChar == '!') {
            data[count] = '\0';
            count = 0;
            return 1;
        }else{
            data[count] = newChar;
            count++;
            return 0;
        }
    }
    return 0;
}


void pushData(){
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

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    struct curl_slist *list = NULL;

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, "http://fast-wave-77815.herokuapp.com/metrology");
        //curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/metrology");

        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        list = curl_slist_append(list, "content-Type:application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}