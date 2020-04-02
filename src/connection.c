#include "connection.h"



void set_up_connection(char *url, int client_socket, deque_t *links) {
    // create the client socket to connect to the server socket
    //int client_socket;
    printf("Fetching %s\n", url);

    /** CREATING A SOCKET CONNECTION **/
    parsed_url_t *host_server = parse_url(url);

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\n Socket creation error \n");
        exit(0);
    }

    // create socket address of the server socket
    struct sockaddr_in remote_address;

    // Internet connection
    remote_address.sin_family = AF_INET;

    // HTTP port
    remote_address.sin_port = htons(80);
    // converts string address IP from command line to address structure
    // inet_aton(address, &remote_address.sin_addr.s_addr);

    // Use DNS to retrieve host attributes
    struct hostent *remote_host;
    remote_host = gethostbyname(host_server->host);

    // copy remote_host address into remote_address (socket)
    memcpy(&remote_address.sin_addr.s_addr, remote_host->h_addr, remote_host->h_length);

    // connect to the remote address
    if (connect(client_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)) < 0) {
        perror("\nConnection failed\n");
        exit(0);
    }

    /** SENDING A REQUEST **/
    // generate a HTTP request from the host server information
    char request[1024];
    generate_request(host_server, request);
    printf("\nTHE REQUEST:\n%s\n", request);

    char response[MAX_RESPONSE_SIZE+1];

    // send request
    if(send(client_socket, request, sizeof(request), 0) <0 ){
        perror("ERROR writing to socket\n");
        exit(0);
    }
    //send_receive(client_socket, request, host_server, response);

    // receive response
    // TO DO: read number of bytes in content length header
    // TO DO: separate header and body
    size_t initial_bytes_recv = 0;
    /*if(recv(client_socket, &response, sizeof(response), 0) < 0){
        perror("ERROR reading from socket\n");
        exit(0);
    }*/

    // keep recv until the header has been received
    while(initial_bytes_recv < MAX_RESPONSE_SIZE) {
        size_t chunk = recv(client_socket, &response, sizeof(response), 0);
        initial_bytes_recv += chunk;
        printf("The initial recv received %zu bytes\n", initial_bytes_recv);

        // if header is received break
        if(strstr(response, BLANK_LINE_DELIM)){
            printf("Header received!\n");
            break;
        }
    }

    /**
     * Split the initial response into a header and body
     * Allows header values to be parsed
     * Allows ability to determine whether all bytes have been read or whether further reading needs to take place
     */
    char *body = strstr(response, BLANK_LINE_DELIM);

    int header_size = strlen(response) - strlen(body);
    printf("Header size: %d\n", header_size);

    //int body_size = strlen(response) - header_size - sizeof(BLANK_LINE_DELIM) + 1; // plus 1 for '\0'?
    //printf("Body size: %d\n\n", body_size);

    char *header = (char *)malloc(header_size);
    strncpy(header, response, header_size);

    //printf("Headers:\n%s", header);
    //printf("\n");
    //printf("Current Body: %s\n", body);


    // check the response code of the response.
    char *head_copy_code = (char *)malloc(header_size);
    strncpy(head_copy_code, response, header_size);
    int code = response_code(head_copy_code);
    printf("\nCode: %d\n\n", code);
    free(head_copy_code);

    // check the content type of the response
    char *head_copy_type = (char *)malloc(header_size);
    strncpy(head_copy_type, response, header_size);
    char *type = get_content_type(head_copy_type);
    printf("Type: %s\n\n", type);
    free(head_copy_type);

    /** Now that we know the html type is valid and the code is 200
     *  Continue reading data until the size of the response buffer is equal to the content-length value
     */

    // Seg fault debug this
    // If code 200 and type MIME then proceed with ensuring all the content has been received
    char html_buffer[MAX_RESPONSE_SIZE];
    if(code == 200 && strstr(type, "text/html")) {

        //printf("Content already received: %d\n", body_size);

        // check the content-length hear to see how many bytes should have been received
        int content_length = get_content_length(header);
        printf("Content-Length: %d\n", content_length);
        free(header);

        // copying the already received bytes into a buffer of maximum response size
        strcpy(html_buffer, body);

        // create a tmp storage buffer to store new incoming chunks
        char tmp[MAX_RECV_VALUE];

        // while the size of html buffer is is less than expected content length, continue to receive data
        while (strlen(html_buffer) < content_length) {

            size_t chunk = recv(client_socket, &tmp, sizeof(tmp), 0);
            printf("chunk = %zu\n", chunk);

            // append the the new data into the main buffer
            strcat(html_buffer, tmp);

            // clean tmps memory for the next chunk of bytes
            bzero(tmp, sizeof(tmp));

            printf("Buffer length: %lu\n", strlen(html_buffer));

        }
        //printf("Final Buffer length: %d\n", buf_len);
        //printf("The entire data: %s\n", big_buffer);
        printf("All data received\n\n");

        // once all the bytes have been received, safe to close the socket
        close(client_socket);
    }

    // If the response code is not 200 or the content-type is not MIME, return, process next url
    else{
        printf("Process next url\n");
        close(client_socket);
        return;
    }


    /** PARSE THE HTML
     * to get the href to crawl through**/
     // the issue is that we are inserting incomplete urls and we dont have a fix for that yet
    GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, html_buffer, sizeof(html_buffer));

    // find the links and store in a queue
    search_for_links(output->root, links, url);

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // take each link and (recurse the process)
    while(deque_size(links)){

        // get the next link from the queue
        data_t data = deque_pop(links);
        printf("Next link: %80s\n", data.url);

        int client_socket_next;
        //set_up_connection(data.url, client_socket_next, links);

    }

}

int get_content_length(char *head){
    // get content-length from head

    char *where_content_length_is = strstr(head, "Content-Length:");
    if(where_content_length_is == NULL){
        where_content_length_is = strstr(head, "content-length:");
    }

    char *token = strtok(where_content_length_is, "\n");
    //printf("\n%s\n", token);

    token = strtok(token, " ");
    //printf("\n%s\n", token);

    token = strtok(NULL, " ");
    //printf("\n%s\n\n", token);

    if(token != NULL){
        int content_length = atoi(token);
        return content_length;
    }
    //printf("Content Length: %d\n", content_length);

    return -1;
}

char *get_content_type(char *head){
    char *where_content_type_is = strstr(head, "Content-Type:");
    if(where_content_type_is == NULL){
        where_content_type_is = strstr(head, "content-type:");
    }
    char *token = strtok(where_content_type_is, "\n");
    //printf("\n%s\n", token);

    token = strtok(token, " ");
    //printf("\n%s\n", token);

    token = strtok(NULL, " ");
    //printf("\n%s\n\n", token);

    return token;
}

int response_code(char *head){
    char *token = strtok(head, " ");

    token = strtok(NULL, " ");

    int code = atoi(token);

    return code;
}
