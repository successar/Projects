#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>

uint32_t my_n = 0, nb_n = 0, neighbour[10], clients[10], my_port;

struct rtable {
    uint32_t port;
    uint32_t cost;
    uint32_t nexthop;
};

struct RIPPkt {
    uint32_t len;
    struct rtable rt[10];
};

void show_rt( struct rtable my[] ) {
    int i;
    printf( "\nNew Route Table (Size %d) for %d is :\n", my_n, my_port );
    for( i = 0; i < my_n; i++ ) {
        printf( "Port : %d\tCost : %d\tNext Hop : %d\n", my[i].port, my[i].cost, my[i].nexthop );
    }
}

int set_server( int port ) {
    int sockfd, portno = port, i;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    int n;
    printf( "%d", IPPROTO_TCP );
    my_port = port;
    if( ( sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 ) {
        perror( "Socket" );
        exit( 1 );
    }
    bzero( ( char * ) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( portno );
    if( bind( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) ) == -1 ) {
        perror( "Bind" );
        exit( 1 );
    }
    return sockfd;
}

void read_rt( struct rtable my[], char* filename ) {
    int n = 0, i;
    FILE* fp = fopen( filename, "r" );
    fscanf( fp, "%d", &n );
    printf( "%d\n", n );
    for( i = 0; i < n; i++ ) {
        fscanf( fp, "%d %d", &my[i].port, &my[i].cost );
        my[i].nexthop = my[i].port;
        neighbour[i] = my[i].port;
        printf( "%d %d\n", my[i].port, my[i].cost );
    }
    my_n = n;
    nb_n = n;
    show_rt( my );
}

void update_rt( struct rtable my[], struct rtable up[], int p, int port ) {
    int flag = 0, i, j, n = my_n, path = 0, nexthop = port;
    printf( "\n" );
    for( i = 0; i < my_n; i++ ) {
        if( my[i].port == port ) {
            path = my[i].cost;
            nexthop = my[i].nexthop;
            break;
        }
    }
    for( i = 0; i < p; i++ ) {
        if ( up[i].port == my_port ) continue;
        up[i].cost += path;
        flag = 0;
        for( j = 0; j < n; j++ ) {
            if( up[i].port == my[j].port ) {
                flag = 1;
                if ( up[i].cost < my[j].cost ) {
                    printf( "Updating Cost of %d to %d via %d\n", up[i].port, up[i].cost, nexthop );
                    my[j].cost = up[i].cost;
                    my[j].nexthop = nexthop;
                }
                break;
            }
        }
        if( flag == 0 ) {
            printf( "Adding New Route to %d with cost %d via %d \n", up[i].port, up[i].cost, nexthop );
            my[my_n].port = up[i].port;
            my[my_n].cost = up[i].cost;
            my[my_n].nexthop = nexthop;
            my_n++;
        }
    }
    show_rt( my );
}

void send_rtable( int sockfd, struct rtable my[], struct sockaddr_in serv_addr ) {
    struct RIPPkt pkt;
    int i, n;
    pkt.len = htonl( my_n );
    for( i = 0; i < my_n; i++ ) {
        pkt.rt[i].port = htonl( my[i].port );
        pkt.rt[i].cost = htonl( my[i].cost );
        pkt.rt[i].nexthop = htonl( my[i].nexthop );
    }
    for( i = 0; i < nb_n; i++ ) {
        if ( neighbour[i] == my_port ) continue;
        serv_addr.sin_port = htons( neighbour[i] );
        n = sendto( sockfd, &pkt, sizeof( struct RIPPkt ), 0, ( const struct sockaddr * ) &serv_addr, sizeof( struct sockaddr_in ) );
    }
}

void recv_rtable( int sockfd, struct rtable my[] ) {
    struct rtable recvrt[10];
    struct RIPPkt pkt;
    int rt_n, n, i = 0, clilen = sizeof( struct sockaddr );
    struct sockaddr_in cli_addr;
    n = recvfrom( sockfd, &pkt, sizeof( struct RIPPkt ), 0, ( struct sockaddr* ) &cli_addr, &clilen );
    if( n < 0 ) {
        return;
    }
    printf( "\n" );
    rt_n = ntohl( pkt.len );
    printf( "Received Route Table of len %d from %d\n", rt_n, ntohs( cli_addr.sin_port ) );
    while( i < rt_n ) {
        recvrt[i].port = ntohl( pkt.rt[i].port );
        recvrt[i].cost = ntohl( pkt.rt[i].cost );
        i++;
    }
    update_rt( my, recvrt, rt_n, ntohs( cli_addr.sin_port ) );
}


int main( int argc, char* argv[] ) {
    struct rtable my[10];
    time_t start;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = set_server( atoi( argv[1] ) );
    read_rt( my, argv[2] );

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
    send_rtable( sockfd, my, serv_addr );
    start = time( NULL );
    while( 1 ) {
        recv_rtable( sockfd, my );
        if( ( int )( time( NULL ) - start ) >= 10 )  {
            send_rtable( sockfd, my, serv_addr );
            start = time( NULL );
        }
    }
    return 0;

}
