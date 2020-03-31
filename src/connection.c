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
    //printf("The request:\n%s\n", request);

    // prepare where to store the response
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
        printf("%zu bytes read\n", initial_bytes_recv);

        // if header is received break
        if(strstr(response, "\r\n\r\n")){
            break;
        }
    }

    printf("Header received!\n");

    // split the output into head and body
    char *body = strstr(response, "\r\n\r\n");
    // size of body = bytes read - size of header

    int header_size = strlen(response) - strlen(body);
    printf("Header size: %d\n", header_size);
    int body_size = strlen(response) - header_size - sizeof("\r\n\r\n") + 1; // plus 1 for '\0'?
    printf("Body size: %d\n", body_size);

    char *head = malloc(header_size);
    strncpy(head, response, header_size);

    printf("Head:\n%s", head);
    printf("\n");
    printf("Body: %s\n", body);

    // check the response code of the response.
    char *head_copy = malloc(header_size);
    strncpy(head_copy, response, header_size);
    int code = response_code(head_copy);
    printf("Code: %d\n", code);

    // If its 200 then proceed with ensuring all the content has been received and eventually parse
    if(code == 200) {

        printf("Content Received: %d\n", body_size);

        // check the content-length hear to see how many bytes should have been received
        int content_length = get_content_length(head);
        printf("Content-Length: %d\n", content_length);

        // check this bit
        // if the bytes (body_size) received is less than expected continue to received data until
        while (body_size < content_length) {
            printf("Content Received: %d\n", body_size);
            printf("Content-Length: %d\n", content_length);
            size_t chunk = recv(client_socket, &body, sizeof(body), 0);
            printf("chunk = %zu\n", chunk);
            body_size += chunk;
        }
        printf("exited\n");
        printf("Body: %s\n", body);

        // once all the bytes have been received, safe to close the socket
        close(client_socket);
    }

    // Otherwise skip the parse and try the next url in the queue
    else{
        return;
    }



    // split the response into headers and body (look for \r\n\r\n)
    // record the expected content length, and compare to actual content legnth
    // if(num_bytes_received == content-legnth_header) => Parse body
    //  else => recv until num_bytes_received == content-legnth_header


    /** PARSE THE HTML
     * to get the href to crawl through**/
    GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, response, sizeof(response));

    // find the links and store in a queue
    search_for_links(output->root, links);

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // take each link and (recurse the process)
    while(deque_size(links)){
        data_t data = deque_pop(links);
        printf("Next link: %80s\t", data.url);
        parsed_url_t *host_server_next = parse_url(data.url);

        /** change this logic **/
        // if the host names are the same, follow the url
        if(strcmp(host_server->host, host_server_next->host) == 0) {
            printf("Fetched\n");

            int client_socket_next;
            //set_up_connection(data.url, client_socket_next, links);
        }
        else{
            printf("Skipped\n");
        }
        // otherwise try the next url in the queue

        // handle how dup urls and urls which aren't full
        // only follow links of hosts who are the same as the current page

    }

}

int get_content_length(char *head){
    // get content-length from head
    char *where_content_length_is = strstr(head, "Content-Length:");

    char *token = strtok(where_content_length_is, "\n");
    //printf("\n%s\n", token);

    token = strtok(token, " ");
    //printf("\n%s\n", token);

    token = strtok(NULL, " ");
    //printf("\n%s\n\n", token);

    int content_length = atoi(token);
    //printf("Content Length: %d\n", content_length);

    return content_length;
}

int response_code(char *head){
    char *token = strtok(head, " ");

    token = strtok(NULL, " ");

    int code = atoi(token);

    return code;
}