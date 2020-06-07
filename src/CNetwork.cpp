// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>

CNetwork Network;

CNetwork::CNetwork()
{
	initted = false;
	memset(&lastError, 0, sizeof(lastError));

	initted = true;
}

CNetwork::~CNetwork()
{
}

void CNetwork::Init()
{
	static bool hasInit = false;

	if (hasInit)
		return;

	memset(&lastError, 0, sizeof(lastError));
	initted = true;
	hasInit = true;

	this->mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex,NULL);

#ifdef ETH32_DEBUG
	Debug.Log("Network Initialised");
#endif
}

char *CNetwork::Get(const char *hostname, const char *path)
{
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")

	if(!initted){
		return NULL;
	}

	int sd, status, len, ix, tmp;
	struct sockaddr local, remote;
	struct hostent *hostinfo;
	fd_set ReadSet;
	struct timeval Time;

	/* Create socket */
	sd = (int)socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		sprintf(lastError, "Error creating socket\n");
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Bind socket */
	local.sa_family = AF_INET;
	memset(local.sa_data, 0, sizeof(local.sa_data));
	status = bind(sd, &local, sizeof(local));
	if(status < 0)
	{
		sprintf(lastError, "Error binding socket\n");
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Lookup host */
	hostinfo = gethostbyname(hostname);
	if(!hostinfo)
	{
		sprintf(lastError, "Error looking up host");
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* connect to host */
	remote.sa_family = hostinfo->h_addrtype;
	memcpy(remote.sa_data + 2, hostinfo->h_addr_list[0], hostinfo->h_length);
	*((short *)remote.sa_data) = 80; // using default port 80

	tmp = remote.sa_data[0];
	remote.sa_data[0] = remote.sa_data[1];
	remote.sa_data[1] = tmp;
	status = connect(sd, &remote, sizeof(remote));

	if(status != 0)
	{
		sprintf(lastError, "Error connecting to host: %s on port 80\0", hostname);
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Send GET request */
	sprintf(cmd, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: eth32\r\n\r\n\0", path, hostname);

	status = send(sd, cmd, (int)strlen(cmd), 0);
	if(status < 0)
	{
		sprintf(lastError, "Error sending GET request\0");
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Read response */
	ix = 0;
	memset(&ReadSet, 0, sizeof(ReadSet));
	FD_SET(sd, &ReadSet);
	Time.tv_sec = TIMEOUT;
	Time.tv_usec = 0;
	len = 1;

	while(len > 0 && select(sd+1, &ReadSet, 0, 0, &Time) > 0)
	{
		len = recv(sd, response + ix, sizeof(response) - ix - 1, 0);
		ix += len;
		Time.tv_sec = TIMEOUT;
		Time.tv_usec = 0;
	}
	response[ix] = 0;

	close(sd);
	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
	return response;
}

char *CNetwork::Post(const char *hostname, const char *path, const char *params)
{
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")

	if(!initted){
		return NULL;
	}

	int sd, status, len, ix, tmp;
	struct sockaddr local, remote;
	struct hostent *hostinfo;
	fd_set ReadSet;
	struct timeval Time;

	/* Create socket */
	sd = (int)socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		sprintf(lastError, "Error creating socket\n");
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Bind socket */
	local.sa_family = AF_INET;
	memset(local.sa_data, 0, sizeof(local.sa_data));
	status = bind(sd, &local, sizeof(local));
	if(status < 0)
	{
		sprintf(lastError, "Error binding socket\n");
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Lookup host */
	hostinfo = gethostbyname(hostname);
	if(!hostinfo)
	{
		sprintf(lastError, "Error looking up host: %s\0", hostname);
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* connect to host */
	remote.sa_family = hostinfo->h_addrtype;
	memcpy(remote.sa_data + 2, hostinfo->h_addr_list[0], hostinfo->h_length);
	*((short *)remote.sa_data) = 80; // using default port 80

	tmp = remote.sa_data[0];
	remote.sa_data[0] = remote.sa_data[1];
	remote.sa_data[1] = tmp;
	status = connect(sd, &remote, sizeof(remote));

	if(status != 0)
	{
		sprintf(lastError, "Error connecting to host: %s on port 80\0", hostname);
		close(sd);
		if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
		return NULL;
	}

	/* Send POST request */
	#define SEND_RQ(MSG) send(sd, MSG, (int)strlen(MSG), MSG_OOB);
	char content_header[100];

	SEND_RQ("POST ");
	SEND_RQ(path);
	SEND_RQ(" HTTP/1.0\r\n");
	SEND_RQ("Accept: */*\r\n");
	SEND_RQ("User-Agent: eth32\r\n");
	sprintf(content_header, "Content-Length: %d\r\n", strlen(params));
	SEND_RQ(content_header);
	SEND_RQ("Accept-Language: en-us\r\n");
	SEND_RQ("Accept-Encoding: gzip, deflate\r\n");
	SEND_RQ("Host: ");
	SEND_RQ(hostname);
	SEND_RQ("\r\n");
	SEND_RQ("Content-Type: application/x-www-form-urlencoded\r\n");
	SEND_RQ("\r\n");
	SEND_RQ("\r\n");
	SEND_RQ(params);
	SEND_RQ("\r\n");

	/* Read response */
	ix = 0;
	memset(&ReadSet, 0, sizeof(ReadSet));
	FD_SET(sd, &ReadSet);
	Time.tv_sec = TIMEOUT;
	Time.tv_usec = 0;
	len = 1;

	while(len > 0 && select(sd+1, &ReadSet, 0, 0, &Time) > 0)
	{
		len = recv(sd, response + ix, sizeof(response) - ix - 1, 0);
		ix += len;
		Time.tv_sec = TIMEOUT;
		Time.tv_usec = 0;
	}
	response[ix] = 0;

	close(sd);
	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
	return response;
}

bool CNetwork::is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

void CNetwork::base64_encode(char *str_to_encode, char *encoded)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	int in_len = (int)strlen(str_to_encode);

	while(in_len--)
	{
		char_array_3[i++] = *(str_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
				i = 0;
		}
	}

	if(i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for(j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';
	}

	memcpy(encoded, ret.c_str(), ret.size());
	encoded[ret.size()]=0;
}

void CNetwork::base64_decode(char *encoded, char *decoded)
{
	int in_len = (int)strlen(encoded);
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while(in_len-- && (encoded[in_] != '=') && is_base64(encoded[in_]))
	{
		char_array_4[i++] = encoded[in_]; in_++;
		if(i ==4)
		{
			for(i = 0; i <4; i++)
				char_array_4[i] = (uchar)base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for(i = 0; (i < 3); i++)
				ret += char_array_3[i];

			i = 0;
		}
	}

	if(i)
	{
		for(j = i; j <4; j++)
			char_array_4[j] = 0;

		for(j = 0; j <4; j++)
			char_array_4[j] = (uchar)base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for(j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	memcpy(decoded, ret.c_str(), ret.size());
	decoded[ret.size()]=0;
}
