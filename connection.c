#include "connection.h"

void set_up_connection(parsed_url_t *host_server) {

    // create the client socket to connect to the server socket
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
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
        printf("\nConnection failed\n");
    }

    // generate a HTTP request from the host server information
    char request[1024];
    generate_request(host_server, request);
    printf("The request:\n%s\n", request);

    // prepare where to store the response
    char response[MAX_RESPONSE_SIZE+1];

    // send request
    send(client_socket, request, sizeof(request), 0);

    // receive response
    recv(client_socket, &response, sizeof(response), 0);

    printf("Response from the server:\n%s\n", response);

    close(client_socket);
}