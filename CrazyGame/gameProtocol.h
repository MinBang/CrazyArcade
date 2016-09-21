
#define CS_CONTACT_FIRST  0
#define SC_CONTACT_RFIRST 1

#define CS_LOGIN  
#define SC_RLOGIN 

typedef struct head{

	int dataType;
	int dLen;
}S_HEAD;

typedef struct contact{

	int serial;
}S_CONTACT, S_RCONTACT;

typedef struct login{

	int msg;
}S_LOGIN;