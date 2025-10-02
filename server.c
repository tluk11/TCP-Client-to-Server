# TCP-Client-to-Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <argp.h>
#include <netinet/in.h>  
#include <arpa/inet.h>

struct server_arguments {
	int port;
	char *salt;
	size_t salt_len;
};


error_t server_parser(int key, char *arg, struct argp_state *state) {
	struct server_arguments *args = state->input;
	error_t ret = 0;
	switch(key) {
	case 'p':
		/* Validate that port is correct and a number, etc!! */
		// done
		args->port = atoi(arg);
		if (args->port == 0) {
			argp_error(state, "Invalid option for a port, must be a number");
		}else if (args->port < 1024) {
			argp_error(state, "Port must be greater than 1024");
		}
		break;
	case 's':
		args->salt_len = strlen(arg);
		args->salt = malloc(args->salt_len+1);
		strcpy(args->salt, arg);
		break;
	default:
		ret = ARGP_ERR_UNKNOWN;
		break;
	}
	return ret;
}

void server_parseopt(int argc, char *argv[]) {
	struct server_arguments args;

	/* bzero ensures that "default" parameters are all zeroed out */
	bzero(&args, sizeof(args));



	struct argp_option options[] = {
		{ "port", 'p', "port", 0, "The port to be used for the server" ,0},
		{ "salt", 's', "salt", 0, "The salt to be used for the server. Zero by default", 0},
		{0}
	};
	struct argp argp_settings = { options, server_parser, 0, 0, 0, 0, 0 };
	if (argp_parse(&argp_settings, argc, argv, 0, NULL, &args) != 0) {
		printf("Got an error condition when parsing\n");
	}
    if (args.port == 0) {
        printf("Port is a required argument\n");
        exit(1);
    }
	/* What happens if you don't pass in parameters? Check args values
	 * for sanity and required parameters being filled in */

	/* If they don't pass in all required settings, you should detect
	 * this and return a non-zero value from main */
	printf("Got port %d and salt %s with length %ld\n", args.port, args.salt, args.salt_len);
	free(args.salt);
}

struct client_arguments {
	struct sockaddr_in server_addr;
	int hashnum;
	int smin;
	int smax;
	FILE *file; 
};

error_t client_parser(int key, char *arg, struct argp_state *state) {
	struct client_arguments *args = state->input;
	error_t ret = 0;
	int len;
    int port;
	switch(key) {
	case 'a':
		/* validate that address parameter makes sense */
		if (inet_pton(AF_INET, arg, &args->server_addr.sin_addr) <= 0) {
                argp_error(state, "Invalid IP address");
            }
            args->server_addr.sin_family = AF_INET;  // must set family
            break;
		break;
	case 'p':
		/* Validate that port is correct and a number, etc!! */
		port = atoi(arg);
		if ( port<= 0) {
			argp_error(state, "Invalid option for a port, must be a number");
		}
		break;
	case 'n':
		/* validate argument makes sense */
		args->hashnum = atoi(arg);
        if (args->hashnum < 0) {
            argp_error(state, "Number of hash requests must be a positive integer");
        }
		break;
	case 300:
		/* validate arg */
		args->smin = atoi(arg);
        if (args->smin < 0) {
            argp_error(state, "Minimum size must be a positive integer");
        }
		break;
	case 301:
		/* validate arg */
		args->smax = atoi(arg);
        if (args->smax < 0){
            argp_error(state, "Maximum size must be a positive integer");
        } else if (args->smax < args->smin) {
            argp_error(state, "Maximum size must be greater than or equal to minimum size");
        } else if (args->smax > 1048576) {
            argp_error(state, "Maximum size must be less than or equal to 2^(24)");
        }
		break;
	case 'f':   
		/* validate file */
		args->file = fopen(arg, "rb");
        if (args->file == NULL) {
            argp_error(state, "Could not open file");
            exit(1);
        }
		break;
	default:
		ret = ARGP_ERR_UNKNOWN;
		break;
	}
	return ret;
}


void client_parseopt(int argc, char *argv[]) {
	struct argp_option options[] = {
		{ "addr", 'a', "addr", 0, "The IP address the server is listening at", 0},
		{ "port", 'p', "port", 0, "The port that is being used at the server", 0},
		{ "hashreq", 'n', "hashreq", 0, "The number of hash requests to send to the server", 0},
		{ "smin", 300, "minsize", 0, "The minimum size for the data payload in each hash request", 0},
		{ "smax", 301, "maxsize", 0, "The maximum size for the data payload in each hash request", 0},
		{ "file", 'f', "file", 0, "The file that the client reads data from for all hash requests", 0},
		{0}
	};

	struct argp argp_settings = { options, client_parser, 0, 0, 0, 0, 0 };

	struct client_arguments args;
	bzero(&args, sizeof(args));

	if (argp_parse(&argp_settings, argc, argv, 0, NULL, &args) != 0) {
		printf("Got error in parse\n");
	}

	/* If they don't pass in all required settings, you should detect
	 * this and return a non-zero value from main */
	printf("Got %s on port %d with n=%d smin=%d smax=%d filename=%s\n",
	       args.ip_address, args.port, args.hashnum, args.smin, args.smax, args.filename);
	free(args.filename);
}

int main(int argc, char *argv[]) {

	if (0) {
		server_parseopt(argc, argv); //parse options as the server would
	} else {
		client_parseopt(argc, argv); //parse options as the client would
	}
	return 0;
}
