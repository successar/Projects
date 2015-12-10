#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
using namespace std;

#define BASE 6400

int my_n, n_p = 10, my_port, neighbour[10], nb_n;

struct rtable {
    uint32_t port;    uint32_t cost;    uint32_t nexthop;
};

struct LinkState {
    int len;    int port;    rtable rt[10];    int curr_cost;  bool valid;
    void print() {
        cout << "Link State Pkt : Len : " << len << " Port : " << port << endl;
        for(int i = 0; i < len; i++) {
            cout << rt[i].port << " " << rt[i].cost << " " << rt[i].nexthop << endl;
        }
    }
};

struct OutputState {
    int len;    int port;    rtable rt[10];
};

bool sort_by_cost(LinkState *s1, LinkState *s2) {
    return s1->curr_cost > s2->curr_cost;
}

int set_server( int port ) {
    int sockfd;
    struct sockaddr_in serv_addr;
    my_port = port;
    port = port + BASE;
    if( ( sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 ) {
        perror( "Socket" );
        exit( 1 );
    }
    bzero( ( char * ) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( port );
    if( bind( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) ) == -1 ) {
        perror( "Bind" );
        exit( 1 );
    }
    return sockfd;
}

void read_file(char* filename, LinkState& lss) {
    fstream fp;
    fp.open( filename );
    fp >> lss.len;
    lss.port = my_port;
    nb_n = lss.len;
    for( int j = 0; j < lss.len; j++ ) {
        fp >> lss.rt[j].port >> lss.rt[j].cost;
        lss.rt[j].nexthop = lss.rt[j].port;
        neighbour[j] = lss.rt[j].port;
    }
    lss.valid = true;
}

void send_LS( int sockfd, LinkState lss, struct sockaddr_in serv_addr, int from) {
    int n = lss.len, rp = lss.port;
    lss.len = htonl( lss.len );
    lss.port = htonl( lss.port );
    lss.curr_cost = htonl( lss.curr_cost );
    for( int i = 0; i < n; i++ ) {
        lss.rt[i].port = htonl( lss.rt[i].port );
        lss.rt[i].cost = htonl( lss.rt[i].cost );
        lss.rt[i].nexthop = htonl( lss.rt[i].nexthop );
    }

    for( int i = 0; i < nb_n; i++ ) {
        if ( neighbour[i] == rp || neighbour[i] == from ) continue;
        serv_addr.sin_port = htons( neighbour[i] + BASE );
        n = sendto( sockfd, &lss, sizeof( LinkState ), 0, ( const struct sockaddr * ) &serv_addr, sizeof( struct sockaddr_in ) );
    }
}

void recv_LS( int sockfd, LinkState lss[], struct sockaddr_in server ) {
    LinkState pkt;
    int clilen = sizeof( struct sockaddr );
    struct sockaddr_in cli_addr;
    if ( recvfrom( sockfd, &pkt, sizeof( LinkState ), 0, ( struct sockaddr* ) &cli_addr, &clilen ) < 0) return;
    printf( "\n" );
    int c = ntohl( pkt.port );
    cout << "Received Link State of " << c << " from " << htons(cli_addr.sin_port) - BASE << endl;
    if (lss[c].valid) return;
    lss[c].len = ntohl( pkt.len );
    lss[c].port = ntohl( pkt.port );
    lss[c].curr_cost = ntohl( pkt.curr_cost );
    for(int i = 0 ; i < lss[c].len; i++) {
        lss[c].rt[i].port = ntohl( pkt.rt[i].port );
        lss[c].rt[i].cost = ntohl( pkt.rt[i].cost );
        lss[c].rt[i].nexthop = ntohl( pkt.rt[i].nexthop );
    }
    lss[c].valid = true;
    lss[c].print();
    send_LS(sockfd, lss[c], server, htons(cli_addr.sin_port) - BASE);
}

void run_SPF(LinkState lss[], OutputState &out) {
    int c = my_port;
    vector<LinkState*> vertex;
    for(int i = 0; i < n_p; i++) {
        lss[i].curr_cost = 34567;
        out.rt[i].port = i;
        if (lss[i].valid) vertex.push_back(&lss[i]);
    }
    lss[c].curr_cost = 0;
    out.rt[c].cost = 0;
    out.rt[c].nexthop = c;

    while(!vertex.empty()) {
        sort(vertex.begin(), vertex.end(), sort_by_cost);
        LinkState* curr = vertex.back();
        vertex.pop_back();
        int c_port = curr->port;

        for(int i = 0; i < curr->len; i++) {
            int neigh = curr->rt[i].port;
            if( lss[neigh].valid ) {
                int nc_dist = lss[neigh].curr_cost;
                int nn_dist = curr->curr_cost + curr->rt[i].cost;
                if( nn_dist < nc_dist ) {
                    lss[neigh].curr_cost = nn_dist;
                    out.rt[neigh].cost = nn_dist;
                    out.rt[neigh].nexthop = curr->port;
                }
            }
        }

    }
    cout << "\nPort : " << c << endl;
    for(int i = 0; i < n_p; i++) {
        if (lss[i].valid)
        cout << "Dest : " << out.rt[i].port << " with Cost : " << out.rt[i].cost << " from : " << out.rt[i].nexthop << endl;
    }

}

int main(int argc, char* argv[]) {
    LinkState lss[10];
    for(int i = 0; i < 10; i++) { lss[i].valid = false; }
    OutputState out;

    time_t start;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = set_server( atoi( argv[1] ) );
    server = gethostbyname( "localhost" );
    if ( server == NULL ) {
        fprintf( stderr, "ERROR, no such host\n" );
        exit( 0 );
    }
    bzero( ( char * ) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    bcopy( ( char * )server->h_addr,
           ( char * )&serv_addr.sin_addr.s_addr,
           server->h_length );
    fcntl( sockfd, F_SETFL, O_NONBLOCK );

    read_file(argv[2], lss[my_port]);
    sleep(30);
    send_LS(sockfd, lss[my_port], serv_addr, my_port );
    start = time( NULL );
    while( 1 ) {
        recv_LS( sockfd, lss, serv_addr );
        if( ( int )( time( NULL ) - start ) >= 10 )  {
            run_SPF(lss, out);
            start = time( NULL );
        }
    }
    return 0;
}
