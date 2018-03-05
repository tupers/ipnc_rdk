#include <onvif.h>
int wsse_ret; 
char wsse_username[20];
char authrealm[50] = "Silvan_http_digest";

/* Ws-user authenticatio */
int silvan_authentication(struct soap *soap)
{
	const char *username;
	const char *password;
	SysInfo* oSysInfo = GetSysInfo();
	int i = 0;
	int user_authenticate=0;
	_wsse__Security *security;

	security = soap_wsse_Security(soap); /* Ws-user authentication */

	if(security) {

		username = soap_wsse_get_Username(soap);
		strcpy(wsse_username,username);

		for(i = 0; i < ACOUNT_NUM; i++)
		{
			if (strlen(oSysInfo->acounts[i].user) == 0)
			{
				user_authenticate = 0;
				break;
			}

			if(!strcmp(username, oSysInfo->acounts[i].user))
			{
				user_authenticate = 1;
				break;
			}
		}

		if (!user_authenticate)
			return 2; // no username: return FailedAuthentication

		else
		{
			password = oSysInfo->acounts[i].password; // lookup password of username
			if (soap_wsse_verify_Password(soap, password))
				return 3; // password verification failed: return FailedAuthentication

			return SOAP_OK;

		}
	}

	else 
	{
		if (soap->authrealm && soap->userid) // Digest authentication
		{
			username = soap->userid;
			for(i = 0; i < ACOUNT_NUM; i++)
			{           
				if (strlen(oSysInfo->acounts[i].user) == 0)
				{
					user_authenticate = 0;
					return 2;
				}

				if(!strcmp(username, oSysInfo->acounts[i].user))
				{
					user_authenticate = 1;
					break;
				}
			}

			if (user_authenticate) {

				password = oSysInfo->acounts[i].password; // database lookup on userid and authrealm to find passwd

				if (!strcmp(soap->authrealm, authrealm) && !strcmp(soap->userid, username))
				{ 
					if (!http_da_verify_post(soap, password)) // HTTP POST DA verification
					{
						return SOAP_OK;
					}
					return 3;
				}
			}
		}

		soap->authrealm = authrealm;
		return 401; /* Not authorized, challenge digest authentication with httpda plugin */
	}
}



