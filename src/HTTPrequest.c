
#include "HTTPrequest.h"


void generate_request(parsed_url_t *host_server, char *request){
    strcat(request, "GET /");
    if(host_server->path != NULL){
        strcat(request, host_server->path);
    }
    strcat(request, " HTTP/1.1\r\n");

    // add Host: header
    char* host_name = host_server->host;
    strcat(request, "Host: ");
    strcat(request, host_name);
    strcat(request, "\r\n");

    // add User-Agent: header
    char * user_agent = "dccol";
    strcat(request, "User-Agent: ");
    strcat(request, user_agent);
    strcat(request, "\r\n");

    char * connection = "close";
    strcat(request, "Connection: ");
    strcat(request, connection);
    strcat(request, "\r\n\r\n");
}

void send_receive(int client_socket, char *request, parsed_url_t *host_server, char *response){

    generate_request(host_server, request);
    printf("THE REQUEST:\n%s", request);

    send(client_socket, request, sizeof(request), 0);
    printf("Request sent successfully\n");

    recv(client_socket, &response, sizeof(response), 0);
    printf("Response from the server:\n%s\n", response);

    close(client_socket);
}
/*
// generate a HTTP request from the host server information
char request[1024];
generate_request(host_server, request);
printf("The request:\n%s\n", request);

// prepare where to store the response
char response[MAX_RESPONSE_SIZE+1];

// send request
send(client_socket, request, sizeof(request), 0);
//send_receive(client_socket, request, host_server, response);

// receive response
recv(client_socket, &response, sizeof(response), 0);

printf("Response from the server:\n%s\n", response);

close(client_socket);

*/