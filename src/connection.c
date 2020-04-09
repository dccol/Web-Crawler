

#include "connection.h"
#include "header_parser.h"

// creates a TCP connection between the client socket and the host server socket
void set_up_connection(char *url, deque_t *links, deque_t *fetched_links) {
    // create the client socket to connect to the server socket
    //int client_socket;
    printf("%s\n", url);
    add_to_queue(fetched_links, url);

    uri_t *base = parse_uri(url);

    // CREATE THE SOCKET ----------------------------------------------------------------------------------------------

    int client_socket;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket creation error \n");
        return;
    }

    // Create socket address of the server socket
    struct sockaddr_in remote_address;

    // Internet connection
    remote_address.sin_family = AF_INET;

    // HTTP port
    remote_address.sin_port = htons(80);

    // converts string address IP from to address structure (adaptation)
    // inet_aton(address, &remote_address.sin_addr.s_addr);

    // Use DNS to retrieve host attributes
    struct hostent *remote_host;
    remote_host = gethostbyname(base->auth);



    // Copy remote_host address into remote_address (socket)
    memcpy(&remote_address.sin_addr.s_addr, remote_host->h_addr, remote_host->h_length);

    // Connect socket to the remote address
    if (connect(client_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)) < 0) {
        fprintf(stderr, "Connection failed\n");
        return;
    }

    // SEND THE REQUEST -----------------------------------------------------------------------------------------------

    // Generate a HTTP request
    char request_buffer[REQUEST_SIZE];
    bzero(request_buffer, sizeof(request_buffer));
    generate_request(base, request_buffer);
    free(base);

    char *get_request = (char*)malloc(sizeof(*get_request) * (strlen(request_buffer) + 1));
    bzero(get_request, strlen(request_buffer));

    strncat(get_request, request_buffer, strlen(request_buffer));

    // Send request
    if(send(client_socket, get_request, strlen(get_request), 0) < 0 ){
        fprintf(stderr, "ERROR writing to socket\n");
        return;
    }
    free(get_request);

    // RECEIVE RESPONSE

    char response[MAX_RESPONSE_SIZE];
    bzero(response, sizeof(response));

    size_t initial_bytes_recv = 0;
    int count = 0;

    // recv until headers have been received
    while(initial_bytes_recv < MAX_RESPONSE_SIZE) {
        size_t chunk = recv(client_socket, &response, sizeof(response), 0);
        initial_bytes_recv = initial_bytes_recv + chunk;

        if(chunk < 0){
            fprintf(stderr, "ERROR reading from socket\n");
            return;
        }
        // if chunk is zero consecutively, suggests the page may be TRUNCATED. Ignore this page
        if(chunk == 0){
            count++;

            if(count > 2) {
                return;
            }
        }
        // if header is received break loop and continue
        if(strstr(response, BLANK_LINE_DELIM)){
            break;
        }
    }

    /*
     * Split the initial response, read header values to be parsed
     * Allows ability to determine whether all bytes have been read or whether further reading needs to take place
     */
    char *body = strstr(response, BLANK_LINE_DELIM)+4;

    int header_size = strlen(response) - strlen(body) - 4;

    char *header = (char *)malloc(sizeof(*header) * (header_size + 1));
    strncpy(header, response, header_size);
    header[header_size] = '\0';


    // parse response code from header
    char *head_copy_code = (char *)malloc(sizeof(*head_copy_code) * (header_size + 1));
    strncpy(head_copy_code, response, header_size);
    head_copy_code[header_size] = '\0';
    int code = get_response_code(head_copy_code);

    // parse content type
    char *head_copy_type = (char *)malloc(sizeof(*head_copy_type) * (header_size + 1));
    strncpy(head_copy_type, response, header_size);
    head_copy_type[header_size] = '\0';
    char *type = get_content_type(head_copy_type);


     // Continue reading data until the size of the html buffer is equal to the content-length value

    char html_buffer[MAX_RESPONSE_SIZE];
    bzero(html_buffer, sizeof(html_buffer));

    // parse the content-length
    int content_length = get_content_length(header);
    free(header);

    // copying the already received bytes into it's own buffer of maximum size
    strcpy(html_buffer, body);

    // create a temp storage buffer to store new incoming chunks
    char tmp[MAX_RESPONSE_SIZE];
    bzero(tmp, sizeof(tmp));

    count = 0;

    // while the size of html buffer is is less than expected content length, continue to receive data
    while (strlen(html_buffer) < content_length) {

        size_t chunk = recv(client_socket, &tmp, sizeof(tmp), 0);

        if (chunk < 0) {
            fprintf(stderr, "ERROR reading from socket\n");
            close(client_socket);
            free(head_copy_type);
            free(head_copy_code);
            return;
        }

        // if chunk is zero consecutively, suggests the page may be TRUNCATED. Ignore this page
        if(chunk == 0){
            count++;
            if((strlen(html_buffer) < content_length) && count > 2) {
                fprintf(stderr, "Returned... Received content less than expected (truncated)... Ignore page\n");
                return;
            }
        }

        // append the the new data onto the html buffer
        strcat(html_buffer, tmp);

        bzero(tmp, sizeof(tmp));

    }

    // once all the bytes have been received, safe to close the socket
    close(client_socket);

    // CHECKS ---------------------------------------------------------------------------------------------------------

    // if the MIME type is not text/html, do not parse the page!
    if(strstr(type, "text/html") == NULL){
        return;
    }

    // if the MIME type is accepted
    else {
        // if response code is 200, indicates success
        if(code == 200) {
            fprintf(stderr, "Successful\tCode %d\n", code);
        }
        // otherwise another suggests some type of failure, however we will still parse these responses
        else{
            fprintf(stderr, "Unsuccessful\tCode %d\n", code);
        }
    }


    free(head_copy_type);
    free(head_copy_code);

    // PARSE THE HTML ("All text/html pages should be parsed and crawled")
    GumboOutput *output = gumbo_parse_with_options(&kGumboDefaultOptions, html_buffer, sizeof(html_buffer));

    // crawl the DOM for links and store in a queue
    search_for_links(output->root, links, url, fetched_links);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}
