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
    // TO DO: separeate header and body
    if(recv(client_socket, &response, sizeof(response), 0) < 0){
        perror("ERROR reading from socket\n");
        exit(0);
    }
    /*FILE *response_test = fdopen(client_socket, "r");
    char dest[MAX_RESPONSE_SIZE+1];
    fread(dest, 1, sizeof(response_test), response_test);

    printf("Response from the server:\n%s\n", dest);
    close(client_socket);
    //printf("Socket closed\n\n");

    // split the response into headers and body (look for \r\n\r\n)
    // record the expected content length, and compare to actual content legnth
    // if(num_bytes_received == content-legnth_header) => Parse body
    //  else => recv until num_bytes_received == content-legnth_header */


    /** PARSE THE HTML **/
    GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, response, sizeof(response));

    // find the links and store in a queue
    search_for_links(output->root, links);

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // take each link and (recurse the process)
    while(deque_size(links)){
        data_t data = deque_pop(links);
        printf("Next link: %s\n", data.url);
        parsed_url_t *host_server_next = parse_url(data.url);

        // if the host names are the same, follow the url
        if(strcmp(host_server->host, host_server_next->host) == 0) {
            printf("Success\n");

            int client_socket_next;
            set_up_connection(data.url, client_socket_next, links);
        }
        else{
            printf("Failure\n");
        }
        // otherwise try the next url in the queue

        // handle how dup urls and urls which aren't full
        // only follow links of hosts who are the same as the current page

    }

}