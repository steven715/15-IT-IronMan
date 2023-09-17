#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
int main(int argc, char **argv)
{
    unsigned int j;
    redisContext *conn;
    redisReply *reply;
    if (argc < 2)
    {
        printf("Usage: example {instance_ip_address} 6379\n");
        exit(0);
    }
    const char *hostname = argv[1];
    const int port = atoi(argv[2]);
    conn = redisConnect(hostname, port);
    if (conn == NULL || conn->err)
    {
        if (conn)
        {
            printf("Connection error: %s\n", conn->errstr);
            redisFree(conn);
        }
        else
        {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* Set */
    reply = (redisReply *)redisCommand(conn, "SET %s %s", "welcome", "Hello, DCS for Redis!");
    printf("SET: %s\n", reply->str);
    freeReplyObject(reply);

    /* Get */
    reply = (redisReply *)redisCommand(conn, "GET welcome");
    printf("GET welcome: %s\n", reply->str);
    freeReplyObject(reply);

    /* Disconnects and frees the context */
    redisFree(conn);
    return 0;
}
